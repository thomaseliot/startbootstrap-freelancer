/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.5 $    $Date: 2009-05-06 00:03:30 $
 * Copyright (c) 2006-2008 Altia Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is 
 * hereby granted, provided that this copyright notice appears in all 
 * copies and that the entity using, copying, or modifying this software 
 * has purchased an Altia Deep Screen license. Permission to sell or 
 * distribute this source code is denied. Altia makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "fb.h"

#ifdef USE_FB_BLEND

#ifndef FB_DIRECT_COLOR_BLENDING
extern CARD32 * pDeviceClut;
#endif /* !FB_DIRECT_COLOR_BLENDING */

static unsigned int rBitsShift;
static unsigned int rBitsMask;
static unsigned int gBitsShift;
static unsigned int gBitsMask;
static unsigned int bBitsShift;
static unsigned int bBitsMask;

static int maskShift(unsigned long p)
{
    int s;

    if (!p) return 0;
    s = 0;
    while (!(p & 1))
    {
        s++;
        p >>= 1;
    }
    return s;
}

static CARD32 __b0x00(CARD32 fg, CARD32 bg)
{
    /* no blending required--return background color */
    return bg;
}

static CARD32 __b0xff(CARD32 fg, CARD32 bg)
{
    /* no blending required--return foreground color */
    return fg;
}

#ifdef __B
#undef __B
#endif /* __B */

#ifdef FB_NOALPHABLENDCACHING
/*******************************************************************************
*
* __B(alpha) - blend routine for specified alpha value
*
* This is a macro that creates a private blend function for a specific alpha
* value.
*
* NOTE:  Depending on the quality of the compiler, most operations should be
* opimized to bitwise operations.
*
*******************************************************************************/
#define __B(alpha)                                                             \
static CARD32 __b##alpha(CARD32 fg, CARD32 bg)                                 \
{                                                                              \
    CARD32 fgc, bgc, r, g, b;                                                  \
                                                                               \
    fgc = (fg >> rBitsShift) & rBitsMask;                                      \
    bgc = (bg >> rBitsShift) & rBitsMask;                                      \
    r   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    fgc = (fg >> gBitsShift) & gBitsMask;                                      \
    bgc = (bg >> gBitsShift) & gBitsMask;                                      \
    g   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    fgc = (fg >> bBitsShift) & bBitsMask;                                      \
    bgc = (bg >> bBitsShift) & bBitsMask;                                      \
    b   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    return (CARD32)                                                            \
        ((r << rBitsShift) | (g << gBitsShift) | (b << bBitsShift));           \
}

#else

/*******************************************************************************
*
* __B(alpha) - blend routine for specified alpha value
*
* This is a macro that creates a private blend function for a specific alpha
* value.
*
* NOTE:  Depending on the quality of the compiler, most operations should be
* opimized to bitwise operations.
*
*******************************************************************************/
#define __B(alpha)                                                             \
static CARD32 __b##alpha(CARD32 fg, CARD32 bg)                                 \
{                                                                              \
    CARD32 fgc, bgc, r, g, b;                                                  \
    static CARD32 pfg = ~((CARD32)0);                                          \
    static CARD32 pbg = ~((CARD32)0);                                          \
    static CARD32 ret;                                                         \
                                                                               \
    if(fg == pfg && bg == pbg)                                                 \
    {                                                                          \
         return ret;                                                           \
    }                                                                          \
                                                                               \
    fgc = (fg >> rBitsShift) & rBitsMask;                                      \
    bgc = (bg >> rBitsShift) & rBitsMask;                                      \
    r   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    fgc = (fg >> gBitsShift) & gBitsMask;                                      \
    bgc = (bg >> gBitsShift) & gBitsMask;                                      \
    g   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    fgc = (fg >> bBitsShift) & bBitsMask;                                      \
    bgc = (bg >> bBitsShift) & bBitsMask;                                      \
    b   = (fgc-bgc)*(alpha*256/255)/256 + bgc;                                 \
                                                                               \
    pfg = fg;                                                                  \
    pbg = bg;                                                                  \
    ret = (CARD32)((r << rBitsShift) | (g << gBitsShift) | (b << bBitsShift)); \
                                                                               \
    return ret;                                                                \
}
#endif /* FB_NOALPHABLENDCACHING */


