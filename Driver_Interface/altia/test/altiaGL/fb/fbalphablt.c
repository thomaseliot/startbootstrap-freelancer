/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.4 $    $Date: 2009-05-06 00:03:30 $
 * Copyright © 2006 Altia, Inc.
 * Copyright © 1998 Keith Packard
 *
 * This file has been created by Altia Inc. based on original work by Keith
 * Packard.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
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
#include <stdio.h>

#define NO_ASSERT
#ifndef NO_ASSERT
#include <assert.h>
#endif /* NO_ASSERT */

#ifdef USE_FB_BLEND

#ifndef FB_DIRECT_COLOR_BLENDING
    #ifdef FB_16BIT
        #undef FB_16BIT
    #endif /* FB_16BIT */

    #ifdef FB_24BIT
        #undef FB_24BIT
    #endif /* FB_24BIT */

    #ifdef FB_32BIT
        #undef FB_32BIT
    #endif /* FB_32BIT */
#endif /* FB_DIRECT_COLOR_BLENDING */

/*******************************************************************************
*
* 8 bits per pixel alpha blending color support
*
*******************************************************************************/

#ifdef FB_8BIT
#ifdef FBALPHABLT_8_16_32
#undef FBALPHABLT_8_16_32
#endif

#ifdef FBTRANSBLT_8_16_32
#undef FBTRANSBLT_8_16_32
#endif

#ifdef FbDoAlphaBlend
#undef FbDoAlphaBlend
#endif

#ifdef FbDoAlphaBlendReverse
#undef FbDoAlphaBlendReverse
#endif

#ifdef FbDoRightMaskAlphaBlend
#undef FbDoRightMaskAlphaBlend
#endif

#ifdef FbDoLeftMaskAlphaBlend
#undef FbDoLeftMaskAlphaBlend
#endif

#ifdef FbDoTransBlend
#undef FbDoTransBlend
#endif

#ifdef FbDoTransBlendReverse
#undef FbDoTransBlendReverse
#endif

#ifdef FbDoRightMaskTransBlend
#undef FbDoRightMaskTransBlend
#endif

#ifdef FbDoLeftMaskTransBlend
#undef FbDoLeftMaskTransBlend
#endif

#define FBALPHABLT_8_16_32 fbAlphaBlt8
#define FBTRANSBLT_8_16_32 fbTransBlt8

extern CARD32 * pDeviceClut;
extern CARD32 driver_BlendedIndexColorAlloc(CARD32);

#if FB_SHIFT == 3
/*******************************************************************************
*
* FbDoAlphaBlend(a,fg,bg,d) - blends 1 8-bit indexed pixel.
*
*    +-------------+
*    | INDEX VALUE | --->
*    +-------------+
*        (8 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 8-bit indexed pixel data
* bg = address to destination (background) 8-bit indexed pixel data
* d  = address to a 8 bit buffer to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlend(a,fg,bg,d)                                              \
{                                                                              \
    CARD32 __rgb = (CARD32)FBBLEND(*(a),                                       \
        (CARD32)(pDeviceClut[*(fg)]), (CARD32)(pDeviceClut[*(bg)]));           \
    *(d) = (CARD8)driver_BlendedIndexColorAlloc(__rgb);                        \
}

#define FbDoAlphaBlendReverse(a,fg,bg,d)   FbDoAlphaBlend(a,fg,bg,d)
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b) FbDoAlphaBlend(a,fg,bg,d)
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)  FbDoAlphaBlend(a,fg,bg,d)

/*******************************************************************************
*
* FbDoTransBlend(a,fg,bg,d) - blends 1 8-bit alpha pixel.
*
*    +-------------+
*    | INDEX VALUE | --->
*    +-------------+
*        (8 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 8-bit alpha pixel data
* bg = address to destination (background) 8-bit alpha pixel data
* d  = address to a 8 bit buffer to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlend(a,fg,bg,d)                                              \
{                                                                              \
    if (*(a) && (*(bg) < 255)) {                                               \
        if (*(bg) && (*(a) < 255))                                             \
            *(d) = (CARD8)((CARD16)*(bg) + (CARD16)*(a)) -                     \
                         (((CARD16)*(bg) * (CARD16)*(a)) / 255);               \
        else                                                                   \
            *(d) = *(a);                                                       \
    } else                                                                     \
        *(d) = *(bg);                                                          \
}

#define FbDoTransBlendReverse(a,fg,bg,d)   FbDoTransBlend(a,fg,bg,d)
#define FbDoRightMaskTransBlend(a,fg,bg,d,b) FbDoTransBlend(a,fg,bg,d)
#define FbDoLeftMaskTransBlend(a,fg,bg,d,b)  FbDoTransBlend(a,fg,bg,d)
#endif /* FB_SHIFT == 3 */

#if FB_SHIFT == 5
#define BITSGET_B08(s)   ( (s)        & (CARD8)0xff)
#define BITSGET_B18(s)   (((s) >>  8) & (CARD8)0xff)
#define BITSGET_B28(s)   (((s) >> 16) & (CARD8)0xff)
#define BITSGET_B38(s)    ((s) >> 24)

#define BITSSTORE_B08(d)  ((d) & (CARD8)0xff)
#define BITSSTORE_B18(d) (((d) & (CARD8)0xff) <<  8)
#define BITSSTORE_B28(d) (((d) & (CARD8)0xff) << 16)
#define BITSSTORE_B38(d)  ((d)                << 24)

/* For 8-bits the entire destination 'd' is an alpha value */
#define BITSBLEND(a,bg,d)                                                      \
{                                                                              \
    if (a) {                                                                   \
        if ((bg) && ((a) < 255)) {                                             \
            if ((bg) < 255)                                                    \
                d = (CARD8)((bg) + (a)) - (((bg) * (a)) / 255);                \
            else                                                               \
                d = (bg);                                                      \
        } else {                                                               \
            d = (a);                                                           \
        }                                                                      \
    }                                                                          \
    else {                                                                     \
        d = (bg);                                                              \
    }                                                                          \
}

/* For 8-bit Alpha, we always use transparent blend (destination has alpha) */
#define FbDoAlphaBlend(a,fg,bg,d)            FbDoTransBlend(a,fg,bg,d)
#define FbDoAlphaBlendReverse(a,fg,bg,d)     FbDoTransBlendReverse(a,fg,bg,d)
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b) FbDoRightMaskTransBlend(a,fg,bg,d,b)
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)  FbDoLeftMaskTransBlend(a,fg,bg,d,b)

