/**
 * @brief   Target driver graphics related functionality
 *
 * @copyright (c) 2015 Altia Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license.  Permission to sell or
 * distribute this source code is denied.
 *
 * Altia makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <string.h>

/*
 * Target driver includes
 */
#include "egl_Wrapper.h"
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "input.h"
#include "driver.h"             // :NOTE:  Must come before BSP includes due to use ALTIA_DRV_STM32F4XX_CHROMART[_xxx]
#include "os_Wrapper.h"
#include "altiaImageAccess.h"
#include "altiaImageData.h"
#include "altiaDebug.h"

/*
 * BSP includes
 */
#include "LCD.h"

#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
#include "stm32f4xx_hal.h"

#if !defined(HAL_DMA2D_MODULE_ENABLED)
#error HAL_DMA2D_MODULE_ENABLED not enabled in BSP / STM32F4 HAL!
#endif  // #if !defined(HAL_DMA2D_MODULE_ENABLED)
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)

#if IMAGE_USE_EXTERNAL_FILES
#include "FileIO.h"
#endif  // #if IMAGE_USE_EXTERNAL_FILES

/*
 * Pipeline includes
 */
#include "screenint.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "globals.h"
#include "mi.h"
#include "migc.h"
#include "fb.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/
extern uint32_t SDRAM_VRAM_BASE_ADDR;
#define LCD_VRAM_BASE_ADDR ((uint32_t)&SDRAM_VRAM_BASE_ADDR)


#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
/*
 * A timeout value of 10 milliseconds was chosen so that a DMA2D problem will
 * not cause an application to hang.  If it is desirable to wait forever, it is
 * recommended to use the STM32F4 BSP "HAL_MAX_DELAY" macro.
*/
#ifndef ALTIA_DRV_DMA2D_TIMEOUT_MILLISECONDS
#define ALTIA_DRV_DMA2D_TIMEOUT_MILLISECONDS        HAL_MAX_DELAY
#endif  // #ifndef ALTIA_DRV_DMA2D_TIMEOUT_MILLISECONDS
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)


/*
 * Macros to deal with info from the fbGetDrawable() macro into usable data.
 * The ">> 3" is just short hand for "divide by 8 (aka 2^3)".  An old school embedded
 * code optimization trick.
 */
#define CONVERT_BPP_TO_BYTES_PER_PIXEL(bpp)         (bpp >> 3)
#define CONVERT_FB_STRIDE_TO_PIXELS(fbStride, bpp)  (fbStride * ((sizeof(FbBits)) / (CONVERT_BPP_TO_BYTES_PER_PIXEL(bpp))))
#define CONVERT_FB_STRIDE_TO_BYTES(fbStride, bpp)   (CONVERT_FB_STRIDE_TO_PIXELS(fbStride,bpp) * (CONVERT_BPP_TO_BYTES_PER_PIXEL(bpp)))


/******************************************************************************
 * Function prototypes
 ******************************************************************************/
#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
static RegionPtr driver_fbCopyArea( DrawablePtr pSrcDrawable,
                                    DrawablePtr pDstDrawable,
                                    GCPtr pGC,
                                    int xIn,
                                    int yIn,
                                    int widthSrc,
                                    int heightSrc,
                                    int xOut,
                                    int yOut );
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)


/******************************************************************************
 * Global variables
 ******************************************************************************/
/*
 * These are the externals we make use from the Altia library
 */
extern AltiaDevInfo * altiaLibGetDevInfo();


/*
 * This variable noDisplayDraw indicates to AltiaGL if it is not
 * to draw to the display on refresh.  If this variable is set to 1, then
 * the SampleRedraw function will not draw directly to the display but
 * instead draw to a memory bitmap and call the CopyArea function
 * to have the bitmap blt'ed to the screen. With this variable set to 0,
 * Altia Animate functions are drawn to a memory bitmap and then blt'ed
 * to the screen (if double buffering is on).  Display refreshes are
 * drawn directly to the Display device and not sent to the display pixmap.
 * When set to a 1 the display refreshes go first to the memory pixmap and
 * then blt'ed to the display.  This allows the user to have complete
 * control on how the display frame buffer memory is accessed.
 *
 */
extern ALTIA_BOOLEAN noDisplayDraw;


#if IMAGE_USE_EXTERNAL_FILES
/*
 * The target driver currently only supports 1 image data bank file is supported.
 * If more than 1 was generated, throw a compile time error.
 */
#if (ALTIA_IMAGE_BANK_COUNT > 1)
#error Only 1 image data bank file is supported!  Please contact Altia for help.
#endif  // #if (ALTIA_IMAGE_BANK_COUNT > 1)

#ifndef ALTIA_DRV_EXTERNAL_FILE_NAME
#define ALTIA_DRV_EXTERNAL_FILE_NAME "altImg0.bin"
#endif

#define ALTIA_DRV_FILE_SYSTEM_READ_CHUNK_SIZE   (1024 * 256)  // 256 KB

static ALTIA_CONST char * ALTIA_CONST sg_pAltiaImageDataBankFiles[ALTIA_IMAGE_BANK_COUNT] =
{
    (char *) ALTIA_DRV_EXTERNAL_FILE_NAME,
};


/*
 * If "generate external resource files" was selected at code generation
 * time, the first generated resource file, altiaImageDataBank0.bin, is located
 * at the symbol "altiaImageDataBank0" per the linker config file or linker cmd line.
 */
static void * sg_pAltiaImageDataBanks[ALTIA_IMAGE_BANK_COUNT];

#endif  // #if IMAGE_USE_EXTERNAL_FILES


#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
DMA2D_HandleTypeDef Dma2dHandler;
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)