/*
 * instantiate functions for alpha values that require blending
 */

/*-----*/ __B(0x01) __B(0x02) __B(0x03) __B(0x04) __B(0x05) __B(0x06) __B(0x07)
__B(0x08) __B(0x09) __B(0x0a) __B(0x0b) __B(0x0c) __B(0x0d) __B(0x0e) __B(0x0f)
__B(0x10) __B(0x11) __B(0x12) __B(0x13) __B(0x14) __B(0x15) __B(0x16) __B(0x17)
__B(0x18) __B(0x19) __B(0x1a) __B(0x1b) __B(0x1c) __B(0x1d) __B(0x1e) __B(0x1f)
__B(0x20) __B(0x21) __B(0x22) __B(0x23) __B(0x24) __B(0x25) __B(0x26) __B(0x27)
__B(0x28) __B(0x29) __B(0x2a) __B(0x2b) __B(0x2c) __B(0x2d) __B(0x2e) __B(0x2f)
__B(0x30) __B(0x31) __B(0x32) __B(0x33) __B(0x34) __B(0x35) __B(0x36) __B(0x37)
__B(0x38) __B(0x39) __B(0x3a) __B(0x3b) __B(0x3c) __B(0x3d) __B(0x3e) __B(0x3f)
__B(0x40) __B(0x41) __B(0x42) __B(0x43) __B(0x44) __B(0x45) __B(0x46) __B(0x47)
__B(0x48) __B(0x49) __B(0x4a) __B(0x4b) __B(0x4c) __B(0x4d) __B(0x4e) __B(0x4f)
__B(0x50) __B(0x51) __B(0x52) __B(0x53) __B(0x54) __B(0x55) __B(0x56) __B(0x57)
__B(0x58) __B(0x59) __B(0x5a) __B(0x5b) __B(0x5c) __B(0x5d) __B(0x5e) __B(0x5f)
__B(0x60) __B(0x61) __B(0x62) __B(0x63) __B(0x64) __B(0x65) __B(0x66) __B(0x67)
__B(0x68) __B(0x69) __B(0x6a) __B(0x6b) __B(0x6c) __B(0x6d) __B(0x6e) __B(0x6f)
__B(0x70) __B(0x71) __B(0x72) __B(0x73) __B(0x74) __B(0x75) __B(0x76) __B(0x77)
__B(0x78) __B(0x79) __B(0x7a) __B(0x7b) __B(0x7c) __B(0x7d) __B(0x7e) __B(0x7f)
__B(0x80) __B(0x81) __B(0x82) __B(0x83) __B(0x84) __B(0x85) __B(0x86) __B(0x87)
__B(0x88) __B(0x89) __B(0x8a) __B(0x8b) __B(0x8c) __B(0x8d) __B(0x8e) __B(0x8f)
__B(0x90) __B(0x91) __B(0x92) __B(0x93) __B(0x94) __B(0x95) __B(0x96) __B(0x97)
__B(0x98) __B(0x99) __B(0x9a) __B(0x9b) __B(0x9c) __B(0x9d) __B(0x9e) __B(0x9f)
__B(0xa0) __B(0xa1) __B(0xa2) __B(0xa3) __B(0xa4) __B(0xa5) __B(0xa6) __B(0xa7)
__B(0xa8) __B(0xa9) __B(0xaa) __B(0xab) __B(0xac) __B(0xad) __B(0xae) __B(0xaf)
__B(0xb0) __B(0xb1) __B(0xb2) __B(0xb3) __B(0xb4) __B(0xb5) __B(0xb6) __B(0xb7)
__B(0xb8) __B(0xb9) __B(0xba) __B(0xbb) __B(0xbc) __B(0xbd) __B(0xbe) __B(0xbf)
__B(0xc0) __B(0xc1) __B(0xc2) __B(0xc3) __B(0xc4) __B(0xc5) __B(0xc6) __B(0xc7)
__B(0xc8) __B(0xc9) __B(0xca) __B(0xcb) __B(0xcc) __B(0xcd) __B(0xce) __B(0xcf)
__B(0xd0) __B(0xd1) __B(0xd2) __B(0xd3) __B(0xd4) __B(0xd5) __B(0xd6) __B(0xd7)
__B(0xd8) __B(0xd9) __B(0xda) __B(0xdb) __B(0xdc) __B(0xdd) __B(0xde) __B(0xdf)
__B(0xe0) __B(0xe1) __B(0xe2) __B(0xe3) __B(0xe4) __B(0xe5) __B(0xe6) __B(0xe7)
__B(0xe8) __B(0xe9) __B(0xea) __B(0xeb) __B(0xec) __B(0xed) __B(0xee) __B(0xef)
__B(0xf0) __B(0xf1) __B(0xf2) __B(0xf3) __B(0xf4) __B(0xf5) __B(0xf6) __B(0xf7)
__B(0xf8) __B(0xf9) __B(0xfa) __B(0xfb) __B(0xfc) __B(0xfd) __B(0xfe) /*-----*/