/*******************************************************************************
*
* FbDoTransBlend(a,fg,bg,d) - blends 4 8-bit ALPHA pixels.
*
*     B08         B38
*    +---+---+---+---+
*    | A | A | A | A | --->
*    +---+---+---+---+
*        (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 8-bit alpha pixel data
* bg = address to destination (background) 8-bit alpha pixel data
* d  = address to a 8 bit buffer to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlend(a,fg,bg,d)                                              \
{                                                                              \
    register CARD8 __bg, __res;                                                \
    register FbBits __bits;                                                    \
                                                                               \
    __bg = (CARD8)BITSGET_B08(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits = BITSSTORE_B08(__res);                                             \
    a++;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B18(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B18(__res);                                            \
    a++;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B28(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B28(__res);                                            \
    a++;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B38(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B38(__res);                                            \
                                                                               \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoTransBlendReverse(a,fg,bg,d) - reverse blends 4 8-bit ALPHA pixels
*
*     B08         B38
*    +---+---+---+---+
*    | A | A | A | A | <---
*    +---+---+---+---+
*        (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlendReverse(a,fg,bg,d)                                       \
{                                                                              \
    register CARD8 __bg, __res;                                                \
    register FbBits __bits;                                                    \
                                                                               \
    __bg = (CARD8)BITSGET_B38(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits = BITSSTORE_B38(__res);                                             \
    a--;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B28(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B28(__res);                                            \
    a--;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B18(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B18(__res);                                            \
    a--;                                                                       \
                                                                               \
    __bg = (CARD8)BITSGET_B08(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits |= BITSSTORE_B08(__res);                                            \
                                                                               \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoRightMaskTransBlend(a,fg,bg,d,b) - blends right masked 8-bit ALPHA pixels
*
*     B08         B38
*    +---+---+---+---+
*    | A | A | A | x | --->
*    +---+---+---+---+
*        (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = bytes identifier as follows (as taken from FbDoRightMaskByteRRop in fbrop.h)
    1 = 08_xx_xx_xx
    2 = 08_18_xx_xx
    3 = 08_18_28_xx (default)
*
*******************************************************************************/
#define FbDoRightMaskTransBlend(a,fg,bg,d,b)                                   \
{                                                                              \
    register CARD8 __bg, __res;                                                \
    register FbBits __bits;                                                    \
                                                                               \
    __bg = (CARD8)BITSGET_B08(*(bg));                                          \
    BITSBLEND((*(a)), __bg, __res);                                            \
    __bits = BITSSTORE_B08(__res);                                             \
    if (b > 1)                                                                 \
    {                                                                          \
        a++;                                                                   \
        __bg = (CARD8)BITSGET_B18(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits |= BITSSTORE_B18(__res);                                        \
        if (b > 2)                                                             \
        {                                                                      \
            a++;                                                               \
            __bg = (CARD8)BITSGET_B28(*(bg));                                  \
            BITSBLEND((*(a)), __bg, __res);                                    \
            __bits |= BITSSTORE_B28(__res);                                    \
        }                                                                      \
    }                                                                          \
                                                                               \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoLeftMaskTransBlend(a,fg,bg,d,b) - blends left masked 8-bit ALPHA pixels
*
*     B08         B38
*    +---+---+---+---+
*    | x | A | A | A | --->
*    +---+---+---+---+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = bytes identifier as follows (as taken from FbDoLeftMaskByteRRop in fbrop.h)
    1 = xx_18_28_38 (default)
    2 = xx_xx_28_38
    3 = xx_xx_xx_38
    5 = xx_18_xx_xx
    6 = xx_xx_28_xx
    9 = xx_18_28_xx
*
*******************************************************************************/
#define FbDoLeftMaskTransBlend(a,fg,bg,d,b)                                    \
{                                                                              \
    register CARD8 __bg, __res;                                                \
    register FbBits __bits;                                                    \
    switch(b)                                                                  \
    {                                                                          \
    case 2: /* xx_xx_28_38 */                                                  \
        __bg = (CARD8)BITSGET_B28(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B28(__res);                                         \
        a++;                                                                   \
        __bg = (CARD8)BITSGET_B38(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits |= BITSSTORE_B38(__res);                                        \
        break;                                                                 \
    case 3: /* xx_xx_xx_38 */                                                  \
        __bg = (CARD8)BITSGET_B38(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B38(__res);                                         \
        break;                                                                 \
    case 5: /* xx_18_xx_xx */                                                  \
        __bg = (CARD8)BITSGET_B18(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B18(__res);                                         \
        break;                                                                 \
    case 6: /* xx_xx_28_xx */                                                  \
        __bg = (CARD8)BITSGET_B28(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B28(__res);                                         \
        break;                                                                 \
    case 9: /* xx_18_28_xx */                                                  \
        __bg = (CARD8)BITSGET_B18(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B18(__res);                                         \
        a++;                                                                   \
        __bg = (CARD8)BITSGET_B28(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits |= BITSSTORE_B28(__res);                                        \
        break;                                                                 \
    default: /* xx_18_28_38 */                                                 \
        __bg = (CARD8)BITSGET_B18(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits = BITSSTORE_B18(__res);                                         \
        a++;                                                                   \
        __bg = (CARD8)BITSGET_B28(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits |= BITSSTORE_B28(__res);                                        \
        a++;                                                                   \
        __bg = (CARD8)BITSGET_B38(*(bg));                                      \
        BITSBLEND((*(a)), __bg, __res);                                        \
        __bits |= BITSSTORE_B38(__res);                                        \
        break;                                                                 \
    }                                                                          \
                                                                               \
    *(d) = __bits;                                                             \
}
#endif /* FB_SHIFT == 5 */

#include "fbalphablt.h"
#endif /* FB_8BIT */

/*******************************************************************************
*
* 16 bits per pixel alpha blending color support
*
*******************************************************************************/

#ifdef FB_16BIT
#ifdef FBALPHABLT_8_16_32
#undef FBALPHABLT_8_16_32
#endif

#ifdef FBTRANSBLT_8_16_32
#undef FBTRANSBLT_8_16_32
#endif

#ifdef FbDoAlphaBlend
#undef FbDoAlphaBlend
#endif

#ifdef FbDoAlphaBlendReverse
#undef FbDoAlphaBlendReverse
#endif

#ifdef FbDoRightMaskAlphaBlend
#undef FbDoRightMaskAlphaBlend
#endif

#ifdef FbDoLeftMaskAlphaBlend
#undef FbDoLeftMaskAlphaBlend
#endif

#ifdef FbDoTransBlend
#undef FbDoTransBlend
#endif

#ifdef FbDoTransBlendReverse
#undef FbDoTransBlendReverse
#endif

#ifdef FbDoRightMaskTransBlend
#undef FbDoRightMaskTransBlend
#endif

#ifdef FbDoLeftMaskTransBlend
#undef FbDoLeftMaskTransBlend
#endif

#define FBALPHABLT_8_16_32 fbAlphaBlt16
#define FBTRANSBLT_8_16_32 fbTransBlt16

#if FB_SHIFT == 5
#define BITSGET_L16(s)   ((s) & (CARD16)0xffff)
#define BITSGET_H16(s)   ((s) >> 16)

#define BITSSTORE_L16(d) ((d) & (CARD16)0xffff)
#define BITSSTORE_H16(d) ((d) << 16)

#define BITSBLEND(a,fg,bg,d)                                                   \
{                                                                              \
    if (a) {                                                                   \
        CARD16 ba = (CARD16)(((bg) & ALPHA_MASK_16) >> ALPHA_SHIFT_16);        \
        if (ba && ((a) < 255)) {                                               \
            d = (CARD16)FBBLEND((a), (CARD32)(fg), (CARD32)(bg));              \
            if (ba < 255)                                                      \
                ba = (CARD16)(ba + (a)) - ((ba * (a)) / 255);                  \
            d |= ((ba << ALPHA_SHIFT_16) & ALPHA_MASK_16);                     \
        } else {                                                               \
            d = fg;                                                            \
            d |= (((a) << ALPHA_SHIFT_16) & ALPHA_MASK_16);                    \
        }                                                                      \
    } else                                                                     \
        d = bg;                                                                \
}

/*******************************************************************************
*
* FbDoAlphaBlend(a,fg,bg,d) - blends 2 16-bit RGB pixels.
*
*       L     H
*    +-----+-----+
*    | RGB | RGB | --->
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlend(a,fg,bg,d)                                              \
{                                                                              \
    register FbBits __bits;                                                    \
    __bits = BITSSTORE_L16((CARD16)FBBLEND(*(a),                               \
        (CARD32)(BITSGET_L16(*(fg))), (CARD32)(BITSGET_L16(*(bg)))));          \
    __bits |= BITSSTORE_H16((CARD16)FBBLEND(*(++(a)),                          \
        (CARD32)(BITSGET_H16(*(fg))), (CARD32)(BITSGET_H16(*(bg)))));          \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoAlphaBlendReverse(a,fg,bg,d) - reverse blends 2 16-bit RGB pixels
*
*       L     H
*    +-----+-----+
*    | RGB | RGB | <---
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendReverse(a,fg,bg,d)                                       \
{                                                                              \
    register FbBits __bits;                                                    \
    __bits = BITSSTORE_H16((CARD16)FBBLEND(*(a),                               \
        (CARD32)(BITSGET_H16(*(fg))), (CARD32)(BITSGET_H16(*(bg)))));          \
    __bits |= BITSSTORE_L16((CARD16)FBBLEND(*(--(a)),                          \
        (CARD32)(BITSGET_L16(*(fg))), (CARD32)(BITSGET_L16(*(bg)))));          \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoRightMaskAlphaBlend(a,fg,bg,d,b) - blends a right masked 16-bit RGB pixel
*
*       L     H
*    +-----+-----+
*    | RGB | xxx |
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = unused
*
*******************************************************************************/
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b)                                   \
{                                                                              \
    *(d) = BITSSTORE_L16((CARD16)FBBLEND(*(a),                                 \
        (CARD32)(BITSGET_L16(*(fg))), (CARD32)(BITSGET_L16(*(bg)))));          \
}

/*******************************************************************************
*
* FbDoLeftMaskAlphaBlend(a,fg,bg,d,b) - blends a left masked 16-bit RGB pixel
*
*       L     H
*    +-----+-----+
*    | xxx | RGB |
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = unused
*
*******************************************************************************/
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)                                    \
{                                                                              \
    *(d) = BITSSTORE_H16((CARD16)FBBLEND(*(a),                                 \
        (CARD32)(BITSGET_H16(*(fg))), (CARD32)(BITSGET_H16(*(bg)))));          \
}

/*******************************************************************************
*
* FbDoTransBlend(a,fg,bg,d) - blends 2 16-bit RGB pixels.
*
*       L     H
*    +-----+-----+
*    | RGB | RGB | --->
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlend(a,fg,bg,d)                                              \
{                                                                              \
    register CARD16 __fg, __bg, __res;                                         \
    register FbBits __bits;                                                    \
    __fg = (CARD16)BITSGET_L16(*(fg));                                         \
    __bg = (CARD16)BITSGET_L16(*(bg));                                         \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    __bits = BITSSTORE_L16(__res);                                             \
    __fg = (CARD16)BITSGET_H16(*(fg));                                         \
    __bg = (CARD16)BITSGET_H16(*(bg));                                         \
    a++;                                                                       \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    __bits |= BITSSTORE_H16(__res);                                            \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoTransBlendReverse(a,fg,bg,d) - reverse blends 2 16-bit RGB pixels
*
*       L     H
*    +-----+-----+
*    | RGB | RGB | <---
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlendReverse(a,fg,bg,d)                                       \
{                                                                              \
    register CARD16 __fg, __bg, __res;                                         \
    register FbBits __bits;                                                    \
    __fg = (CARD16)BITSGET_H16(*(fg));                                         \
    __bg = (CARD16)BITSGET_H16(*(bg));                                         \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    __bits = BITSSTORE_H16(__res);                                             \
    __fg = (CARD16)BITSGET_L16(*(fg));                                         \
    __bg = (CARD16)BITSGET_L16(*(bg));                                         \
    a--;                                                                       \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    __bits |= BITSSTORE_L16(__res);                                            \
    *(d) = __bits;                                                             \
}

/*******************************************************************************
*
* FbDoRightMaskTransBlend(a,fg,bg,d,b) - blends a right masked 16-bit RGB pixels
*
*       L     H
*    +-----+-----+
*    | RGB | xxx |
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = unused
*
*******************************************************************************/
#define FbDoRightMaskTransBlend(a,fg,bg,d,b)                                   \
{                                                                              \
    register CARD16 __fg, __bg, __res;                                         \
    __fg = (CARD16)BITSGET_L16(*(fg));                                         \
    __bg = (CARD16)BITSGET_L16(*(bg));                                         \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    *(d) = BITSSTORE_L16(__res);                                               \
}

/*******************************************************************************
*
* FbDoLeftMaskTransBlend(a,fg,bg,d,b) - blends a left masked 16-bit RGB pixels
*
*       L     H
*    +-----+-----+
*    | xxx | RGB |
*    +-----+-----+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
* b  = unused
*
*******************************************************************************/
#define FbDoLeftMaskTransBlend(a,fg,bg,d,b)                                    \
{                                                                              \
    register CARD16 __fg, __bg, __res;                                         \
    __fg = (CARD16)BITSGET_H16(*(fg));                                         \
    __bg = (CARD16)BITSGET_H16(*(bg));                                         \
    BITSBLEND((*(a)), __fg, __bg, __res);                                      \
    *(d) = BITSSTORE_H16(__res);                                               \
}
#endif /* FB_SHIFT == 5 */

#if FB_SHIFT == 4
/*******************************************************************************
*
* FbDoAlphaBlend(a,fg,bg,d) - blends 1 16-bit RGB pixel.
*
*    +-----------+
*    |    RGB    |
*    +-----------+
*      (16 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 16 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlend(a,fg,bg,d)                                              \
{                                                                              \
    *(d) = (CARD16)FBBLEND(*(a), (CARD32)(*(fg)), (CARD32)(*(bg)));            \
}

#define FbDoAlphaBlendReverse(a,fg,bg,d)     FbDoAlphaBlend(a,fg,bg,d)
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b) FbDoAlphaBlend(a,fg,bg,d)
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)  FbDoAlphaBlend(a,fg,bg,d)

/*******************************************************************************
*
* FbDoTransBlend(a,fg,bg,d) - blends 1 16-bit ARGB pixel.
*
*    +-----------+
*    |    RGB    |
*    +-----------+
*      (16 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 16 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlend(a,fg,bg,d)                                              \
{                                                                              \
    if (*(a)) {                                                                \
        CARD16 ba = (CARD16)(((*((CARD16 *)(bg))) & ALPHA_MASK_16) >>          \
                             ALPHA_SHIFT_16);                                  \
        if (ba && (*(a) < 255)) {                                              \
            *(d) = (CARD16)FBBLEND(*(a), (CARD32)(*(fg)), (CARD32)(*(bg)));    \
            if (ba < 255)                                                      \
                ba = (CARD16)(ba + *(a)) - ((ba * (*(a))) / 255);              \
            *((CARD16 *)(d)) |= ((ba << ALPHA_SHIFT_16) & ALPHA_MASK_16);      \
        } else {                                                               \
            *((CARD16 *)(d)) = *((CARD32 *)(fg));                              \
            *((CARD16 *)(d)) |= ((*(a)) << ALPHA_SHIFT_16) & ALPHA_MASK_16);   \
        }                                                                      \
    } else                                                                     \
        *((CARD16 *)(d)) = *((CARD16 *)(bg));                                  \
}

#define FbDoAlphaBlendReverse(a,fg,bg,d)     FbDoTransBlend(a,fg,bg,d)
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b) FbDoTransBlend(a,fg,bg,d)
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)  FbDoTransBlend(a,fg,bg,d)
#endif /* FB_SHIFT == 4 */

#include "fbalphablt.h"
#endif /* FB_16BIT */

/*******************************************************************************
*
* 24 bits per pixel alpha blending color support
*
*******************************************************************************/

#ifdef FB_24BIT
#if FB_SHIFT == 5
#define FbDoTypeStore(b,t,x,s)  (*((t *) (b)) = (x) >> (s))
#if BITMAP_BIT_ORDER == LSBFirst
#define BITSSTORE(b,x)  ((unsigned long) (b) & 1 ? \
             (FbDoTypeStore (b, CARD8, x, 0), \
              FbDoTypeStore ((b) + 1, CARD16, x, 8)) : \
             (FbDoTypeStore (b, CARD16, x, 0), \
              FbDoTypeStore ((b) + 2, CARD8, x, 16)))



#else
#define BITSSTORE(b,x)  ((unsigned long) (b) & 1 ? \
             (FbDoTypeStore (b, CARD8, x, 16), \
              FbDoTypeStore ((b) + 1, CARD16, x, 0)) : \
             (FbDoTypeStore (b, CARD16, x, 8), \
              FbDoTypeStore ((b) + 2, CARD8, x, 0)))
#endif


/* BITSSTORE halfword | byte */
#define BITSSTORE_HWB(b,x)                  \
    (FbDoTypeStore (b, CARD16, x, 0),       \
     FbDoTypeStore ((b) + 2, CARD8, x, 16))

/* BITSSTORE  byte | halfword */
#define BITSSTORE_BHW(b,x)                  \
    (FbDoTypeStore (b, CARD8, x, 0),        \
     FbDoTypeStore ((b) + 1, CARD16, x, 8))

/* BITSGET is used to retrieve bits from a unaligned 32-bit address */
#define BITSGET(base, offset) ((CARD32) \
    (((((CARD8 *)(base))[offset+2]) << 16) | \
     ((((CARD8 *)(base))[offset+1]) << 8)  | \
       ((CARD8 *)(base))[offset]))


/* BITSGET byte | halfword */
#define BITSGET_BHW(b) \
    (*((CARD8 *)(b))) | *((CARD16 *)((b) + 1)) << 8

/* BITSGET halfword | byte */
#define BITSGET_HWB(b) \
    (*((CARD16 *)(b))) | *((CARD8 *)((b) + 2)) << 16


#define BITSGET_L24(s)   (CARD32)((s) & (CARD32)0xffffff)
#define BITSGET_H24(s)   (CARD32)(((s) >> 8) & (CARD32)0xffffff)


/*******************************************************************************
*
* FbDoAlphaBlendTriplet(a,fg,bg,d) - blends an even aligned 24-bit RGB triplet.
*
* This function blends 96 bits or 4 24-bit RGB pixels.
*
*    +-----+---+ +----+----+ +---+-----+
*    | RGB | R | | GB | RG | | B | RGB |  --->
*    +-----+---+ +----+----+ +---+-----+
*     (32 bits)   (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendTriplet(a,fg,bg,d)                                       \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_L24(*((CARD32 *)(fg))), BITSGET_L24(*((CARD32 *)(bg))));       \
    BITSSTORE_HWB(((CARD8 *)(d)), __bits);                                     \
    __bits = FBBLEND(*((a)+1),                                                 \
        BITSGET_BHW(((CARD8 *)(fg)) + 3), BITSGET_BHW(((CARD8 *)(bg)) + 3));   \
    BITSSTORE_BHW((((CARD8 *)(d)) + 3), __bits);                               \
    __bits = FBBLEND(*((a)+2),                                                 \
        BITSGET_HWB(((CARD8 *)(fg)) + 6), BITSGET_HWB(((CARD8 *)(bg)) + 6));   \
    BITSSTORE_HWB((((CARD8 *)(d)) + 6), __bits);                               \
    __bits = FBBLEND(*((a)+3),                                                 \
        BITSGET_H24(*(((CARD32 *)(fg)) + 2)),                                  \
        BITSGET_H24(*(((CARD32 *)(bg)) + 2)));                                 \
    BITSSTORE_BHW((((CARD8 *)(d)) + 9), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendTripletR(a,fg,bg,d) - blends an even aligned 24-bit RGB triplet.
*
* This function blends 96 bits or 4 24-bit RGB pixels in reverse order.
*
*    +-----+---+ +----+----+ +---+-----+
*    | RGB | R | | GB | RG | | B | RGB |  <---
*    +-----+---+ +----+----+ +---+-----+
*     (32 bits)   (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendTripletR(a,fg,bg,d)                                      \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*((a)-1),                                                 \
        (*((CARD32 *)(((CARD8 *)(fg)) - 4)) >> 8) & ((CARD32)0xffffff),        \
        (*((CARD32 *)(((CARD8 *)(bg)) - 4)) >> 8) & ((CARD32)0xffffff));       \
    BITSSTORE((((CARD8 *)(d)) + 9), __bits);                                   \
    __bits = FBBLEND(*((a)-2), BITSGET(fg, -6), BITSGET(bg, -6));              \
    BITSSTORE((((CARD8 *)(d)) + 6), __bits);                                   \
    __bits = FBBLEND(*((a)-3), BITSGET(fg, -9), BITSGET(bg, -9));              \
    BITSSTORE((((CARD8 *)(d)) + 3), __bits);                                   \
    __bits = FBBLEND(*((a)-4),                                                 \
        *((CARD32 *)(((CARD8 *)(fg)) - 12)) & ((CARD32)0xffffff),              \
        *((CARD32 *)(((CARD8 *)(bg)) - 12)) & ((CARD32)0xffffff));             \
    BITSSTORE(((CARD8 *)(d)), __bits);                                         \
}

/*******************************************************************************
*
* FbDoAlphaBlendSinglet(a,fg,bg,d) - blends an even aligned 24 bit RGB singlet.
*
* This function blends 32 bits or 1 1/3 24-bit RGB pixels.  NOTE: to blend the
* 1/3 pixel the next 16 bits of the second 32-bit segment are required to
* properly compute the blended value.
*
*    +-----+---+ +----+
*    | RGB | R | | GB |  --->
*    +-----+---+ +----+
*     (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendSinglet(a,fg,bg,d)                                       \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_L24(*((CARD32 *)(fg))), BITSGET_L24(*((CARD32 *)(bg))));       \
    BITSSTORE_HWB(((CARD8 *)(d)), __bits);                                     \
    __bits = FBBLEND(*((a)+1),                                                 \
        BITSGET_BHW(((CARD8 *)(fg)) + 3), BITSGET_BHW(((CARD8 *)(bg)) + 3));   \
    BITSSTORE_BHW((((CARD8 *)(d)) + 3), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendSingletR(a,fg,bg,d) - blends an even aligned 24 bit RGB singlet.
*
* This function blends 32 bits or 1 1/3 24-bit RGB pixels in reverse order.
* NOTE: to blend the 1/3 pixel the previous 16 bits of the second 32-bit segment
* are required to properly compute the blended value.
*
*                     +----+ +---+-----+
*                     | RG | | B | RGB |  <---
*                     +----+ +---+-----+ 
*                             (32 bits)  
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendSingletR(a,fg,bg,d)                                      \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*((a)-1),                                                 \
        (*((CARD32 *)(((CARD8 *)(fg)) - 4)) >> 8) & ((CARD32)0xffffff),        \
        (*((CARD32 *)(((CARD8 *)(bg)) - 4)) >> 8) & ((CARD32)0xffffff));       \
    BITSSTORE((((CARD8 *)(d)) + 9), __bits);                                   \
    __bits = FBBLEND(*((a)-2), BITSGET(fg, -6), BITSGET(bg, -6));              \
    BITSSTORE((((CARD8 *)(d)) + 6), __bits);                                   \
}

/*******************************************************************************
*
* FbDoAlphaBlendTuplet(a,fg,bg,d) - blends an even aligned 24-bit RGB tuplet.
*
* This function blends 64 bits or 2 2/3 24-bit RGB pixels.  NOTE: to blend the
* 2/3 pixel the next 8 bits of the third 32-bit segment are required to properly
* compute the blended value.
*
*    +-----+---+ +----+----+ +---+
*    | RGB | R | | GB | RG | | B |  --->
*    +-----+---+ +----+----+ +---+
*     (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendTuplet(a,fg,bg,d)                                        \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_L24(*((CARD32 *)(fg))), BITSGET_L24(*((CARD32 *)(bg))));       \
    BITSSTORE_HWB(((CARD8 *)(d)), __bits);                                     \
    __bits = FBBLEND(*((a)+1),                                                 \
        BITSGET_BHW(((CARD8 *)(fg)) + 3), BITSGET_BHW(((CARD8 *)(bg)) + 3));   \
    BITSSTORE_BHW((((CARD8 *)(d)) + 3), __bits);                               \
    __bits = FBBLEND(*((a)+2),                                                 \
        BITSGET_HWB(((CARD8 *)(fg)) + 6), BITSGET_HWB(((CARD8 *)(bg)) + 6));   \
    BITSSTORE_HWB((((CARD8 *)(d)) + 6), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendTupletR(a,fg,bg,d) - blends an even aligned 24-bit RGB tuplet.
*
* This function blends 64 bits or 2 2/3 24-bit RGB pixels.  NOTE: to blend the
* 2/3 pixel the previous 8 bits of the first 32-bit segment are required to
* properly compute the blended value.
*
*          +---+ +----+----+ +---+-----+
*          | R | | GB | RG | | B | RGB |  <---
*          +---+ +----+----+ +---+-----+
*                             (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendTupletR(a,fg,bg,d)                                       \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*((a)-1),                                                 \
        (*((CARD32 *)(((CARD8 *)(fg)) - 4)) >> 8) & ((CARD32)0xffffff),        \
        (*((CARD32 *)(((CARD8 *)(bg)) - 4)) >> 8) & ((CARD32)0xffffff));       \
    BITSSTORE((((CARD8 *)(d)) + 9), __bits);                                   \
    __bits = FBBLEND(*((a)-2), BITSGET(fg, -6), BITSGET(bg, -6));              \
    BITSSTORE((((CARD8 *)(d)) + 6), __bits);                                   \
    __bits = FBBLEND(*((a)-3), BITSGET(fg, -9), BITSGET(bg, -9));              \
    BITSSTORE((((CARD8 *)(d)) + 3), __bits);                                   \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0xff000000(a,fg,bg,d,n) - 8-bit LSB masked blender.
*
* This function blends a even aligned 24-bit RGB triplet with the 8 least
* significant bits (second 24-bit RGB pixel) masked on the first 32 bits.  NOTE:
* the second and third 32-bit segments will only be blended if required.
*
*       0xff000000
*            |
*    +-----+---+ +----+----+ +---+-----+
*    | xxx | R | | GB | RG | | B | RGB |  --->
*    +-----+---+ +----+----+ +---+-----+
*     (32 bits)   (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
* n  = number of 32-bit segments following this 32-bit segment.
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0xff000000(a,fg,bg,d,n)                            \
{                                                                              \
    FbBits __bits;                                                             \
    int __num = (n);                                                           \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_BHW(((CARD8 *)(fg)) + 3), BITSGET_BHW(((CARD8 *)(bg)) + 3));   \
    BITSSTORE_BHW((((CARD8 *)(d)) + 3), __bits);                               \
    if((__num) > 0)                                                            \
    {                                                                          \
        (__num)--;                                                             \
        __bits = FBBLEND(*((a)+1), BITSGET_HWB(((CARD8 *)(fg)) + 6),           \
            BITSGET_HWB(((CARD8 *)(bg)) + 6));                                 \
        BITSSTORE_HWB((((CARD8 *)(d)) + 6), __bits);                           \
    }                                                                          \
    if((__num) > 0)                                                            \
    {                                                                          \
        __bits = FBBLEND(*((a)+2),                                             \
            BITSGET_H24(*(((CARD32 *)fg) + 2)),                                \
            BITSGET_H24(*(((CARD32 *)bg) + 2)));                               \
        BITSSTORE_BHW((((CARD8 *)(d)) + 9), __bits);                           \
    }                                                                          \
}
        
/*******************************************************************************
*
* FbDoAlphaBlendMasked0xffff0000(a,fg,bg,d,n) - 16-bit LSB masked blender.
*
* This function blends a 24-bit RGB tuplet with the 16 least significant bits
* (third 24-bit RGB pixel in an even aligned triplet) masked on the second 32
* bit segment.  NOTE: the third 32-bit segment will only be blended if required.
*
*                  0xffff0000
*                       |
*                +----+----+ +---+-----+
*                | xx | RG | | B | RGB |  --->
*                +----+----+ +---+-----+
*                 (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
* n  = number of 32-bit segments following this 32-bit segment
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0xffff0000(a,fg,bg,d,n)                            \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_HWB(((CARD8 *)(fg)) + 2), BITSGET_HWB(((CARD8 *)(bg)) + 2));   \
    BITSSTORE_HWB((((CARD8 *)(d)) + 6), __bits);                               \
    if((n) > 0)                                                                \
    {                                                                          \
        __bits = FBBLEND(*((a)+1),                                             \
            BITSGET_H24(*(((CARD32 *)(fg)) + 1)),                              \
            BITSGET_H24(*(((CARD32 *)(bg)) + 1)));                             \
        BITSSTORE_BHW((((CARD8 *)(d)) + 9), __bits);                           \
    }                                                                          \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0xffffff00(a,fg,bg,d) - 24-bit LSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 24 least significant bits
* (fourth 24-bit RGB pixel in an even aligned triplet) masked on the third 32
* bit segment.
*
*                              0xffffff00
*                                   |
*                            +---+-----+
*                            | x | RGB |  --->
*                            +---+-----+
*                             (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0xffffff00(a,fg,bg,d)                              \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_H24(*((CARD32 *)(fg))), BITSGET_H24(*((CARD32 *)(bg))));       \
    BITSSTORE_BHW((((CARD8 *)(d)) + 9), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0x000000ff(a,fg,bg,d) - 8-bit MSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 8 most significant bits
* (third 24-bit RGB pixel in an even aligned triplet) masked on the third 32
* bit segment.  This blend operation requires the previous 16 bits of the
* previous 32-bit segment.
*
*                         0x000000ff
*                              |
*                     +----+ +---+-----+
*                     | RG | | B | xxx |  --->
*                     +----+ +---+-----+
*                             (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0x000000ff(a,fg,bg,d)                              \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_HWB(((CARD8 *)(fg)) - 2), BITSGET_HWB(((CARD8 *)(bg)) - 2));   \
    BITSSTORE_HWB((((CARD8 *)(d)) + 6), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0x000000ffR(a,fg,bg,d,n) - 8-bit MSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 8 most significant bits
* (third 24-bit RGB pixel in an even aligned triplet) masked on the third 32
* bit segment.  This blend operation requires the previous 16 bits of the
* previous 32-bit segment. FbDoAlphaBlendMasked0x000000ffR is intended for
* blending the scanline in the reverse order.  Therefore, if required, the
* previous 1 or 2 32-bit segments will also be blended to be on an even aligned
* triplet. 
*
* (n == 0)
*                             0x000000ff
*                              |
*                     +----+ +---+-----+
*                     | RG | | B | xxx |  <---
*                     +----+ +---+-----+
*                             (32 bits)
*
* (n == 1)
*                             0x000000ff
*                              |
*          +---+ +----+----+ +---+-----+
*          | R | | GB | RG | | B | xxx |  <---
*          +---+ +----+----+ +---+-----+
*                 (32 bits)   (32 bits)
*
* (n == 2)
*                             0x000000ff
*                              |
*    +-----+---+ +----+----+ +---+-----+
*    | RGB | R | | GB | RG | | B | xxx |  <---
*    +-----+---+ +----+----+ +---+-----+
*     (32 bits)   (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
* n  = number of 32-bit segments before this 32-bit segment
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0x000000ffR(a,fg,bg,d,n)                           \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a), BITSGET(fg, -2), BITSGET(bg, -2));                  \
    BITSSTORE((((CARD8 *)(d)) + 6), __bits);                                   \
    if((n) > 0)                                                                \
    {                                                                          \
        __bits = FBBLEND(*((a)-1), BITSGET(fg, -5), BITSGET(bg, -5));          \
        BITSSTORE((((CARD8 *)(d)) + 3), __bits);                               \
        __bits = FBBLEND(*((a)-2),                                             \
            *((CARD32 *)(((CARD8 *)(fg)) - 8)) & (CARD32)0xffffff,             \
            *((CARD32 *)(((CARD8 *)(bg)) - 8)) & (CARD32)0xffffff);            \
        BITSSTORE(((CARD8 *)(d)), __bits);                                     \
    }                                                                          \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0x0000ffff(a,fg,bg,d) - 16-bit MSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 16 most significant bits
* (second 24-bit RGB pixel in an even aligned triplet) masked on the second 32
* bit segment.  This blend operation requires the previous 8 bits of the
* previous 32-bit segment.
*
*              0x0000ffff
*                   |
*          +---+ +----+----+
*          | R | | GB | xx |  --->
*          +---+ +----+----+
*                 (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0x0000ffff(a,fg,bg,d)                              \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_BHW(((CARD8 *)(fg)) - 1), BITSGET_BHW(((CARD8 *)(bg)) - 1));   \
    BITSSTORE_BHW((((CARD8 *)(d)) + 3), __bits);                               \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0x0000ffffR(a,fg,bg,d,n) - 16-bit MSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 16 most significant bits
* (second 24-bit RGB pixel in an even aligned triplet) masked on the second 32
* bit segment.  This blend operation requires the previous 8 bits of the
* previous 32-bit segment. FbDoAlphaBlendMasked0x0000ffffR is intended for
* blending the scanline in the reverse order.  Therefore, if required, the
* previous 32-bit segment will also be blended to be on an even aligned
* triplet.
*
* (n == 0)
*                 0x0000ffff
*                  |
*          +---+ +----+----+
*          | R | | GB | xx |  <---
*          +---+ +----+----+
*                 (32 bits)
*
* (n == 1)
*                 0x0000ffff
*                  |
*    +-----+---+ +----+----+
*    | RGB | R | | GB | xx |  <---
*    +-----+---+ +----+----+
*     (32 bits)   (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
* n  = number of 32-bit segments before this 32-bit segment
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0x0000ffffR(a,fg,bg,d,n)                           \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a), BITSGET(fg, -1), BITSGET(bg, -1));                  \
    BITSSTORE((((CARD8 *)(d)) + 3), __bits);                                   \
    if((n) > 0)                                                                \
    {                                                                          \
        __bits = FBBLEND(*((a)-1),                                             \
            *((CARD32 *)(((CARD8 *)(fg)) - 4)) & (CARD32)0xffffff,             \
            *((CARD32 *)(((CARD8 *)(bg)) - 4)) & (CARD32)0xffffff);            \
        BITSSTORE(((CARD8 *)(d)), __bits);                                     \
    }                                                                          \
}

/*******************************************************************************
*
* FbDoAlphaBlendMasked0x00ffffff(a,fg,bg,d) - 24-bit MSB masked blender.
*
* This function blends a 24-bit RGB singlet with the 24 most significant bits
* (first 24-bit RGB pixel in an even aligned triplet) masked on the first 32
* bit segment.
*
*  0xff000000
*       |
*    +-----+---+
*    | RGB | x |  --->
*    +-----+---+
*     (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 24-bit RGB pixel data
* bg = address to destination (background) 24-bit RGB pixel data
* d  = address to a 3-32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlendMasked0x00ffffff(a,fg,bg,d)                              \
{                                                                              \
    FbBits __bits;                                                             \
    __bits = FBBLEND(*(a),                                                     \
        BITSGET_L24(*((CARD32 *)(fg))), BITSGET_L24(*((CARD32 *)(bg))));       \
    BITSSTORE_HWB(((CARD8 *)(d)), __bits);                                     \
}

static void
fbAlphaBlt24Line (FbBits        *src,
                  int           srcX,

                  FbBits        *dst,
                  int           dstX,

                  unsigned char *alphas,
                  int           alphasX,

                  int           width,

                  int           alu,
                  FbBits        pm,
                  int           bpp,
     
                  Bool          reverse)
{
    int     leftShift, rightShift;
    FbBits  startmask, endmask;
    int     n;
    
    FbBits  bits, bits1, bits2[3];
    FbBits  mask;

    int     rot;
    FbDeclareMergeRop ();

    FbInitializeMergeRop (alu, FB_ALLONES);

    FbMaskBits(dstX, width, startmask, n, endmask);

    if (reverse)
    {
    src += ((srcX + width - 1) >> FB_SHIFT) + 1;
    dst += ((dstX + width - 1) >> FB_SHIFT) + 1;
    alphas += alphasX + (width / bpp);
    rot = FbFirst24Rot (((dstX + width - 8) & FB_MASK));
    rot = FbPrev24Rot(rot);
    srcX = (srcX + width - 1) & FB_MASK;
    dstX = (dstX + width - 1) & FB_MASK;
    }
    else
    {
    src += srcX >> FB_SHIFT;
    dst += dstX >> FB_SHIFT;
    alphas += alphasX;
    srcX &= FB_MASK;
    dstX &= FB_MASK;
    rot = FbFirst24Rot (dstX);
    }
    mask = FbRot24(pm,rot);

    if (srcX == dstX)
    {
    if (reverse)
    {
        if (endmask)
        {
        --src;
        --dst;
        --alphas;

        if(endmask == (CARD32)0x000000ff)
        {
            FbDoAlphaBlendMasked0x000000ffR(alphas,src,dst,bits2,n);

            *dst = FbDoMaskMergeRop(bits2[2], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);

            if(n == 0)
            {
                --dst;
#ifndef NO_ASSERT
                assert(startmask == (CARD32)0xffff0000);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop(bits2[1], *dst, mask & startmask);
                startmask = (CARD32)0x00000000;
            }
            else
            {
                --src;
                --dst;
                --alphas;
                --n;
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
                mask = FbPrev24Pix(mask);

                if(n == 0)
                {
                    --dst;
#ifndef NO_ASSERT
                    assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
                    *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
                    startmask = (CARD32)0x00000000;
                }
                else
                {
                    --src;
                    --dst;
                    --alphas;
                    --n;
                    *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
                    mask = FbPrev24Pix(mask);
                }
            }
        }
        else if(endmask == (CARD32)0x0000ffff)
        {
            FbDoAlphaBlendMasked0x0000ffffR(alphas,src,dst,bits2,n);

            *dst = FbDoMaskMergeRop(bits2[1], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);

            if(n == 0)
            {
                --dst;
#ifndef NO_ASSERT
                assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & startmask);
                startmask = (CARD32)0x00000000;
            }
            else
            {
                --src;
                --dst;
                --alphas;
                --n;
                *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
                mask = FbPrev24Pix(mask);
            }
        }
        else if(endmask == (CARD32)0x00ffffff)
        {
            FbDoAlphaBlendMasked0x00ffffff(alphas,src,dst,bits2);
            *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);
        }
        }

        while (n >= 3)
        {
            FbDoAlphaBlendTripletR(alphas, src, dst, bits2);

            --dst;
            --src;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            --src;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            --src;
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            mask = FbPrev24Pix(mask);

            n-=3;
            alphas-=4;
        }

        if(n == 1)
        {
            FbDoAlphaBlendSingletR(alphas,src,dst,bits2);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xffff0000);
#endif /* NO_ASSERT */
            --dst;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & startmask);
            startmask = (CARD32)0x00000000;
        }
        else if(n == 2)
        {
            FbDoAlphaBlendTupletR(alphas,src,dst,bits2);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            mask = FbPrev24Pix(mask);

#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
            --dst;
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
            startmask = (CARD32)0x00000000;
        }

        if (startmask)
        {
#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xffffff00);
#endif /* NO_ASSERT */
            --src;
            --dst;
            --alphas;

            FbDoAlphaBlendMasked0xffffff00(alphas,src,dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & startmask);
        }
    }
    else
    {
        if (startmask)
        {
        if(startmask == (CARD32)0xff000000)
        {
            FbDoAlphaBlendMasked0xff000000(alphas,src,dst,bits2,n);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
            mask = FbNext24Pix (mask);
            dst++;
            src++;

            if(n == 0)
            {
#ifndef NO_ASSERT
                assert(endmask == (CARD32)0x0000ffff);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & endmask);
                endmask = (CARD32)0x00000000;
            }
            else
            {
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
                dst++;
                src++;
                n--;
                alphas++;
                mask = FbNext24Pix(mask);

                if(n == 0)
                {
#ifndef NO_ASSERT
                    assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
                    *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
                    endmask = (CARD32)0x00000000;
                }
                else
                {
                    *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
                    dst++;
                    src++;
                    n--;
                    alphas+=2;
                    mask = FbNext24Pix(mask);
                }
            }
        }
        else if(startmask == (CARD32)0xffff0000)
        {
            FbDoAlphaBlendMasked0xffff0000(alphas,src,dst,bits2,n);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & startmask);
            dst++;
            src++;
            mask = FbNext24Pix (mask);

            if(n == 0)
            {
#ifndef NO_ASSERT
                assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
                endmask = (CARD32)0x00000000;
            }
            else
            {
                *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
                dst++;
                src++;
                n--;
                alphas+=2;
                mask = FbNext24Pix(mask);
            }
        }
        else if(startmask == (CARD32)0xffffff00)
        {
            FbDoAlphaBlendMasked0xffffff00(alphas,src,dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & startmask);
            dst++;
            src++;
            alphas++;
            mask = FbNext24Pix(mask);
        }
        }

        while (n >= 3)
        {
            FbDoAlphaBlendTriplet(alphas, src, dst, bits2);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            n-=3;
            src+=3;
            alphas+=4;
        }

        if(n == 1)
        {
            FbDoAlphaBlendSinglet(alphas,src,dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x0000ffff);
#endif /* NO_ASSERT */
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & endmask);
            endmask = (CARD32)0x00000000;
        }
        else if(n == 2)
        {
            FbDoAlphaBlendTuplet(alphas,src,dst,bits2);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
            endmask = (CARD32)0x00000000;
        }

        if (endmask)
        {
#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x00ffffff); 
#endif /* NO_ASSERT */
            FbDoAlphaBlendMasked0x00ffffff(alphas,src,dst,bits2);
            *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & endmask);
        }
    }
    }
    else
    {
    FbBits bits3[3];

    if (srcX > dstX)
    {
        leftShift = srcX - dstX;
        rightShift = FB_UNIT - leftShift;
    }
    else
    {
        rightShift = dstX - srcX;
        leftShift = FB_UNIT - rightShift;
    }
    
    bits1 = 0;
    if (reverse)
    {
        if (srcX < dstX)
        {
            bits1 = *--src;
        }

        if (endmask)
        {
        --dst;
        --alphas;

        if(endmask == (CARD32)0x000000ff)
        {
            bits3[2] = FbScrRight(bits1, rightShift); 
            if (FbScrRight(endmask, leftShift))
            {
                bits1 = *--src;
                bits3[2] |= FbScrLeft(bits1, leftShift);
            }

            bits3[1] = FbScrRight(bits1, rightShift); 
            if (FbScrRight(endmask, leftShift))
            {
                bits1 = *(src-1);
                bits3[1] |= FbScrLeft(bits1, leftShift);
            }

            if(n > 0)
            {
                bits3[0] = FbScrRight(bits1, rightShift); 
                if (FbScrRight(endmask, leftShift))
                {
                    bits1 = *(src-2);
                    bits3[0] |= FbScrLeft(bits1, leftShift);
                }
            }

            FbDoAlphaBlendMasked0x000000ffR(alphas,&bits3[2],dst,bits2,n);

            *dst = FbDoMaskMergeRop(bits2[2], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);

            if(n == 0)
            {
                --dst;
#ifndef NO_ASSERT
                assert(startmask == (CARD32)0xffff0000);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop(bits2[1], *dst, mask & startmask);
                startmask = (CARD32)0x00000000;
            }
            else
            {
                --src;
                --dst;
                --alphas;
                --n;
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
                mask = FbPrev24Pix(mask);

                if(n == 0)
                {
                    --dst;
#ifndef NO_ASSERT
                    assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
                    *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
                    startmask = (CARD32)0x00000000;
                }
                else
                {
                    --src;
                    --dst;
                    --alphas;
                    --n;
                    *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
                    mask = FbPrev24Pix(mask);
                }
            }
        }
        else if(endmask == (CARD32)0x0000ffff)
        {
            bits3[1] = FbScrRight(bits1, rightShift); 
            if (FbScrRight(endmask, leftShift))
            {
                bits1 = *--src;
                bits3[1] |= FbScrLeft(bits1, leftShift);
            }

            bits3[0] = FbScrRight(bits1, rightShift); 
            if (FbScrRight(endmask, leftShift))
            {
                bits1 = *(src-1);
                bits3[0] |= FbScrLeft(bits1, leftShift);
            }

            FbDoAlphaBlendMasked0x0000ffffR(alphas,&bits3[1],dst,bits2,n);

            *dst = FbDoMaskMergeRop(bits2[1], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);

            if(n == 0)
            {
                --dst;
#ifndef NO_ASSERT
                assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & startmask);
                startmask = (CARD32)0x00000000;
            }
            else
            {
                --src;
                --dst;
                --alphas;
                --n;
                *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
                mask = FbPrev24Pix(mask);
            }
        }
        else if(endmask == (CARD32)0x00ffffff)
        {
            bits3[0] = FbScrRight(bits1, rightShift); 
            if (FbScrRight(endmask, leftShift))
            {
                bits1 = *--src;
                bits3[0] |= FbScrLeft(bits1, leftShift);
            }

            FbDoAlphaBlendMasked0x00ffffff(alphas,bits3,dst,bits2);
            *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & endmask);
            mask = FbPrev24Pix (mask);
        }
        }

        while (n >= 3)
        {
            bits3[2] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[2] |= FbScrLeft(bits1, leftShift);

            bits3[1] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[1] |= FbScrLeft(bits1, leftShift);
        
            bits3[0] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[0] |= FbScrLeft(bits1, leftShift);

            FbDoAlphaBlendTripletR(alphas, (&bits3[2]+1), dst, bits2);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            mask = FbPrev24Pix(mask);

            n-=3;
            alphas-=4;
        }

        if(n == 1)
        {
            bits3[2] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[2] |= FbScrLeft(bits1, leftShift);

            bits3[1] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[1] |= FbScrLeft(bits1, leftShift);

            FbDoAlphaBlendSingletR(alphas,(&bits3[2]+1),dst,bits2);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xffff0000);
