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

#ifndef _DRIVER_H_
#define _DRIVER_H_

/******************************************************************************
 * Includes
 ******************************************************************************/


/******************************************************************************
 * Definitions
 ******************************************************************************/
/*
 * :NOTE:  Relevant Chrom-ART / DMA2D C definitions:  ALTIA_DRV_STM32F4XX_CHROMART[_xxx]
 * If one of the target driver supported Chrom-ART definitions is specified on the
 * build command line, make sure the proper #defines are turned on to enable the
 * target driver to build with proper Chrom-ART support.
 *
 * ALTIA_DRV_STM32F4XX_CHROMART  (default)
 *      This C define turns on driver support for the Chrom-ART.  This is the default
 *      for the target since it is the best blend of graphics performance and minimal
 *      side effects.
 *
 *      Side effects:
 *          - Since the DMA2D memory xfers are writing directly to the front framebuffer,
 *          it is possible that some tearing may be observed on the screen, depending
 *          on where the LTDC (LCD TFT Display Controller) is during it's LCD screen
 *          refresh cycle.  Typically this is not observed, except during periods
 *          of a large number of CPU intensive calculations over a large number
 *          of pixels.  This is design dependent, and can involve large images,
 *          various transforms on objects, lots of run-time alpha blending, etc.
 *
 * ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC
 *      This C define turns on driver support for the Chrom-ART and enables the
 *      target to sync up front framebuffer writes at the start of the next vertical
 *      blanking period (VSYNC).  This can be useful if large amounts of tearing
 *      is observed and thought to be a problem.  While not a guarantee that all
 *      the tearing issues will be solved, in many cases it may help alleviate it,
 *      sometimes completely.  The reason it can't be guaranteed is that the LTDC
 *      is continually going about it's business keeping the LCD refreshed and up
 *      to date with the contents of the front framebuffer.  If all the drawing
 *      operations do not complete within the vertical blanking interval, then it's
 *      possible that tearing may still be observed.
 *
 *      Side effects:
 *          - Usage of this C define will typically result in a 1-5% performance hit
 *          since the target does not start any drawing operations until the next
 *          vertical blanking period.
 *          - Tearing may still occur if the time length of all the render operations
 *          during a draw cycle exceeds the length of the vertical blanking period.
 *
 * ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE  (not recommended, historical reference only)
 *      This C define turns on driver support for the Chrom-ART and enables the
 *      target to sync up front framebuffer writes at the start of the next vertical
 *      blanking period (VSYNC).  The LTDC peripheral is temporarily disabled during
 *      all render operations, which includes writes to the front framebuffer.
 *      After all render operations are complete, the LTDC peripheral is re-enabled.
 *      This was originally done during target development to help manage tearing
 *      and squeeze all possible performance out of the DMA2D / Chrom-ART peripheral.
 *
 *      Side effects:
 *          - While tearing was eliminated using this method, several severe issues
 *          were found during system testing.  These issues can include risk of screen
 *          fading / flashing, ghosting of rendered objects, and unexpected graphical
 *          artifacts.
 *          - Using this C define will result in a ~5% performance improvement for
 *          render operations that leverage the DMA2D / Chrom-ART.
 */
#if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)
#undef ALTIA_DRV_STM32F4XX_CHROMART
#define ALTIA_DRV_STM32F4XX_CHROMART                1
#undef ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC
#define ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC     1

#elif defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC)
#undef ALTIA_DRV_STM32F4XX_CHROMART
#define ALTIA_DRV_STM32F4XX_CHROMART                1

#endif  // #if defined(ALTIA_DRV_STM32F4XX_CHROMART_WITH_VSYNC_AND_LTDC_DISABLE)


/******************************************************************************
 * Types
 ******************************************************************************/
typedef enum
{
    DRIVER_ERROR_INIT_ALTIA = 0,
    DRIVER_ERROR_INIT_DISPLAY,
    DRIVER_ERROR_STM32_HAL_DMA2D_INIT_FAILURE,
    DRIVER_ERROR_STM32_HAL_DMA2D_CONFIG_LAYER_FAILURE,
    DRIVER_ERROR_STM32_HAL_DMA2D_START_FAILURE,
    DRIVER_ERROR_STM32_HAL_DMA2D_TRANSFER_ERROR,
    DRIVER_ERROR_STM32_HAL_DMA2D_TRANSFER_TIMEOUT,

    DRIVER_ERROR_STM32_LTDC_PROGRAM_LINE_EVENT,

    DRIVER_ERROR_COUNT
} DRIVER_ERROR_T;


/******************************************************************************
 * Configuration
 ******************************************************************************/


/******************************************************************************
 * APIs
 ******************************************************************************/

/* Initialization and Termination */
extern void driver_finishInit(void);
extern BOOL driver_createGC(GCPtr gc);
extern BOOL driver_destroyGC(GCPtr gc);
extern BOOL driver_close(void);

/* Display configuration */
extern int driver_display_init(int id, int index, int width, int height);
extern int driver_display_enable(int id, int index, int enable);
extern int driver_display_color(int id, int index, unsigned long color);
extern int driver_display_close(int id, int index);

/* Layer configuration */
extern int driver_layer_init(int layer, int display);
extern int driver_layer_close(int layer);
extern int driver_layer_update(int layer, int drawn);
extern int driver_layer_makeCurrent(int layer);

/* Draw Operations */
extern void driver_prepareGraphics(void);
extern BOOL driver_StartGraphics(GCPtr gc);
extern BOOL driver_StopGraphics(GCPtr gc);
extern void driver_finishGraphics(void);

/* Dynamic Image Memory */
extern unsigned long driver_PixmapAlloc(unsigned long size);
extern void driver_PixmapFree(unsigned long addr);

/* Memory Management (system RAM) */
extern void xmemset(void * ptr, unsigned char value, size_t size);
extern void * xalloc(size_t size);
extern void xfree(void * ptr);
extern void * xrealloc(void * ptr, size_t size);
extern void * xcalloc(int num, size_t size);

/* Debugging */
#ifdef DEBUG_WINDOW
extern void driver_OpenDebugWindow();
#endif /* DEBUG_WINDOW */

/* Error reporting */
extern void driver_error(DRIVER_ERROR_T code);

#endif  // #ifndef _DRIVER_H_