#undef __B

const BLEND_FUNCTION fbblend[256] =
{
    __b0x00, __b0x01, __b0x02, __b0x03, __b0x04, __b0x05, __b0x06, __b0x07,
    __b0x08, __b0x09, __b0x0a, __b0x0b, __b0x0c, __b0x0d, __b0x0e, __b0x0f,
    __b0x10, __b0x11, __b0x12, __b0x13, __b0x14, __b0x15, __b0x16, __b0x17,
    __b0x18, __b0x19, __b0x1a, __b0x1b, __b0x1c, __b0x1d, __b0x1e, __b0x1f,
    __b0x20, __b0x21, __b0x22, __b0x23, __b0x24, __b0x25, __b0x26, __b0x27,
    __b0x28, __b0x29, __b0x2a, __b0x2b, __b0x2c, __b0x2d, __b0x2e, __b0x2f,
    __b0x30, __b0x31, __b0x32, __b0x33, __b0x34, __b0x35, __b0x36, __b0x37,
    __b0x38, __b0x39, __b0x3a, __b0x3b, __b0x3c, __b0x3d, __b0x3e, __b0x3f,
    __b0x40, __b0x41, __b0x42, __b0x43, __b0x44, __b0x45, __b0x46, __b0x47,
    __b0x48, __b0x49, __b0x4a, __b0x4b, __b0x4c, __b0x4d, __b0x4e, __b0x4f,
    __b0x50, __b0x51, __b0x52, __b0x53, __b0x54, __b0x55, __b0x56, __b0x57,
    __b0x58, __b0x59, __b0x5a, __b0x5b, __b0x5c, __b0x5d, __b0x5e, __b0x5f,
    __b0x60, __b0x61, __b0x62, __b0x63, __b0x64, __b0x65, __b0x66, __b0x67,
    __b0x68, __b0x69, __b0x6a, __b0x6b, __b0x6c, __b0x6d, __b0x6e, __b0x6f,
    __b0x70, __b0x71, __b0x72, __b0x73, __b0x74, __b0x75, __b0x76, __b0x77,
    __b0x78, __b0x79, __b0x7a, __b0x7b, __b0x7c, __b0x7d, __b0x7e, __b0x7f,
    __b0x80, __b0x81, __b0x82, __b0x83, __b0x84, __b0x85, __b0x86, __b0x87,
    __b0x88, __b0x89, __b0x8a, __b0x8b, __b0x8c, __b0x8d, __b0x8e, __b0x8f,
    __b0x90, __b0x91, __b0x92, __b0x93, __b0x94, __b0x95, __b0x96, __b0x97,
    __b0x98, __b0x99, __b0x9a, __b0x9b, __b0x9c, __b0x9d, __b0x9e, __b0x9f,
    __b0xa0, __b0xa1, __b0xa2, __b0xa3, __b0xa4, __b0xa5, __b0xa6, __b0xa7,
    __b0xa8, __b0xa9, __b0xaa, __b0xab, __b0xac, __b0xad, __b0xae, __b0xaf,
    __b0xb0, __b0xb1, __b0xb2, __b0xb3, __b0xb4, __b0xb5, __b0xb6, __b0xb7,
    __b0xb8, __b0xb9, __b0xba, __b0xbb, __b0xbc, __b0xbd, __b0xbe, __b0xbf,
    __b0xc0, __b0xc1, __b0xc2, __b0xc3, __b0xc4, __b0xc5, __b0xc6, __b0xc7,
    __b0xc8, __b0xc9, __b0xca, __b0xcb, __b0xcc, __b0xcd, __b0xce, __b0xcf,
    __b0xd0, __b0xd1, __b0xd2, __b0xd3, __b0xd4, __b0xd5, __b0xd6, __b0xd7,
    __b0xd8, __b0xd9, __b0xda, __b0xdb, __b0xdc, __b0xdd, __b0xde, __b0xdf,
    __b0xe0, __b0xe1, __b0xe2, __b0xe3, __b0xe4, __b0xe5, __b0xe6, __b0xe7,
    __b0xe8, __b0xe9, __b0xea, __b0xeb, __b0xec, __b0xed, __b0xee, __b0xef,
    __b0xf0, __b0xf1, __b0xf2, __b0xf3, __b0xf4, __b0xf5, __b0xf6, __b0xf7,
    __b0xf8, __b0xf9, __b0xfa, __b0xfb, __b0xfc, __b0xfd, __b0xfe, __b0xff
};