#endif /* NO_ASSERT */
            --dst;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & startmask);
            startmask = (CARD32)0x00000000;
        }
        else if(n == 2)
        {
            bits3[2] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[2] |= FbScrLeft(bits1, leftShift);

            bits3[1] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[1] |= FbScrLeft(bits1, leftShift);
        
            bits3[0] = FbScrRight(bits1, rightShift); 
            bits1 = *--src;
            bits3[0] |= FbScrLeft(bits1, leftShift);

            FbDoAlphaBlendTupletR(alphas,(&bits3[2]+1),dst,bits2);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            mask = FbPrev24Pix(mask);

            --dst;
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            mask = FbPrev24Pix(mask);

#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xff000000);
#endif /* NO_ASSERT */
            --dst;
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
            startmask = (CARD32)0x00000000;
        }

        if (startmask)
        {
#ifndef NO_ASSERT
            assert(startmask == (CARD32)0xffffff00);
#endif /* NO_ASSERT */
            --alphas;
            bits = FbScrRight(bits1, rightShift); 
            if (FbScrRight(startmask, leftShift))
            {
                bits1 = *--src;
                bits |= FbScrLeft(bits1, leftShift);
            }

            --dst;
            FbDoAlphaBlendMasked0xffffff00(alphas,&bits,dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & startmask);
        }
    }
    else
    {
        if (srcX > dstX)
        {
            bits1 = *src++;
        }

        if (startmask)
        {
        if(startmask == (CARD32)0xff000000)
        {
            bits3[0] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[0] |= FbScrRight(bits1, rightShift);

            bits3[1] = FbScrLeft(bits1, leftShift); 
    
            if(n == 0)
            {
                if (FbScrLeft(endmask, rightShift))
                {
                    bits1 = *src;
                    bits3[1] |= FbScrRight(bits1, rightShift);
                }
            }
            else if(n == 1)
            {
                bits1 = *src;
                bits3[1] |= FbScrRight(bits1, rightShift);

                bits3[2] = FbScrLeft(bits1, leftShift); 

                if (FbScrLeft(endmask, rightShift))
                {
                    bits1 = *(src+1);
                    bits3[2] |= FbScrRight(bits1, rightShift);
                }
            }
            else
            {
                bits1 = *src;
                bits3[1] |= FbScrRight(bits1, rightShift);

                bits3[2] = FbScrLeft(bits1, leftShift); 
                bits1 = *(src+1);
                bits3[2] |= FbScrRight(bits1, rightShift);
            }

            FbDoAlphaBlendMasked0xff000000(alphas,bits3,dst,bits2,n);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask & startmask);
            dst++;
            mask = FbNext24Pix(mask);

            if(n == 0)
            {
#ifndef NO_ASSERT
                assert(endmask == (CARD32)0x0000ffff);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & endmask);
                endmask = (CARD32)0x00000000;
            }
            else
            {
                *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
                dst++;
                src++;
                n--;
                alphas++;
                mask = FbNext24Pix(mask);

                if(n == 0)
                {
#ifndef NO_ASSERT
                    assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
                    *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
                    endmask = (CARD32)0x00000000;
                }
                else
                {
                    *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
                    dst++;
                    src++;
                    n--;
                    alphas+=2;
                    mask = FbNext24Pix(mask);
                }
            }
        }
        else if(startmask == (CARD32)0xffff0000)
        {
            bits3[1] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[1] |= FbScrRight(bits1, rightShift);

            bits3[2] = FbScrLeft(bits1, leftShift); 

            if(n == 0)
            {
                if (FbScrLeft(endmask, rightShift))
                {
                    bits1 = *src;
                    bits3[2] |= FbScrRight(bits1, rightShift);
                }
            }
            else
            {
                bits1 = *src;
                bits3[2] |= FbScrRight(bits1, rightShift);
            }
  
            FbDoAlphaBlendMasked0xffff0000(alphas,&bits3[1],dst,bits2,n);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & startmask);
            dst++;
            mask = FbNext24Pix(mask);

            if(n == 0)
            {
#ifndef NO_ASSERT
                assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
                *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
                endmask = (CARD32)0x00000000;
            }
            else
            {
                *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
                dst++;
                src++;
                n--;
                alphas+=2;
                mask = FbNext24Pix(mask);
            }
        }
        else if(startmask == (CARD32)0xffffff00)
        {
            bits3[2] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[2] |= FbScrRight(bits1, rightShift);

            FbDoAlphaBlendMasked0xffffff00(alphas,&bits3[2],dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & startmask);
            dst++;
            alphas++;
            mask = FbNext24Pix(mask);
        }
        }

        while (n >= 3)
        {
            bits3[0] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[0] |= FbScrRight(bits1, rightShift);

            bits3[1] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[1] |= FbScrRight(bits1, rightShift);
        
            bits3[2] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[2] |= FbScrRight(bits1, rightShift);

            FbDoAlphaBlendTriplet(alphas, bits3, dst, bits2);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            alphas+=4;
            n-=3;
        }

        if(n == 1)
        {
            bits3[0] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[0] |= FbScrRight(bits1, rightShift);

            bits3[1] = FbScrLeft(bits1, leftShift); 
            if (FbScrLeft(endmask, rightShift))
            {
                bits1 = *src;
                bits3[1] |= FbScrRight(bits1, rightShift);
            }

            FbDoAlphaBlendSinglet(alphas,bits3,dst,bits2);
            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x0000ffff);