#if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)
extern LTDC_HandleTypeDef LtdcHandler;
static volatile uint32_t sg_vsyncFlag = 0;
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)


/******************************************************************************
 * Private types
 ******************************************************************************/
typedef struct
{
    ScreenPtr       pScreen;
    unsigned long   dwWidth;
    unsigned long   dwHeight;
    unsigned long   dwDepth;
    unsigned long   dwStrideBytes;
    unsigned long   dwStride;
    unsigned long   dwRedMask;
    unsigned long   dwBlueMask;
    unsigned long   dwGreenMask;
    unsigned long   redCntBits;
    unsigned long   blueCntBits;
    unsigned long   greenCntBits;
    unsigned long   redShift;
    unsigned long   greenShift;
    unsigned long   blueShift;
    unsigned long   greenMax;
    unsigned long   redMax;
    unsigned long   blueMax;
    char            * pfb;
} PrivScreenInfo;


/******************************************************************************
 * Private variables
 ******************************************************************************/
static PrivScreenInfo privScreenInfo;
static AltiaDevInfo * devInfo = NULL;

static PixmapFormatRec g_PixmapFormats[] =
{
    { 1,    1,      BITMAP_SCANLINE_PAD },
    { 8,    8,      BITMAP_SCANLINE_PAD },
    { 16,   16,     BITMAP_SCANLINE_PAD },
    { 32,   32,     BITMAP_SCANLINE_PAD }
};
const int NUMFORMATS = sizeof (g_PixmapFormats) / sizeof (g_PixmapFormats[0]);


/*
 * AltiaGL allows the user to override its default functions to support things
 * like graphics hardware acceleration, customized hardware features, or any
 * other user customization requirements.  This is done via the use of function
 * pointers.  The GC (graphics device context) contains pointers to the GCFuncs
 * and the GCOps structures defined below. The function prototypes for these
 * functions can be found in the gcstruct.h file.
 */


/**
 * @details For the GCFuncs we are setting the ValidateGC function with  the frame buffer
 * version since we will always draw to a frame buffer.  For the rest of the
 * functions we are using the machine independent versions.
 */
const GCFuncs winGCFuncs =
{
    fbValidateGC,
    miChangeClip,
    miDestroyClip,
    miCopyClip,
    {NULL}
};


/**
 * @details For the GCOps we are using the frame buffer versions of all the functions
 * except for FillPolygon since there is no frame buffer version of that
 * function. For the rest of the functions we are using the machine independent
 * versions. Here if the driver had any hardware graphics acceleration
 * available, we could change these functions to utilize it.  For example, if
 * the the display hardware can draw zero width lines (pixel width of one) we
 * would change fbPolyLine to myPolyLine.  We would then define a function that
 * would check if we are drawing a single pixel line to the display and if we
 * were we could then call the functions such that the hardware would draw the
 * line and if not then just call the fbPolyLine function to draw the line. This
 * driver might overide the fbCopyPlane function if the required (see below).
 */
GCOps winGCOps =
{
    fbFillSpans,
    fbSetSpans,
#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
    driver_fbCopyArea,
#else
    fbCopyArea,
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)
    fbCopyPlane,
    fbPolyPoint,
    fbPolyLine,
    fbPolyRectangle,
    miFillPolygon,
    fbPolyFillRect,
#ifdef ALTIA_ALPHA_BLENDING
    fbAlphaCopyArea,
#else
    {NULL},
#endif
    {NULL}
};


#ifdef ALTIA_DRV_PERFORMANCE_METRICS
static OS_timespec sg_flushStartTime = {0, 0};
#endif  // #ifdef ALTIA_DRV_PERFORMANCE_METRICS


/******************************************************************************
 * Private functions
 ******************************************************************************/
/**
 * @brief  Number of pixels to least significant one bit.  Used to figure out how to shift rgb values.
 */
static int maskShift (Pixel p)
{
    int s;

    if (!p)
    {
        return (0);
    }

    s = 0;

    while (!(p & 1))
    {
        s++;
        p >>= 1;
    }

    return (s);
}


/**
 * @brief  Counts the number of of bits in an rgb color mask
 */
static int winCountBits (unsigned long dw)
{
    int dwBits = 0;

    while (dw)
    {
        dwBits += (dw & 1);
        dw >>= 1;
    }

    return (dwBits);
}


#if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)
/**
 * @brief  vertical blanking interval (aka VSYNC) ISR callback
 * @description   This function is executed during the vertical blanking interval
 * of the display.  It's used to create a synchronization point to avoid screen
 * tearing during all draw operations to the main framebuffer (aka front buffer).
 * @param hltdc  LTDC driver construct handle
 * @return  n/a
 */
#ifdef TOOLCHAIN_IAR
__root void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef * hltdc)
#else
#error  __root keyword not supported with current tool chain!
#endif
{
    sg_vsyncFlag = 1;

    /*
     * Reset VSYNC interrupt
     */
    if(HAL_LTDC_ProgramLineEvent(hltdc, 0) != HAL_OK)
    {
        driver_error(DRIVER_ERROR_STM32_LTDC_PROGRAM_LINE_EVENT);
    }
}
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)


#if defined(ALTIA_DRV_STM32F4XX_CHROMART)
/**
 * @brief  DMA2D M2M transfer error ISR callback
 * @description  Called when an error occurs during a DMA2D memory to memory copy
 * operation
 * @param hdma2d  DMA2D driver construct handle
 * @return  n/a
 */
static void DMA2D_M2M_Error(DMA2D_HandleTypeDef * hdma2d)
{
    driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_TRANSFER_ERROR);
}