int fbInitAlphaBlending(
    ScreenInfo * screenInfo, unsigned long r, unsigned long g, unsigned long b)
{
    int format;

#ifndef FB_DIRECT_COLOR_BLENDING
    /*
     * r, g, b colors are indexed so lookup them up.
     */
    r = (unsigned long)pDeviceClut[r];
    g = (unsigned long)pDeviceClut[g];
    b = (unsigned long)pDeviceClut[b];
#endif /* !FB_DIRECT_COLOR_BLENDING */

    rBitsShift   = maskShift(r);
    rBitsMask    = (unsigned int)((r) >> rBitsShift);
    gBitsShift   = maskShift(g);
    gBitsMask    = (unsigned int)((g) >> gBitsShift);
    bBitsShift   = maskShift(b);
    bBitsMask    = (unsigned int)((b) >> bBitsShift);

#ifdef FB_8BIT
    /*
     * Make sure 8-bit pixmaps are supported--this is required for alpha
     * blending.
     */
    for (format = 0; format < screenInfo->numPixmapFormats; format++)
    {
        if(screenInfo->formats[format].depth == 8)
        {
            break;
        }
    }

    if(format >= screenInfo->numPixmapFormats)
    {
        return -1;
    }
#else
    return -1;
#endif

    return 0;
}

void fbGetAlphaBlending(unsigned long * rshift, unsigned long * rmask,
                        unsigned long * gshift, unsigned long * gmask,
                        unsigned long * bshift, unsigned long * bmask)
{
    if (rshift)
        *rshift = rBitsShift;

    if (gshift)
        *gshift = gBitsShift;

    if (bshift)
        *bshift = bBitsShift;

    if (rmask)
        *rmask = rBitsMask;

    if (gmask)
        *gmask = gBitsMask;

    if (bmask)
        *bmask = bBitsMask;
}
#endif /* USE_FB_BLEND */