#endif /* NO_ASSERT */
            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask & endmask);
            endmask = (CARD32)0x00000000;
        }
        else if(n == 2)
        {
            bits3[0] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[0] |= FbScrRight(bits1, rightShift);

            bits3[1] = FbScrLeft(bits1, leftShift); 
            bits1 = *src++;
            bits3[1] |= FbScrRight(bits1, rightShift);
        
            bits3[2] = FbScrLeft(bits1, leftShift); 

            if (FbScrLeft(endmask, rightShift))
            {
                bits = *src;
                bits3[2] |= FbScrRight(bits, rightShift);
            }

            FbDoAlphaBlendTuplet(alphas,bits3,dst,bits2);

            *dst = FbDoMaskMergeRop (bits2[0], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

            *dst = FbDoMaskMergeRop (bits2[1], *dst, mask);
            dst++;
            mask = FbNext24Pix(mask);

#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x000000ff);
#endif /* NO_ASSERT */
            *dst = FbDoMaskMergeRop (bits2[2], *dst, mask & endmask);
            endmask = (CARD32)0x00000000;
        }

        if (endmask)
        {
#ifndef NO_ASSERT
            assert(endmask == (CARD32)0x00ffffff);
#endif /* NO_ASSERT */

            bits3[0] = FbScrLeft(bits1, leftShift); 
            if (FbScrLeft(endmask, rightShift))
            {
                bits1 = *src;
                bits3[0] |= FbScrRight(bits1, rightShift);
            }

            FbDoAlphaBlendMasked0x00ffffff(alphas,bits3,dst,bits2);
            *dst = FbDoMaskMergeRop(bits2[0], *dst, mask & endmask);
        }
    }
    }
}
#endif /* FB_SHIFT == 5 */