/**
 * @brief  DMA2D M2M transfer complete ISR callback
 * @description  Called when a DMA2D memory to memory copy operation has completed.
 * DMA2D M2M transfers are used to copy various scratch buffers to the active /
 * drawing / "front" framebuffer
 * @param hdma2d  DMA2D driver construct handle
 * @return  n/a
 */
static void DMA2D_M2M_Complete(DMA2D_HandleTypeDef * hdma2d)
{
    /*
     * Nothing to do
     */
}


/**
 * @brief  Custom target driver copy function that leverages the STM32F4 ChromART
 * @description  The target driver utilizes the STM32F4 ChromART DMA2D to facilitate
 * faster memory to memory (M2M) copy operations using the STM32F4's DMA2D (aka
 * ChromART) subsystem.  These copies are faster than a standard memcpy() or memmove()
 * call.  Not all copy operations use the ChromART; only those where the buffer
 * is of the correct type.
 * Since it is not possible to safely put the driver's calling thread to sleep,
 * all DMA operations are synchronous; the DMA2D registers are polled until the
 * DMA operation is complete, an error occurs, or the DMA operation times out.
 * The DMA operation timeout is configurable using the ALTIA_DRV_DMA2D_TIMEOUT_MILLISECONDS
 * preprocessor macro.
 * :NOTE:
 * fbCopyArea copies a rectangle of pixels from one drawable to another if the same
 * depth.  This function must support if the source and destination are the same
 * drawable and the rectangles overlap.
 * @param pSrcDrawable  source drawable (pixmap) ie the bits to copy
 * @param pDstDrawable  destination drawable; this could be the same or another pixmap or the display
 * @param pGC           graphics context to use
 * @param xIn           source X coordinate to copy from
 * @param yIn           source Y coordinate to copy from
 * @param srcWidth      source rectangle width to copy
 * @param srcHeight     source rectangle height to copy
 * @param xOut          destination X coordinate to copy to
 * @param yOut          destination Y coordinate to copy to
 * @return  typically NULL (hard coded if DMA2D operation was used) or fbCopyArea()
 */
