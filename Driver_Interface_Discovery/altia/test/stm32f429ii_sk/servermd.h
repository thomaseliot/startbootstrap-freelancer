/* $XFree86: xc/programs/Xserver/include/servermd.h,v 3.51 2001/12/14 19:59:56 dawes Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Xorg: servermd.h,v 1.3 2000/08/17 19:53:31 cpqbld Exp $ */

#ifndef SERVERMD_H
#define SERVERMD_H 1


/* Specify the bit order of the processor the driver is going to
 * be built for.  The choice for IMAGE_BYTE_ORDER and BITMAP_BIT_ORDER
 * are LSBFirst (little indian) or MSBFirst (big indian)
 */
# define IMAGE_BYTE_ORDER   LSBFirst
# define BITMAP_BIT_ORDER   LSBFirst

/*  When getting data from a pixmap the optimal size to fetch data.
 *  This is used in miGetPlane, getting data from a pixmap.
 */
#define BITMAP_SCANLINE_UNIT    32

/*
 * This define defines the base size of the data unit used in pixmaps
 * and on the display.  Its the number to raise to the 2 to the power of.
 * for example a LOG2_BITMAP_PAD value of 4 means the data point size
 * that the frame buffer code deals with is 2^4 or 16 bits.
 */
#define LOG2_BITMAP_PAD     5
// #define LOG2_BITMAP_PAD     4 // :NOTE:  Setting this causes compile errors in fbalphablt.c

/*
 * This define indicates how a scan line is padded in number of bytes.
 * This number is the power to raise 2 to get the number of bytes.
 * For example if scanlines are padded to 32 bits or 4 bytes that
 * would be 2^2 bytes so this value is 2.
 */
#define LOG2_BYTES_PER_SCANLINE_PAD 2

/*
 * This is the same number as above but in a different format.
 * If the above define is represents 32 bits then this define
 * must be 32.
 */
#define BITMAP_SCANLINE_PAD 32

/*
 * These masks are used for transparent buffer draws. They specify
 * the location of the alpha channel in the various buffer formats.
 */
#define ALPHA_MASK_32   0xFF000000
#define ALPHA_MASK_16   0xF000
#define ALPHA_MASK_8    0xFF

/*
 * This shifts are used for transparent buffer draws.  They specify
 * the bit-shift required to position an alpha value in the alpha
 * channel of the various buffer formats.  These should match
 * the masks above in bit-position.
 */
#define ALPHA_SHIFT_32  24
#define ALPHA_SHIFT_16  8

/* Here you indicate what frame buffer sizes you want to support.
 * These sizes are for both the video frame buffer and the pixmaps.
 * You can save code by uncommenting out depths you know that you
 * will not need. So for example, this driver only support 24 bit
 * bitmaps and monochrome bitmaps (see driver.c) so the only define
 * we comment out is FBNO24Bit.  This saves us from including extra
 * code that we will not use.
 */
//#define FBNO8BIT
//#define FBNO16BIT
#define FBNO24BIT
//#define FBNO32BIT
#define FBNOTRANS

/*
 * Define this for if your device uses indexed color and therefore requires
 * indexed alpha color blending.  Not defining this will deem direct color
 * alpha blending.
 */
//#define USE_INDEXED_COLOR_BLENDING

/* 
 *   This returns the number of padding units, for depth d and width w.
 * For bitmaps this can be calculated with the macros above.
 * Other depths require either grovelling over the formats field of the
 * screenInfo or hardwired constants.
 */

typedef struct _PaddingInfo {
    int     padRoundUp; /* pixels per pad unit - 1 */
    int padPixelsLog2;  /* log 2 (pixels per pad unit) */
    int     padBytesLog2;   /* log 2 (bytes per pad unit) */
    int notPower2;  /* bitsPerPixel not a power of 2 */
    int bytesPerPixel;  /* only set when notPower2 is TRUE */
    int bitsPerPixel;   /* bits per pixel */
} PaddingInfo;
extern PaddingInfo PixmapWidthPaddingInfo[];

/* The only portable way to get the bpp from the depth is to look it up */
#define BitsPerPixel(d) (PixmapWidthPaddingInfo[d].bitsPerPixel)

#define PixmapWidthInPadUnits(w, d) \
    (PixmapWidthPaddingInfo[d].notPower2 ? \
    (((int)(w) * PixmapWidthPaddingInfo[d].bytesPerPixel +  \
             PixmapWidthPaddingInfo[d].bytesPerPixel) >> \
    PixmapWidthPaddingInfo[d].padBytesLog2) : \
    ((int)((w) + PixmapWidthPaddingInfo[d].padRoundUp) >> \
    PixmapWidthPaddingInfo[d].padPixelsLog2))

/*
 *  Return the number of bytes to which a scanline of the given
 * depth and width will be padded.
 */
#define PixmapBytePad(w, d) \
    (PixmapWidthInPadUnits(w, d) << PixmapWidthPaddingInfo[d].padBytesLog2)

#define BitmapBytePad(w) \
    (((int)((w) + BITMAP_SCANLINE_PAD - 1) >> LOG2_BITMAP_PAD) << LOG2_BYTES_PER_SCANLINE_PAD)

#define PixmapWidthInPadUnitsProto(w, d) PixmapWidthInPadUnits(w, d)
#define PixmapBytePadProto(w, d) PixmapBytePad(w, d)
#define BitmapBytePadProto(w) BitmapBytePad(w)

#endif /* SERVERMD_H */