void
fbAlphaBlt24 (FbBits   *srcLine,
       FbStride srcStride,
       int  srcX,
       
       FbBits   *dstLine,
       FbStride dstStride,
       int  dstX,
       
       unsigned char *alphasLine,
       FbStride alphasStride,
       int alphasX,

       int  width,
       int  height,
       
       int  alu,
       FbBits   pm,
       int  bpp,
       
       Bool reverse,
       Bool upsidedown)
{
    if (upsidedown)
    {
    srcLine += (INT32)(height - 1) * (INT32)srcStride;
    dstLine += (INT32)(height - 1) * (INT32)dstStride;
    alphasLine += (INT32)(height - 1) * (INT32)alphasStride;
    srcStride = -srcStride;
    dstStride = -dstStride;
    alphasStride = -alphasStride;
    }

    while (height--)
    {
    fbAlphaBlt24Line (srcLine, srcX, dstLine, dstX, alphasLine, alphasX, width,
        alu, pm, bpp, reverse);
    srcLine += srcStride;
    dstLine += dstStride;
    alphasLine += alphasStride;
    }
}
#endif /* FB_24BIT */

/*******************************************************************************
*
* 32 bits per pixel alpha blending color support
*
*******************************************************************************/

#ifdef FB_32BIT
#ifdef FBALPHABLT_8_16_32
#undef FBALPHABLT_8_16_32
#endif