static RegionPtr driver_fbCopyArea( DrawablePtr pSrcDrawable,
                                    DrawablePtr pDstDrawable,
                                    GCPtr pGC,
                                    int xIn,
                                    int yIn,
                                    int srcWidth,
                                    int srcHeight,
                                    int xOut,
                                    int yOut )
{
    HAL_StatusTypeDef halStatus;
    int  srcStride, dstStride;
    int  srcStridePixels, dstStridePixels;
    int  srcStrideBytes, dstStrideBytes;
    int  srcBpp, dstBpp;
    int  srcXoff, srcYoff, dstXoff, dstYoff;
    FbBits * p_srcBits, * p_dstBits;
    uint32_t dma2dSrcAddress, dma2dDstAddress;
    uint32_t dma2dWidth, dma2dHeight;
    int dx, dy, numRects;
    BoxRec box;
    RegionPtr cclip;

    /*
     * A drawable can be of type DRAWABLE_PIXMAP, which means it's a
     * memory pixmap, or type DRAWABLE_BUFFER, which means that it's
     * the display frame buffer.
     *
     * There are several situations where we want to let the std fbCopyArea() function
     * handle the work, since this function is geared towards blitting really fast
     * to the front buffer only:
     *  - if we're drawing to a memory pixmap
     *  - if the graphics context is not doing a boiler plate copy (GXcopy) operation;
     *    this can occur when drawing raster objects that have a transparency mask
     *    that was set using Altia Design's "set transparency" feature; refer to
     *    egl_TransBitmapBlt() for example
     */
    if (    (pDstDrawable->type != DRAWABLE_BUFFER) ||
            (pGC->alu != GXcopy) )
    {
        return (fbCopyArea(pSrcDrawable, pDstDrawable, pGC, xIn, yIn, srcWidth, srcHeight, xOut, yOut));
    }

    /*
     * Copied from fbDoCopy() to manage source, destination object extent clipping
     * The destination extent clipping is important when objects are drawn outside
     * the canvas.
     */
    xIn += pSrcDrawable->x;
    yIn += pSrcDrawable->y;

    xOut += pDstDrawable->x;
    yOut += pDstDrawable->y;

    box.x1 = xIn;
    box.y1 = yIn;
    box.x2 = xIn + srcWidth;
    box.y2 = yIn + srcHeight;

    dx = xIn - xOut;
    dy = yIn - yOut;

    /*
     * Clip the source
     */
    if (box.x1 < pSrcDrawable->x)
    {
        box.x1 = pSrcDrawable->x;
    }
    if (box.y1 < pSrcDrawable->y)
    {
        box.y1 = pSrcDrawable->y;
    }
    if (box.x2 > pSrcDrawable->x + (int) pSrcDrawable->width)
    {
        box.x2 = pSrcDrawable->x + (int) pSrcDrawable->width;
    }
    if (box.y2 > pSrcDrawable->y + (int) pSrcDrawable->height)
    {
        box.y2 = pSrcDrawable->y + (int) pSrcDrawable->height;
    }

    /* Translate and clip the dst to the destination composite clip */
    box.x1 -= dx;
    box.x2 -= dx;
    box.y1 -= dy;
    box.y2 -= dy;

    /* If the destination composite clip is one rectangle we can
       do the clip directly.  Otherwise we have to create a full
       blown region and call intersect */

    cclip = fbGetCompositeClip(pGC);
    if (1 == REGION_NUM_RECTS(cclip))
    {
        BoxPtr pBox = REGION_RECTS(cclip);

        if (box.x1 < pBox->x1) box.x1 = pBox->x1;
        if (box.x2 > pBox->x2) box.x2 = pBox->x2;
        if (box.y1 < pBox->y1) box.y1 = pBox->y1;
        if (box.y2 > pBox->y2) box.y2 = pBox->y2;
    }

    /*
     * Extract important info from the source & destination drawable buffers
     * Calculate start, end addresses from the extracted data, taking into account
     * any clipping
     *
     * Source, destination address calculations relied on the code in fbDoCopy(),
     * fbCopyRegion(), and fbBlt() to figure out how the altiaGL pipeline is handling
     * the drawables
     *
     * :NOTE:  Negative strides are not expected / supported by these calcs!
     */
    dma2dWidth = (uint32_t) (box.x2 - box.x1);
    dma2dHeight = (uint32_t) (box.y2 - box.y1);

    fbGetDrawable(pSrcDrawable, p_srcBits, srcStride, srcBpp, srcXoff, srcYoff);
    fbGetDrawable(pDstDrawable, p_dstBits, dstStride, dstBpp, dstXoff, dstYoff);

    srcStridePixels = CONVERT_FB_STRIDE_TO_PIXELS(srcStride, srcBpp);
    srcStrideBytes = CONVERT_FB_STRIDE_TO_BYTES(srcStride, srcBpp);

    dstStridePixels = CONVERT_FB_STRIDE_TO_PIXELS(dstStride, dstBpp);
    dstStrideBytes = CONVERT_FB_STRIDE_TO_BYTES(dstStride, dstBpp);

    dma2dSrcAddress = (uint32_t) p_srcBits;
    dma2dSrcAddress += ((box.y1 + dy + srcYoff) * srcStrideBytes);
    dma2dSrcAddress += ((box.x1 + dx + srcXoff) * CONVERT_BPP_TO_BYTES_PER_PIXEL(srcBpp));

    dma2dDstAddress = (uint32_t) p_dstBits;
    dma2dDstAddress += ((box.y1 + dstYoff) * dstStrideBytes);
    dma2dDstAddress += ((box.x1 + dstXoff) * CONVERT_BPP_TO_BYTES_PER_PIXEL(dstBpp));

    /*
     * The drawable contains a buffer that we can use the ChromART for
     * Go setup the DMA2D memory to memory (aka M2M) xfer operation
     */
    Dma2dHandler.Instance = DMA2D;

    /*
     * :NOTE: DMA2D output offset
     * If we get this far, assume we're writing into the framebuffer.  Since the
     * source / input data's stride will probably not match a full screen (ie the
     * entire framebuffer's) stride, take that into account here.  The output offset
     * is used to advance the destination memory address after each rectangle copy.
     *
     * :NOTE:  Negative strides are not expected / supported by these calcs!
     */
    Dma2dHandler.Init.Mode = DMA2D_M2M;
    Dma2dHandler.Init.ColorMode = DMA2D_RGB565;
    Dma2dHandler.Init.OutputOffset = (uint32_t)(((uint32_t) dstStridePixels) - dma2dWidth);

    Dma2dHandler.XferCpltCallback = DMA2D_M2M_Complete;
    Dma2dHandler.XferErrorCallback = DMA2D_M2M_Error;

    /*
     * Foreground Configuration
     * InputAlpha is fully opaque.
     *
     * :NOTE:  Negative strides are not expected / supported by these calcs!
     */
    Dma2dHandler.LayerCfg[1].InputOffset = (uint32_t)(((uint32_t) srcStridePixels) - dma2dWidth);
    Dma2dHandler.LayerCfg[1].InputColorMode = DMA2D_RGB565;
    Dma2dHandler.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    Dma2dHandler.LayerCfg[1].InputAlpha = 0x000000FF;

    /*
     * Init DMA2D (aka ChromART)
     * Kick off DMA2D operation
     * While DMA2D operation is active, make sure the LTDC peripheral is disabled
     * to prevent screen tearing
     * Then poll until the DMA xfer is complete.  The DMA needs to be a synchronous
     * operation ie wait until it's done (just like a std memcpy() call) to guarantee
     * the source data remains stable.
     */
    if (HAL_DMA2D_Init(&Dma2dHandler) != HAL_OK)
    {
        driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_INIT_FAILURE);
    }

    if (HAL_DMA2D_ConfigLayer(&Dma2dHandler, 1) != HAL_OK)
    {
        driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_CONFIG_LAYER_FAILURE);
    }

    if (HAL_DMA2D_Start(&Dma2dHandler, dma2dSrcAddress, dma2dDstAddress, dma2dWidth, dma2dHeight) != HAL_OK)
    {
        driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_START_FAILURE);
    }

    halStatus = HAL_DMA2D_PollForTransfer(&Dma2dHandler, (uint32_t) ALTIA_DRV_DMA2D_TIMEOUT_MILLISECONDS);
    switch (halStatus)
    {
        case HAL_ERROR:
        case HAL_BUSY:
        {
            driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_TRANSFER_ERROR);
            break;
        }

        case HAL_TIMEOUT:
        {
            driver_error(DRIVER_ERROR_STM32_HAL_DMA2D_TRANSFER_TIMEOUT);
            break;
        }

        default:
        {
            /*
             * Nothing to do
             * Assume HAL_OK returned
             */
            break;
        }
    }

    return (NULL);
}
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART)


/******************************************************************************
 * APIs
 ******************************************************************************/
