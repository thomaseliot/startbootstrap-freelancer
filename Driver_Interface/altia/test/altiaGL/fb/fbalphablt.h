/* $Revision: 1.2 $    $Date: 2008-05-23 20:34:03 $
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

static void
FBALPHABLT_8_16_32 (FbBits        *srcLine,
                    FbStride       srcStride,
                    int            srcX,

                    FbBits        *dstLine,
                    FbStride       dstStride,
                    int            dstX,

                    unsigned char *alphasLine,
                    FbStride       alphasStride,
                    int            alphasX,

                    int            width,
                    int            height,

                    int            alu,
                    FbBits         pm,
                    int            bpp,

                    Bool           reverse,
                    Bool           upsidedown)
{
    FbBits        *src, *dst;
    int           leftShift, rightShift;
    FbBits        startmask, endmask;
    FbBits        bits, bits1, bits2;
    int           n, nmiddle;
    Bool          destInvarient;
    int           startbyte, endbyte;
    int           directWrite = 0;
    unsigned char *alpha;
    FbDeclareMergeRop ();

    FbInitializeMergeRop(alu, pm);

    if (_ca2 + 1 == 0 && _cx2 == 0)
        directWrite = 1;

    destInvarient = FbDestInvarientMergeRop();

    if (upsidedown)
    {
        srcLine += (INT32)(height - 1) * (INT32)(srcStride);
        dstLine += (INT32)(height - 1) * (INT32)(dstStride);
        alphasLine += (INT32)(height - 1) * (INT32)(alphasStride);
        srcStride = -srcStride;
        dstStride = -dstStride;
        alphasStride = -alphasStride;
    }

    FbMaskBitsBytes (dstX, width, destInvarient, 
                     startmask, startbyte,
                     nmiddle, 
                     endmask, endbyte);

    if (reverse)
    {
        srcLine += ((srcX + width - 1) >> FB_SHIFT) + 1;
        dstLine += ((dstX + width - 1) >> FB_SHIFT) + 1;
        alphasLine += alphasX + (width / bpp);
        srcX = (srcX + width - 1) & FB_MASK;
        dstX = (dstX + width - 1) & FB_MASK;
    }
    else
    {
        srcLine += srcX >> FB_SHIFT;
        dstLine += dstX >> FB_SHIFT;
        alphasLine += alphasX;
        srcX &= FB_MASK;
        dstX &= FB_MASK;
    }

    if (srcX == dstX)
    {
        while (height--)
        {
            src = srcLine;
            srcLine += srcStride;
            alpha = alphasLine;
            alphasLine += alphasStride;
            dst = dstLine;
            dstLine += dstStride;
            if (reverse)
            {
                if (endmask)
                {
                    --alpha;
                    --src;
                    --dst;
                    FbDoRightMaskAlphaBlend(alpha, src, dst, &bits, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits, endbyte, endmask);
                }

                n = nmiddle;
                if (destInvarient)
                {
                    if (directWrite)
                    {
                        while (n--)
                        {
                            --alpha;
                            --src;
                            --dst;
                            FbDoAlphaBlendReverse(alpha, src, dst, &bits);
                            *dst = bits;
                        }
                    }
                    else
                    {
                        while (n--)
                        {
                            --alpha;
                            --src;
                            --dst;
                            FbDoAlphaBlendReverse(alpha, src, dst, &bits);
                            *dst = FbDoDestInvarientMergeRop(bits);
                        }
                    }
                }
                else
                {
                    while (n--)
                    {
                        --alpha;
                        --src;
                        --dst;
                        FbDoAlphaBlendReverse(alpha, src, dst, &bits);
                        *dst = FbDoMergeRop (bits, *dst);
                    }
                }

                if (startmask)
                {
                    --alpha;
                    --src;
                    --dst;
                    FbDoLeftMaskAlphaBlend(alpha, src, dst, &bits, startbyte);
                    FbDoLeftMaskByteMergeRop(dst, bits, startbyte, startmask);
                }
            }
            else
            {
                if (startmask)
                {
                    FbDoLeftMaskAlphaBlend(alpha, src, dst, &bits, startbyte);
                    alpha++;
                    src++;
                    FbDoLeftMaskByteMergeRop(dst, bits, startbyte, startmask);
                    dst++;
                }

                n = nmiddle;
                if (destInvarient)
                {
                    if (directWrite)
                    {
                        while (n--)
                        {
                            FbDoAlphaBlend(alpha, src, dst, &bits);
                            alpha++;
                            src++;
                            *dst++ = bits;
                        }
                    }
                    else
                    {
                        while (n--)
                        {
                            FbDoAlphaBlend(alpha, src, dst, &bits);
                            alpha++;
                            src++;
                            *dst++ = FbDoDestInvarientMergeRop(bits);
                        }
                    }
                }
                else
                {
                    while (n--)
                    {
                        FbDoAlphaBlend(alpha, src, dst, &bits);
                        alpha++;
                        src++;
                        *dst = FbDoMergeRop (bits, *dst);
                        dst++;
                    }
                }

                if (endmask)
                {
                    FbDoRightMaskAlphaBlend(alpha, src, dst, &bits, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits, endbyte, endmask);
                }
            }
        }
    }
    else
    {
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

        while (height--)
        {
            src = srcLine;
            srcLine += srcStride;
            alpha = alphasLine;
            alphasLine += alphasStride;
            dst = dstLine;
            dstLine += dstStride;

            bits1 = 0;
            if (reverse)
            {
                if (srcX < dstX)
                    bits1 = *--src;

                if (endmask)
                {
                    bits = FbScrRight(bits1, rightShift);
                    if (FbScrRight(endmask, leftShift))
                    {
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                    }
                    --dst;
                    --alpha;
                    FbDoRightMaskAlphaBlend(alpha, &bits, dst, &bits2, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits2, endbyte, endmask);
                }

                n = nmiddle;
                if (destInvarient)
                {
                    while (n--)
                    {
                        bits = FbScrRight(bits1, rightShift);
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                        --dst;
                        --alpha;
                        FbDoAlphaBlendReverse(alpha, &bits, dst, &bits2);
                        *dst = FbDoDestInvarientMergeRop(bits2);
                    }
                }
                else
                {
                    while (n--)
                    {
                        bits = FbScrRight(bits1, rightShift);
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                        --dst;
                        --alpha;
                        FbDoAlphaBlendReverse(alpha, &bits, dst, &bits2);
                        *dst = FbDoMergeRop(bits2, *dst);
                    }
                }

                if (startmask)
                {
                    bits = FbScrRight(bits1, rightShift);
                    if (FbScrRight(startmask, leftShift))
                    {
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                    }
                    --dst;
                    --alpha;
                    FbDoLeftMaskAlphaBlend(alpha, &bits, dst, &bits2, startbyte);
                    FbDoLeftMaskByteMergeRop (dst, bits2, startbyte, startmask);
                }
            }
            else
            {
                if (srcX > dstX)
                    bits1 = *src++;
                if (startmask)
                {
                    bits = FbScrLeft(bits1, leftShift);
                    bits1 = *src++;
                    bits |= FbScrRight(bits1, rightShift);
                    FbDoLeftMaskAlphaBlend(alpha, &bits, dst, &bits2, startbyte);
                    alpha++;
                    FbDoLeftMaskByteMergeRop (dst, bits2, startbyte, startmask);
                    dst++;
                }

                n = nmiddle;
                if (destInvarient)
                {
                    while (n--)
                    {
                        bits = FbScrLeft(bits1, leftShift);
                        bits1 = *src++;
                        bits |= FbScrRight(bits1, rightShift);
                        FbDoAlphaBlend(alpha, &bits, dst, &bits2);
                        alpha++;
                        *dst = FbDoDestInvarientMergeRop(bits2);
                        dst++;
                    }
                }
                else
                {
                    while (n--)
                    {
                        bits = FbScrLeft(bits1, leftShift);
                        bits1 = *src++;
                        bits |= FbScrRight(bits1, rightShift);
                        FbDoAlphaBlend(alpha, &bits, dst, &bits2);
                        alpha++;
                        *dst = FbDoMergeRop(bits2, *dst);
                        dst++;
                    }
                }

                if (endmask)
                {
                    bits = FbScrLeft(bits1, leftShift);
                    if (FbScrLeft(endmask, rightShift))
                    {
                        bits1 = *src;
                        bits |= FbScrRight(bits1, rightShift);
                    }
                    FbDoRightMaskAlphaBlend(alpha, &bits, dst, &bits2, endbyte);
                    FbDoRightMaskByteMergeRop (dst, bits2, endbyte, endmask);
                }
            }
        }
    }
}

#ifdef FB_TRANS
static void
FBTRANSBLT_8_16_32 (FbBits        *srcLine,
                    FbStride       srcStride,
                    int            srcX,

                    FbBits        *dstLine,
                    FbStride       dstStride,
                    int            dstX,

                    unsigned char *alphasLine,
                    FbStride       alphasStride,
                    int            alphasX,

                    int            width,
                    int            height,

                    int            alu,
                    FbBits         pm,
                    int            bpp,

                    Bool           reverse,
                    Bool           upsidedown)
{
    FbBits        *src, *dst;
    int           leftShift, rightShift;
    FbBits        startmask, endmask;
    FbBits        bits, bits1, bits2;
    int           n, nmiddle;
    Bool          destInvarient;
    int           startbyte, endbyte;
    int           directWrite = 0;
    unsigned char *alpha;
    FbDeclareMergeRop ();

    FbInitializeMergeRop(alu, pm);

    if (_ca2 + 1 == 0 && _cx2 == 0)
        directWrite = 1;

    destInvarient = FbDestInvarientMergeRop();

    if (upsidedown)
    {
        srcLine += (INT32)(height - 1) * (INT32)(srcStride);
        dstLine += (INT32)(height - 1) * (INT32)(dstStride);
        alphasLine += (INT32)(height - 1) * (INT32)(alphasStride);
        srcStride = -srcStride;
        dstStride = -dstStride;
        alphasStride = -alphasStride;
    }

    FbMaskBitsBytes (dstX, width, destInvarient, 
                     startmask, startbyte,
                     nmiddle, 
                     endmask, endbyte);
    if (reverse)
    {
        srcLine += ((srcX + width - 1) >> FB_SHIFT) + 1;
        dstLine += ((dstX + width - 1) >> FB_SHIFT) + 1;
        alphasLine += alphasX + (width / bpp);
        srcX = (srcX + width - 1) & FB_MASK;
        dstX = (dstX + width - 1) & FB_MASK;
    }
    else
    {
        srcLine += srcX >> FB_SHIFT;
        dstLine += dstX >> FB_SHIFT;
        alphasLine += alphasX;
        srcX &= FB_MASK;
        dstX &= FB_MASK;
    }

    if (srcX == dstX)
    {
        while (height--)
        {
            src = srcLine;
            srcLine += srcStride;
            alpha = alphasLine;
            alphasLine += alphasStride;
            dst = dstLine;
            dstLine += dstStride;
            if (reverse)
            {
                if (endmask)
                {
                    --alpha;
                    --src;
                    --dst;
                    FbDoRightMaskTransBlend(alpha, src, dst, &bits, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits, endbyte, endmask);
                }

                n = nmiddle;
                if (destInvarient)
                {
                    if (directWrite)
                    {
                        while (n--)
                        {
                            --alpha;
                            --src;
                            --dst;
                            FbDoTransBlendReverse(alpha, src, dst, &bits);
                            *dst = bits;
                        }
                    }
                    else
                    {
                        while (n--)
                        {
                            --alpha;
                            --src;
                            --dst;
                            FbDoTransBlendReverse(alpha, src, dst, &bits);
                            *dst = FbDoDestInvarientMergeRop(bits);
                        }
                    }
                }
                else
                {
                    while (n--)
                    {
                        --alpha;
                        --src;
                        --dst;
                        FbDoTransBlendReverse(alpha, src, dst, &bits);
                        *dst = FbDoMergeRop (bits, *dst);
                    }
                }

                if (startmask)
                {
                    --alpha;
                    --src;
                    --dst;
                    FbDoLeftMaskTransBlend(alpha, src, dst, &bits, startbyte);
                    FbDoLeftMaskByteMergeRop(dst, bits, startbyte, startmask);
                }
            }
            else
            {
                if (startmask)
                {
                    FbDoLeftMaskTransBlend(alpha, src, dst, &bits, startbyte);
                    alpha++;
                    src++;
                    FbDoLeftMaskByteMergeRop(dst, bits, startbyte, startmask);
                    dst++;
                }

                n = nmiddle;
                if (destInvarient)
                {
                    if (directWrite)
                    {
                        while (n--)
                        {
                            FbDoTransBlend(alpha, src, dst, &bits);
                            alpha++;
                            src++;
                            *dst++ = bits;
                        }
                    }
                    else
                    {
                        while (n--)
                        {
                            FbDoTransBlend(alpha, src, dst, &bits);
                            alpha++;
                            src++;
                            *dst++ = FbDoDestInvarientMergeRop(bits);
                        }
                    }
                }
                else
                {
                    while (n--)
                    {
                        FbDoTransBlend(alpha, src, dst, &bits);
                        alpha++;
                        src++;
                        *dst = FbDoMergeRop (bits, *dst);
                        dst++;
                    }
                }

                if (endmask)
                {
                    FbDoRightMaskTransBlend(alpha, src, dst, &bits, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits, endbyte, endmask);
                }
            }
        }
    }
    else
    {
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

        while (height--)
        {
            src = srcLine;
            srcLine += srcStride;
            alpha = alphasLine;
            alphasLine += alphasStride;
            dst = dstLine;
            dstLine += dstStride;

            bits1 = 0;
            if (reverse)
            {
                if (srcX < dstX)
                    bits1 = *--src;
                if (endmask)
                {
                    bits = FbScrRight(bits1, rightShift);
                    if (FbScrRight(endmask, leftShift))
                    {
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                    }
                    --dst;
                    --alpha;
                    FbDoRightMaskTransBlend(alpha, &bits, dst, &bits2, endbyte);
                    FbDoRightMaskByteMergeRop(dst, bits2, endbyte, endmask);
                }

                n = nmiddle;
                if (destInvarient)
                {
                    while (n--)
                    {
                        bits = FbScrRight(bits1, rightShift);
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                        --dst;
                        --alpha;
                        FbDoTransBlendReverse(alpha, &bits, dst, &bits2);
                        *dst = FbDoDestInvarientMergeRop(bits2);
                    }
                }
                else
                {
                    while (n--)
                    {
                        bits = FbScrRight(bits1, rightShift);
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                        --dst;
                        --alpha;
                        FbDoTransBlendReverse(alpha, &bits, dst, &bits2);
                        *dst = FbDoMergeRop(bits2, *dst);
                    }
                }

                if (startmask)
                {
                    bits = FbScrRight(bits1, rightShift);
                    if (FbScrRight(startmask, leftShift))
                    {
                        bits1 = *--src;
                        bits |= FbScrLeft(bits1, leftShift);
                    }
                    --dst;
                    --alpha;
                    FbDoLeftMaskTransBlend(alpha, &bits, dst, &bits2, startbyte);
                    FbDoLeftMaskByteMergeRop (dst, bits2, startbyte, startmask);
                }
            }
            else
            {
                if (srcX > dstX)
                    bits1 = *src++;
                if (startmask)
                {
                    bits = FbScrLeft(bits1, leftShift);
                    bits1 = *src++;
                    bits |= FbScrRight(bits1, rightShift);
                    FbDoLeftMaskTransBlend(alpha, &bits, dst, &bits2, startbyte);
                    alpha++;
                    FbDoLeftMaskByteMergeRop (dst, bits2, startbyte, startmask);
                    dst++;
                }

                n = nmiddle;
                if (destInvarient)
                {
                    while (n--)
                    {
                        bits = FbScrLeft(bits1, leftShift);
                        bits1 = *src++;
                        bits |= FbScrRight(bits1, rightShift);
                        FbDoTransBlend(alpha, &bits, dst, &bits2);
                        alpha++;
                        *dst = FbDoDestInvarientMergeRop(bits2);
                        dst++;
                    }
                }
                else
                {
                    while (n--)
                    {
                        bits = FbScrLeft(bits1, leftShift);
                        bits1 = *src++;
                        bits |= FbScrRight(bits1, rightShift);
                        FbDoTransBlend(alpha, &bits, dst, &bits2);
                        alpha++;
                        *dst = FbDoMergeRop(bits2, *dst);
                        dst++;
                    }
                }

                if (endmask)
                {
                    bits = FbScrLeft(bits1, leftShift);
                    if (FbScrLeft(endmask, rightShift))
                    {
                        bits1 = *src;
                        bits |= FbScrRight(bits1, rightShift);
                    }
                    FbDoRightMaskTransBlend(alpha, &bits, dst, &bits2, endbyte);
                    FbDoRightMaskByteMergeRop (dst, bits2, endbyte, endmask);
                }
            }
        }
    }
}
#endif /* FB_TRANS */