#ifdef FBTRANSBLT_8_16_32
#undef FBTRANSBLT_8_16_32
#endif

#ifdef FbDoAlphaBlend
#undef FbDoAlphaBlend
#endif

#ifdef FbDoAlphaBlendReverse
#undef FbDoAlphaBlendReverse
#endif

#ifdef FbDoRightMaskAlphaBlend
#undef FbDoRightMaskAlphaBlend
#endif

#ifdef FbDoLeftMaskAlphaBlend
#undef FbDoLeftMaskAlphaBlend
#endif

#ifdef FbDoTransBlend
#undef FbDoTransBlend
#endif

#ifdef FbDoTransBlendReverse
#undef FbDoTransBlendReverse
#endif

#ifdef FbDoRightMaskTransBlend
#undef FbDoRightMaskTransBlend
#endif

#ifdef FbDoLeftMaskTransBlend
#undef FbDoLeftMaskTransBlend
#endif

#define FBALPHABLT_8_16_32 fbAlphaBlt32
#define FBTRANSBLT_8_16_32 fbTransBlt32

#if FB_SHIFT == 5
/*******************************************************************************
*
* FbDoAlphaBlend(a,fg,bg,d) - blends 1 32-bit RGB pixel.
*
*    +-----------+
*    |    RGB    |
*    +-----------+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoAlphaBlend(a,fg,bg,d)                                              \
{                                                                              \
    *((CARD32 *)(d)) = (CARD32)FBBLEND(*(a),                                   \
    (CARD32)(*((CARD32 *)(fg))), (CARD32)(*((CARD32 *)(bg))));                 \
}

#define FbDoAlphaBlendReverse(a,fg,bg,d)     FbDoAlphaBlend(a,fg,bg,d)
#define FbDoRightMaskAlphaBlend(a,fg,bg,d,b) FbDoAlphaBlend(a,fg,bg,d)
#define FbDoLeftMaskAlphaBlend(a,fg,bg,d,b)  FbDoAlphaBlend(a,fg,bg,d)

/*******************************************************************************
*
* FbDoTransBlend(a,fg,bg,d) - blends 1 32-bit RGB pixel.
*
*    +-----------+
*    |    RGB    |
*    +-----------+
*      (32 bits)
*
* a  = address to alpha values array
* fg = address to source (foreground) 16-bit RGB pixel data
* bg = address to destination (background) 16-bit RGB pixel data
* d  = address to a 32 bit array to store blended pixel data
*
*******************************************************************************/
#define FbDoTransBlend(a,fg,bg,d)                                              \
{                                                                              \
    if (*(a)) {                                                                \
        CARD32 ba = ((*((CARD32 *)(bg))) & ALPHA_MASK_32) >> ALPHA_SHIFT_32;   \
        if (ba && (*(a) < 255)) {                                              \
            FbDoAlphaBlend(a, fg, bg, d);                                      \
            if (ba < 255)                                                      \
                ba = (ba + *(a)) - ((ba * (*(a))) / 255);                      \
            *((CARD32 *)(d)) |= ((ba << ALPHA_SHIFT_32) & ALPHA_MASK_32);      \
        } else {                                                               \
            *((CARD32 *)(d)) = *((CARD32 *)(fg));                              \
            *((CARD32 *)(d)) |= (((*(a)) << ALPHA_SHIFT_32) & ALPHA_MASK_32);  \
        }                                                                      \
    } else                                                                     \
        *((CARD32 *)(d)) = *((CARD32 *)(bg));                                  \
}