BOOL driver_open(EGL_CHAR *name, ScreenInfo *screenInfo, ScreenPtr screen)
{
    BOOL retVal = TRUE;
    int i;
    uint32_t lcdWidth;
    uint32_t lcdHeight;
    uint32_t lcdBpp;

    screenInfo->numPixmapFormats = NUMFORMATS;

    /*
     * copying over the pixmap formats that we will support
     */
    for(i = 0; i < NUMFORMATS; i++)
    {
        screenInfo->formats[i] = g_PixmapFormats[i];
    }

    /*
     * Cache LCD info
     *
     * :NOTE: RGB565 / 16-bit color depth  (default, typically set via def'n in LCD.h)
     * This target driver was originally developed to support 32-bit color.  16-bit
     * color (RGB565) was added later once customer requirements were consolidated.
     * The BSP LCD.h file typically contains the desired color depth when the generated
     * code gets built.
     *
     * :NOTE: RGB888 / 32-bit color depth
     * Even though the screen is 24-bit color depth (ie RGB888), we still spec the
     * dwDepth param as 32 bits.  Otherwise we get unpredictable runtime behavior ie
     * some sort of weird SW reset
     */
    privScreenInfo.dwWidth = (unsigned long) C_GLCD_H_SIZE;
    privScreenInfo.dwHeight = (unsigned long) C_GLCD_V_SIZE;

#if defined(LCD_CONFIG_32_BPP)
    privScreenInfo.dwDepth  = (unsigned long) 32;

#elif defined(LCD_CONFIG_16_BPP)
    privScreenInfo.dwDepth  = (unsigned long) 16;

#else
#error LCD_CONFIG_32_BPP or LCD_CONFIG_16_BPP not defined!

#endif  // #if defined(LCD_CONFIG_32_BPP)

    privScreenInfo.pfb = (char *) LCD_VRAM_BASE_ADDR;

    privScreenInfo.dwStrideBytes = privScreenInfo.dwWidth * (privScreenInfo.dwDepth / 8);
    privScreenInfo.dwStride = (privScreenInfo.dwStrideBytes * 8) / privScreenInfo.dwDepth;

    /*
     * :NOTE:
     * Comment in if LCD buffer validation is needed
     */
//    {
//        uint32_t j;
//        uint32_t * pfb = (uint32_t *) privScreenInfo.pfb;
//        uint32_t colors[] =
//        {
//                0x00000000,  // black
//                0x00FFFFFF,  // white
//                0x00FF0000,  // red
//                0x0000FF00,  // green
//                0x000000FF,  // blue
//                0x00000000,  // black
//        };
//
//        for (i = 0; i < (sizeof(colors) / sizeof(colors[0])); i++)
//        {
//            for (j = 0; j < (privScreenInfo.dwWidth * privScreenInfo.dwHeight); j++)
//            {
//                pfb[j] = colors[i];
//            }
//        }
//    }

    /*
     * setup RGB color count, shift, and mask data
     */
#if defined(LCD_CONFIG_32_BPP)
    privScreenInfo.dwRedMask = 0x00FF0000;
    privScreenInfo.redCntBits = winCountBits(privScreenInfo.dwRedMask);
    privScreenInfo.redMax = 1 << privScreenInfo.redCntBits;
    privScreenInfo.redShift = maskShift(privScreenInfo.dwRedMask);

    privScreenInfo.dwGreenMask  = 0x0000FF00;
    privScreenInfo.greenCntBits = winCountBits(privScreenInfo.dwGreenMask);
    privScreenInfo.greenMax = 1 << privScreenInfo.greenCntBits;
    privScreenInfo.greenShift = maskShift(privScreenInfo.dwGreenMask);

    privScreenInfo.dwBlueMask   = 0x000000FF;
    privScreenInfo.blueCntBits = winCountBits(privScreenInfo.dwBlueMask);
    privScreenInfo.blueMax = 1 << privScreenInfo.blueCntBits;
    privScreenInfo.blueShift = maskShift(privScreenInfo.dwBlueMask);

#elif defined(LCD_CONFIG_16_BPP)
    privScreenInfo.dwRedMask = 0x0000F800;
    privScreenInfo.redCntBits = winCountBits(privScreenInfo.dwRedMask);
    privScreenInfo.redMax = 1 << privScreenInfo.redCntBits;
    privScreenInfo.redShift = maskShift(privScreenInfo.dwRedMask);

    privScreenInfo.dwGreenMask  = 0x000007E0;
    privScreenInfo.greenCntBits = winCountBits(privScreenInfo.dwGreenMask);
    privScreenInfo.greenMax = 1 << privScreenInfo.greenCntBits;
    privScreenInfo.greenShift = maskShift(privScreenInfo.dwGreenMask);

    privScreenInfo.dwBlueMask   = 0x0000001F;
    privScreenInfo.blueCntBits = winCountBits(privScreenInfo.dwBlueMask);
    privScreenInfo.blueMax = 1 << privScreenInfo.blueCntBits;
    privScreenInfo.blueShift = maskShift(privScreenInfo.dwBlueMask);

#else
#error LCD_CONFIG_32_BPP or LCD_CONFIG_16_BPP not defined!
#endif  // #if defined(LCD_CONFIG_32_BPP)

    /*
     * Cache the screen info
     */
    privScreenInfo.pScreen = screen;

    /*
     * initialize the screen
     */
    if ( !miScreenInit( screen,
                        privScreenInfo.pfb,
                        privScreenInfo.dwWidth,
                        privScreenInfo.dwHeight,
                        privScreenInfo.dwStride,
                        privScreenInfo.dwDepth ) )
    {
        DS_DBG_PRINT("miScreenInit failed");
        retVal = FALSE;
        goto driverOpenReturn;
    }

    /* miScreenInit sets the CreatePixmap and DestroyPixmap functions
     * to point to the machine independent versions of these functions.
     * But if we are using a frame buffer, then we want these functions
     * to use the frame buffer versions of these functions so lets
     * set that up.
     */
    screen->CreatePixmap = fbCreatePixmap;
    screen->DestroyPixmap = fbDestroyPixmap;

    /* We want to use the frame buffer version of GetSpans */
    screen->GetSpans = fbGetSpans;

    /* Set the default white and black pixel positions */
    screen->whitePixel = (Pixel) 0xffffff;
    screen->blackPixel = (Pixel) 0;

    /* Set the any offsets required for frame buffer */
    screen->xoff = 0;
    screen->yoff = 0;

    /*
     * Get device info
     */
    devInfo = altiaLibGetDevInfo();

    /*
     * initialize OS
     */
    if (!os_open())
    {
        retVal = FALSE;
        goto driverOpenReturn;
    }

    /*
     * Enable touch screen input
     */
    driver_InputConfig(privScreenInfo.dwWidth, privScreenInfo.dwHeight);
    driver_InputEnable(1);

#if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)
    /*
     * Enable VSYNC interrupts for use with Chrom-ART (aka DMA2D)
     * Set LTDC interrupt to the lowest priority and enable it
     *
     * :NOTE:  LTDC interrupt assumptions
     * Assumes the LTDC interrupt handler is properly plugged into the ISR vector
     * table
     */
    if(HAL_LTDC_ProgramLineEvent(&LtdcHandler, 0) != HAL_OK)
    {
        DS_DBG_PRINT("ERROR:  Failed to init LTDC VSYNC ISR");
        retVal = FALSE;
        goto driverOpenReturn;
    }

    HAL_NVIC_SetPriority(LTDC_IRQn, NVIC_PRIORITYGROUP_4, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)

#if IMAGE_USE_EXTERNAL_FILES
    /*
     * Init the driver specific data for external resource asset files
     */
    for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
    {
        uint32_t bytesRead;
        FILE_IO fileInfo;
        enum FILE_IO_ERROR fileError;
        BOOL tmpStatus;

        tmpStatus = FileOpen(MMC_SD_CARD, sg_pAltiaImageDataBankFiles[i], FILE_READ, &fileInfo);
        if (FALSE == tmpStatus)
        {
            DS_DBG_PRINT("ERROR:  Failed to open external resource file <%d:  %s> from SDcard.", i, sg_pAltiaImageDataBankFiles[i]);
            retVal = FALSE;
            goto driverOpenFileCleanupReturn;
        }

        /*
         * Allocate some potentially large chunks from the heap to store the entire
         * external resource file in memory to make for quick image data access.
         * Store the external resource file in memory is especially important for
         * the STM32 target since it's a low-resource target.
         */
        sg_pAltiaImageDataBanks[i] = xalloc((size_t) uAltiaImageFileSizes[i]);
        if (NULL == sg_pAltiaImageDataBanks[i])
        {
            DS_DBG_PRINT("ERROR:  Failed to allocate <%d> bytes from heap for external resource <%d:  %s> on SDcard.", uAltiaImageFileSizes[i], i, sg_pAltiaImageDataBankFiles[i]);
            retVal = FALSE;
            goto driverOpenFileCleanupReturn;
        }

        fileError = FileReadBlock(&fileInfo, sg_pAltiaImageDataBanks[i], uAltiaImageFileSizes[i], &bytesRead);
        if ((bytesRead != uAltiaImageFileSizes[i]) || (fileError != NO_FILE_ERROR))
        {
            DS_DBG_PRINT("ERROR:  Failed to read all <%d> bytes from external resource <%d:  %s> on SDcard.", uAltiaImageFileSizes[i], i, sg_pAltiaImageDataBankFiles[i]);
            retVal = FALSE;
            goto driverOpenFileCleanupReturn;
        }

        /*
         * :NOTE: work-around for BSP SDcard file system driver bug wrt reading large files
         * The BSP provided with the STM32F429II board had some issues trying to
         * read a large file directly into RAM.  The work-around below - (commented
         * out now) - is to read the file in small chunks into RAM until finished.
         * Leaving here for posterity, in case work-around is needed again in the
         * future.
         */
//        {
//            uint8_t * tmpReadBuffer = (uint8_t *) sg_pAltiaImageDataBanks[i];
//            uint32_t readBufferIndex = 0;
//            uint32_t readChunkSize;
//
//            while (readBufferIndex < uAltiaImageFileSizes[i])
//            {
//                if ((uAltiaImageFileSizes[i] - readBufferIndex) > ALTIA_DRV_FILE_SYSTEM_READ_CHUNK_SIZE)
//                {
//                    readChunkSize = ALTIA_DRV_FILE_SYSTEM_READ_CHUNK_SIZE;
//                }
//                else
//                {
//                    readChunkSize = uAltiaImageFileSizes[i] - readBufferIndex;
//                }
//
//                fileError = FileReadBlock(fileInfo, sg_pAltiaImageDataBanks[i], uAltiaImageFileSizes[i], &bytesRead);
//                if ((bytesRead != readChunkSize) || (fileError != NO_FILE_ERROR))
//                {
//                    DS_DBG_PRINT("ERROR:  Failed to read all <%d> bytes from external resource <%d:  %s> on SDcard.", uAltiaImageFileSizes[i], i, sg_pAltiaImageDataBankFiles[i]);
//                    retVal = FALSE;
//                    goto driverOpenFileCleanupReturn;
//                }
//
//                readBufferIndex += readChunkSize;
//            }
//        }

driverOpenFileCleanupReturn:

        FileClose(&fileInfo);
    }  // for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
#endif  // #if IMAGE_USE_EXTERNAL_FILES

driverOpenReturn:

    return (retVal);
}


