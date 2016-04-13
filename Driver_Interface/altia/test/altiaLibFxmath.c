/* $Revision: 1.6 $    $Date: 2009-10-01 23:12:34 $
 * Copyright (c) 2001 Altia Inc.
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

/* This definitions ensures proper declaration of the FX math
** functions when not using inline option.  This definition MUST
** appear before the #include for altiaBase.h
*/
#define ALTIA_LIB_FX_MATH

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"



typedef union
{
    unsigned int i;
    float f;
}IntFloatType;

#if !ALTIA_USE_INLINE_FX_MATH
static int noMsg = 0;

void fxoverflow(void)
{
    /* Display overflow error message only if it has not been
    ** shown yet.
    */
    if (noMsg)
        return;

    _altiaErrorMessage(ALT_TEXT("Fixed point math overflow!"));
    noMsg = 1;
}
#endif /* !ALTIA_USE_INLINE_FX_MATH */

#if 0
/* This converts a 32 bit ieee float number into a fixed point number */
ALTIA_FIXPT convF2FX(double a)
{
    IntFloatType x;
    ALTIA_FIXPT res;
    int exp;
    int sign;
    int shift;

    x.f = (float)a;
    exp = ((x.i >> 23) & 0xFF);
    sign = x.i & 0x80000000;
    if (exp != 0)
    {
       exp -= 127;
       res = (x.i & 0x7FFFFF) | 0x800000;
    }else
    {
       exp = -126;
       res = (x.i & 0x7FFFFF);
    }
    shift = 23 - ALT_FSHIFT - exp;
    if (shift < 0)
    {
	if (shift < -(32 - ALT_FSHIFT-1))
	{
	    if (noMsg)
	    {
		_altiaErrorMessage(ALT_TEXT("Conv F2FX overflow"));
		noMsg = 0;
	    }
	}
	res <<= -shift;
    }else
	res >>= shift;
    if (sign)
        res = -res;
    return res;
}
#endif


char leadZeroNibble[] = 
{
 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
};

int fxsignificant(ALTIA_FIXPT a)
{
    register int shift = 0;
    register unsigned long temp;
    if (a < 0)
       a = -a;
    temp = a;
    if (temp < 0x10000)
    {
	shift += 16;
	temp <<= 16; 
    }
    if (temp < 0x1000000)
    {
	shift += 8;
	temp <<= 8; 
    }
    if (temp < 0x10000000)
    {
	shift += 4;
	temp <<= 4; 
    }
    shift += leadZeroNibble[temp >> 28];
    return (32 - shift);
}

/* This is a 64 bit multiply.  It breaks the fixed point number
 * into a 32 bit whole number and a 32 bit fraction and uses
 * the following formula x * y = (wx + fx) * (wy + fy)
 * or x * y  = wx * wy + wx * fy + wy * fx + fx * fy.
 * where wx is whole x and fx is the fraction part of x
 */
#ifndef ALTIA_INLINE_MULTIPLY
ALTIA_FIXPT fxmultiply(ALTIA_FIXPT x, ALTIA_FIXPT y)
{
    long ix, iy, fx, fy, ires, fres, temp;
    ALTIA_FIXPT res;
    int negx = 0;
    int negy = 0;
    ALTIA_CONST long maxVal = (long)1 << (long)(31 - ALT_FSHIFT);

    if (x < 0)
    {
        negx = 1;
	x = -x;
    }
    if (y < 0)
    {
        negy = 1;
	y = -y;
    }
    ix = x >> ALT_FSHIFT;
    iy = y >> ALT_FSHIFT;
    fx = ((1 << ALT_FSHIFT) - 1) & x;
    fy = ((1 << ALT_FSHIFT) - 1) & y;
    ires = ix * iy;
    if (ires > maxVal)
    {
	if (noMsg)
	{
	    _altiaErrorMessage(ALT_TEXT("Multiply overflow"));
	    noMsg = 0;
	}
    }
    fres = ((ix) * fy) + ((iy) * fx);
    temp = (fy) * (fx);
    if (temp < 0)
	temp = ((fy >> 1) * (fx >> 1)) >> (ALT_FSHIFT-2); /* overflow */
    else
        temp >>= ALT_FSHIFT;
    res = ((ires << ALT_FSHIFT) + fres + temp);
    if (res < 0)
    {
	if (noMsg)
	{
	    _altiaErrorMessage(ALT_TEXT("Multiply overflow"));
	    noMsg = 0;
	}
    }
    if (negx != negy)
       res = -res;
    return res;
}

/* This is a 64 bit divide that relies on the 64 bit multiply.  It
 * uses the Goldschmidt algorithm to compute the reciprocal of y
 * and then multiply.  This algorithm was derived by work from
 * Pascal Massimino.
 */
ALTIA_FIXPT fxdivide(ALTIA_FIXPT x, ALTIA_FIXPT y)
{
    ALTIA_FIXPT e;
    ALTIA_FIXPT res;
    int negx = 0;
    int negy = 0;
    ALTIA_CONST int fixedOne = 1 << ALT_FSHIFT;

    if (x < 0)
    {
        negx = 1;
	x = -x;
    }
    if (y < 0)
    {
        negy = 1;
	y = -y;
    }

    if (y == 0)
        return 0;

    /* Here we assume a word size of 32 so we can only shift up to 30 */
#if (TWICE_SHIFT > 30)
	e = ((1L << 30) / y) << (TWICE_SHIFT-30); 
#else
        e = (1L << TWICE_SHIFT) / y;
#endif
    res = ALT_MULFX(x, e);
    e = fixedOne - (ALT_MULFX(y, e));
    while (e != 0)
    {
	/* since e is so small we can use regular multiply in the loop */
        res += (res * e) >> ALT_FSHIFT;
	e = (e * e) >> ALT_FSHIFT;
	if (e == fixedOne)
	    break;
    }
    if (negx != negy)
       res = -res;
    return res;
}
#endif /* ALTIA_INLINE_MULTIPLY */