#define FbDoTransBlendReverse(a,fg,bg,d)     FbDoTransBlend(a,fg,bg,d)
#define FbDoRightMaskTransBlend(a,fg,bg,d,b) FbDoTransBlend(a,fg,bg,d)
#define FbDoLeftMaskTransBlend(a,fg,bg,d,b)  FbDoTransBlend(a,fg,bg,d)
#endif /* FB_SHIFT == 5 */

#include "fbalphablt.h"
#endif /* FB_32BIT */

void
fbAlphaBlt (GCPtr pGC,
       FbBits   *srcLine,
       FbStride srcStride,
       int  srcX,
       
       FbBits   *dstLine,
       FbStride dstStride,
       int  dstX,
       
       unsigned char *alphasLine,
       FbStride alphasStride,
       int alphasX,

       int  width,
       int  height,
       
       int  alu,
       FbBits   pm,
       int  bpp,
       
       Bool reverse,
       Bool upsidedown)
{
    switch(bpp)
    {
#ifdef FB_8BIT
        case 8:
        {
    #ifdef FB_TRANS
            if (pGC->transDraw)
                fbTransBlt8 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            else
    #endif /* FB_TRANS*/
                fbAlphaBlt8 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            break;
        }
#endif

#ifdef FB_16BIT
        case 16:
        {
    #ifdef FB_TRANS
            if (pGC->transDraw)
                fbTransBlt16 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            else
    #endif /* FB_TRANS*/
                fbAlphaBlt16 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            break;
        }
#endif

#ifdef FB_24BIT
        case 24:
        {
            fbAlphaBlt24 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                reverse, upsidedown);
            break;
        }
#endif

#ifdef FB_32BIT
        case 32:
        {
    #ifdef FB_TRANS
            if (pGC->transDraw)
                fbTransBlt32 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            else
    #endif /* FB_TRANS*/
                fbAlphaBlt32 (srcLine, srcStride, srcX, dstLine, dstStride, dstX,
                    alphasLine, alphasStride, alphasX, width, height, alu, pm, bpp,
                    reverse, upsidedown);
            break;
        }
#endif

        default:
        {
            fbBlt (pGC, srcLine, srcStride, srcX, dstLine, dstStride, dstX, width,
                height, alu, pm, bpp, reverse, upsidedown);
            break;
        }

    }
}

#endif /* USE_FB_BLEND */