BOOL driver_close()
{
    os_close();

    /*
     * Disable touch screen input
     */
    driver_InputEnable(0);

#if IMAGE_USE_EXTERNAL_FILES
    {
        uint32_t i;

        /*
         * Free up any allocated memory used to store external resource asset files
         */
        for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
        {
            xfree(sg_pAltiaImageDataBanks[i]);
        }
    }
#endif  // #if IMAGE_USE_EXTERNAL_FILES

    return (TRUE);
}


/**
 * @details This function is called when an new graphics device context
 * is being created.  This gives the driver a chance to initialize
 * the gc's function pointers and do any hardware dependent code
 * that might be related to a device context.
 * Here we just initialize the function pointers and make sure
 * that the miTranslate flag is not set since we don't have an
 * offset to apply to a frame buffer.
 */
BOOL driver_createGC(GCPtr pGC)
{
    pGC->ops = (GCOps *) &winGCOps;
    pGC->funcs = (GCFuncs *) &winGCFuncs;

    /* 
     * if miTranslate is set to one then the offset in the screen struct (xoff
     * and yoff) are added to coordinates we graphics are sent to the display.
     * useful if the graphics don't start at the begining of the frame buffer.
     */
    pGC->miTranslate = 0;

    return (TRUE);
}


BOOL driver_destroyGC(GCPtr pGC)
{
    /*
     * Nothing to do
     */
    return (TRUE);
}


void driver_finishInit()
{
    /*
     * Nothing to do
     */
}


BOOL driver_StartGraphics(GCPtr pGC)
{
#if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)
    /*
     * Wait until the next VSYNC interrupt occurs before doing any drawing.
     * Once we get to the next vertical blanking period, temporarily disable the
     * LTDC (LCD-TFT controller) peripheral.  This is done to enable the ChromART
     * (aka DMA2D) to do HW-based memory to memory xfers in the background without
     * causing screen tearing (and other unexpected visual artifacts) and enable
     * maximum DMA xfer efficiency by preventing internal ARM bus conflicts
     */
    sg_vsyncFlag = 0;
    while (0 == sg_vsyncFlag);
#if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)
    __HAL_LTDC_DISABLE(&LtdcHandler);
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART) && defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)

#ifdef ALTIA_DRV_PERFORMANCE_METRICS
    /*
     * Get, cache system time
     */
    int tmp;

    tmp = os_clock_gettime(OS_CLOCK_REALTIME, &sg_flushStartTime);
    if (tmp)
    {

        DS_DBG_PRINT("ERROR:  os_clock_gettime failed:  %d", tmp);
        memset(&sg_flushStartTime, 0, sizeof(sg_flushStartTime));
    }
#endif  // #ifdef ALTIA_DRV_PERFORMANCE_METRICS

    return (TRUE);
}


BOOL driver_StopGraphics(GCPtr pGC)
{
#if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)
    /*
     * Re-enable the LTDC after all drawing to the front framebuffer is complete
     */
    __HAL_LTDC_ENABLE(&LtdcHandler);
#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)

#ifdef ALTIA_DRV_PERFORMANCE_METRICS
    /*
     * Get system time, then calculate total flush time for last updates to front
     * framebuffer
     */
    OS_timespec flushEndTime;
    int tmp;

    tmp = os_clock_gettime(OS_CLOCK_REALTIME, &flushEndTime);
    if (tmp)
    {
        DS_DBG_PRINT("ERROR:  os_clock_gettime failed:  %d", tmp);
    }
    else
    {
        uint32_t flushTimeMsec = flushEndTime.tv_sec - sg_flushStartTime.tv_sec;
        flushTimeMsec *= 1000;
        flushTimeMsec += ((flushEndTime.tv_nsec - sg_flushStartTime.tv_nsec) / 1000000);
        DS_DBG_PRINT("PERF:  flushTimeMsec:  %d", flushTimeMsec);
    }

    memset(&sg_flushStartTime, 0, sizeof(sg_flushStartTime));
#endif  // #ifdef ALTIA_DRV_PERFORMANCE_METRICS

    return (TRUE);
}


/**
 * @details This routine is called whenever a device independent color needs
 *  to be converted to a device dependent or target color.  This
 *  function is called a lot and should be fast.  If color allocation
 *  is required it should first check to see if the color has already
 *  been allocated. AltiaGL keeps around the device independent colors
 *  and calls this whenever it needs what the device thinks that color
 *  should be.
 */
EGL_STATUS driver_ColorAlloc(   EGL_DEVICE_ID devID,
                                ScreenPtr pScreen,
                                EGL_ARGB *pAllocColors,
                                EGL_ORD *pClutIndexes,
                                EGL_COLOR *pUglColors,
                                EGL_SIZE numColors )
{
    int i;
    unsigned char r, g, b;
    EGL_ARGB *rgbPtr = pAllocColors;
    EGL_COLOR *cPtr = pUglColors;
    EGL_ARGB pixel;

    for (i = 0; i < numColors; i++)
    {
        unsigned long redPart;
        unsigned long greenPart;
        unsigned long bluePart;
        pixel = *rgbPtr;
        b = pixel & 0x0ff;
        g = (pixel >> 8) & 0x0ff;
        r = (pixel >> 16) & 0x0ff;
        /* Now we have to convert this to the native format
         * The formula is color/256 * max.  This is percentage of intensity
         * times the max intensity.
         */
        redPart =
            ((privScreenInfo.redMax * r) >> 8) << privScreenInfo.redShift;
        greenPart =
            ((privScreenInfo.greenMax * g) >> 8) << privScreenInfo.greenShift;
        bluePart =
            ((privScreenInfo.blueMax * b) >> 8) << privScreenInfo.blueShift;

        *cPtr = (redPart & privScreenInfo.dwRedMask) |
            (greenPart & privScreenInfo.dwGreenMask) |
            (bluePart & privScreenInfo.dwBlueMask);

        rgbPtr++;
        cPtr++;
    }

    return EGL_STATUS_OK;
}


EGL_STATUS driver_getEvent( EGL_EVENT_Q_ID eventQID,
                            EGL_EVENT *pEvent,
                            EGL_SIZE eventSize,
                            EGL_TIMEOUT timeout )
{
    EGL_STATUS retVal = EGL_STATUS_Q_EMPTY;



    return (retVal);
}


void xmemset(void * ptr, unsigned char value, size_t size)
{
    memset(ptr, value, size);
}


#ifdef DEBUG_MALLOC
static int xallocSize = 0;
static int maxAlloc = 0;
#endif /* DEBUG_MALLOC */

void * xalloc(size_t size)
{
#ifdef DEBUG_MALLOC
    int * ptr;

    xallocSize += size;

    if(xallocSize > maxAlloc)
    {
    maxAlloc = xallocSize;
    fprintf(stdout, "xalloc max %d size %d\n", maxAlloc, size);
    }
    ptr = (int *)malloc(size + 4);
    *ptr = size;

    return ((char*)ptr) + 4;
#else
    return malloc(size);
#endif  /* DEBUG_MALLOC */
}


void xfree(void * ptr)
{
#ifdef DEBUG_MALLOC
    int * p;

    p = ((int *)ptr) - 1;

    /*
     * a free occured so reduce the xallocSize by that amount.  this
     * is why we need to store the extra bytes for the size. if the
     * user knows the allocation scheme used by the OS then this might
     * not be required.
     */
    xallocSize -= *p;
    free(p);
#else
    free(ptr);
#endif  /* DEBUG_MALLOC */
}


void * xrealloc(void * ptr, size_t size)
{
#ifdef DEBUG_MALLOC
    if(ptr == nil)
    {
        int * p;

        xallocSize += size;

        p = (int *)malloc(size + 4);
        *p = size;

        if(xallocSize > maxAlloc)
        {
            maxAlloc = xallocSize;
            fprintf(stdout, "xrealloc nil max %d size %d\n", maxAlloc, size);
        }
        
        return ((char *)p) + 4;
    }
    else
    {
        int * p = ((int *)ptr) - 1;

        xallocSize -= *p;
        xallocSize += size;

        if(xallocSize > maxAlloc)
        {
            maxAlloc = xallocSize;
            fprintf(stdout, "xrealloc max %d size %d\n", maxAlloc, size);
        }

        p = (int *)realloc(p, size + 4);
        *p = size;
        return ((char *)p) + 4;
    }
#else
    return realloc(ptr, size);
#endif /* DEBUG_MALLOC */
}


void * xcalloc(int num, size_t size)
{
#ifdef DEBUG_MALLOC
    int * p;
    xallocSize += size;

    if(xallocSize > maxAlloc)
    {
        maxAlloc = xallocSize;
        fprintf(stdout, "xcalloc called max %d size %d\n", maxAlloc, size);
    }

    p = (int *)calloc(num, size + 4);
    *p = size;

    return ((char*)p) + 4;
#else
    return calloc(num, size);
#endif /* DEBUG_MALLOC */
}


#ifdef DEBUG_WINDOW
void driver_OpenDebugWindow()
{
}
#endif  // #ifdef DEBUG_WINDOW


ALTIA_WINDOW driverGetMainWindowID()
{
    return ((ALTIA_WINDOW) NULL);
}


void driver_error(DRIVER_ERROR_T code)
{
    DS_DBG_PRINT("ERROR:  %d", code);
}


/*
 * :NOTE: 3/6/15  --  Commented out __write() for now since it's not needed for this target.
 */
//size_t __write(int handle, const unsigned char * buffer, size_t size)
//{
//    /*
//     * :NOTE:
//     * Required for IAR standard libs to successfully link if putchar(), printf(),
//     * etc are called
//     *
//     * For now, this is just an empty stub
//     */
//    return (0);
//}


#if IMAGE_USE_EXTERNAL_FILES
/**
 * @brief  Called by Altia to load an external resource asset
 * @description  Load the specified external resource asset (ie the image data bank
 * file).
 * @param  filename xternal resource asset file name
 * @param  size  size of asset in bytes
 * @return  base  address of external resource asset
 */
void * driverLoadAsset(char * filename, int size)
{
    const char basename[] = "altiaImageDataBank";
    EGL_INT numPos;
    EGL_INT bank;
    void * base;

    /*
     * get the bank number from the filename
     */
    if ((void*)strstr(filename, basename) != (void*)filename)
    {
        /*
         * invalid base filename
         */
        return NULL;
    }

    numPos = (EGL_INT) strlen(basename);

    /*
     * get first digit of bank number
     * 0x30 == ASCII '0' (zero)
     */
    bank = (EGL_INT)(filename[numPos] - 0x30);

    /*
     * set the base address
     */
    if ((bank >= 0) && (bank < ALTIA_IMAGE_BANK_COUNT))
    {
        base = sg_pAltiaImageDataBanks[bank];
    }
    else
    {
        DS_DBG_PRINT("Unsupported image data bank:  %d", bank);
        base = NULL;
    }

    return base;
}


/**
 * @brief  Called by Altia post external resource loading
 * @description  Called after the external resource data is loaded (ie the image
 * data bank files).
 * @param  asset asset handle
 * @param  size  size of asset in bytes
 * @return  base address of external resource asset
 */
void * driverFinishAsset(void * asset, int size)
{
    /*
     * Nothing to do
     * Just return the base address of the asset
     */
    return (asset);
}
#endif  // #if IMAGE_USE_EXTERNAL_FILES


/**
 * @brief  Called by Altia to free external resources
 * @description  Free any allocated resources for external assets (ie the image
 * data bank files).
 * @param  asset asset handle
 * @param  size  size of asset in bytes
 * @return  n/a
 */
void driverFreeAsset(void * asset, int size)
{
    /*
     * Nothing to do
     */
}

