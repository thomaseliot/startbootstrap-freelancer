/* $Revision: 1.2 $    $Date: 2009-09-30 22:48:28 $
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
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"




/* This routine does a fixed point square root. The algorithm
 * was taken from work done by Jaap Suter and Mark T. Price in the
 * Socrates game boy advance development kit.
 */
ALTIA_FIXPT fxsqrt(ALTIA_FIXPT value)
{
    unsigned long i = 0;
    unsigned long a = 0;
    unsigned long e = 0;
    unsigned long r = 0;
    int reciprocal = 0;
    ALTIA_FIXPT res;
    ALTIA_CONST int fixedOne = 1 << ALT_FSHIFT;

    /* Square roots of values less than one 
    ** should result in a larger value.  Instead
    ** this function will return a smaller
    ** value (because it's based upon integers)
    ** To fix the problem take the reciprocal of
    ** the small number, perform the root, then
    ** take the reciprocol again.
    */
    if (value < fixedOne)
    {
        reciprocal = 1;
        value = fxdivide(1, value);
    }
    for (; i < (32 >> 1); i++)
    {
        r <<= 2;
	r += ((unsigned long)value >> 30);
	value <<= 2;
	a <<= 1;
	e = (a << 1) | 1;
	if (r >= e)
	{
	    r -= e;
	    a++;
	}
    }
    /* The square root takes the middle bits */
    res = a << (ALT_FSHIFT/2);
    if (reciprocal)
        res = fxdivide(1, res);
    return res;
}

ALTIA_FIXPT fxhypot(ALTIA_FIXPT fx, ALTIA_FIXPT fy)
{
    ALTIA_FIXPT res = fxsqrt(ALT_MULFX(fx, fx) + ALT_MULFX(fy, fy));
    return (res);
}


ALTIA_CONST ALTIA_FIXPT g_SineAndCosineTable[ 256 + 64 ] = 
{
	(ALTIA_FIXPT)(        0.000000 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.024541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.049068 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.073565 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.098017 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.122411 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.146730 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.170962 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.195090 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.219101 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.242980 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.266713 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.290285 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.313682 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.336890 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.359895 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.382683 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.405241 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.427555 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.449611 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.471397 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.492898 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.514103 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.534998 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.555570 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.575808 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.595699 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.615232 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.634393 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.653173 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.671559 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.689541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.707107 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.724247 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.740951 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.757209 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.773010 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.788346 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.803208 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.817585 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.831470 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.844854 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.857729 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.870087 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.881921 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.893224 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.903989 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.914210 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.923880 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.932993 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.941544 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.949528 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.956940 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.963776 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.970031 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.975702 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.980785 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.985278 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.989177 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.992480 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.995185 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.997290 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.998795 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.999699 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        1.000000 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.999699 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.998795 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.997290 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.995185 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.992480 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.989177 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.985278 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.980785 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.975702 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.970031 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.963776 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.956940 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.949528 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.941544 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.932993 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.923880 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.914210 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.903989 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.893224 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.881921 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.870087 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.857729 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.844854 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.831470 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.817585 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.803208 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.788346 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.773010 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.757209 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.740951 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.724247 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.707107 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.689541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.671559 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.653173 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.634393 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.615232 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.595699 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.575808 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.555570 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.534998 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.514103 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.492898 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.471397 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.449611 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.427555 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.405241 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.382683 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.359895 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.336890 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.313682 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.290285 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.266713 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.242980 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.219101 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.195090 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.170962 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.146730 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.122411 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.098017 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.073565 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.049068 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.024541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.000000 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(       -0.024541 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.049068 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.073565 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.098017 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.122411 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.146730 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.170962 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.195090 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.219101 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.242980 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.266713 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.290285 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.313682 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.336890 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.359895 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.382683 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.405241 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.427555 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.449611 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.471397 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.492898 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.514103 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.534998 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.555570 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.575808 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.595699 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.615232 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.634393 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.653173 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.671559 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.689541 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.707107 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.724247 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.740951 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.757209 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.773010 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.788346 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.803208 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.817585 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.831470 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.844854 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.857729 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.870087 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.881921 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.893224 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.903989 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.914210 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.923880 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.932993 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.941544 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.949528 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.956940 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.963776 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.970031 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.975702 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.980785 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.985278 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.989177 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.992480 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.995185 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.997290 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.998795 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.999699 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -1.000000 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.999699 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.998795 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.997290 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.995185 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.992480 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.989177 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.985278 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.980785 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.975702 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.970031 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.963776 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.956940 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.949528 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.941544 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.932993 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.923880 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.914210 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.903989 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.893224 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.881921 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.870087 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.857729 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.844854 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.831470 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.817585 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.803208 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.788346 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.773010 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.757209 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.740951 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.724247 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.707107 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.689541 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.671559 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.653173 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.634393 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.615232 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.595699 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.575808 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.555570 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.534998 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.514103 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.492898 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.471397 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.449611 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.427555 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.405241 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.382683 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.359895 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.336890 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.313682 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.290285 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.266713 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.242980 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.219101 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.195090 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.170962 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.146730 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.122411 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.098017 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.073565 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.049068 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.024541 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(       -0.000000 * ((double)ALT_I2FX( 1 ))-0.5), 
	(ALTIA_FIXPT)(        0.024541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.049068 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.073565 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.098017 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.122411 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.146730 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.170962 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.195090 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.219101 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.242980 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.266713 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.290285 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.313682 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.336890 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.359895 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.382683 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.405241 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.427555 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.449611 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.471397 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.492898 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.514103 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.534998 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.555570 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.575808 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.595699 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.615232 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.634393 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.653173 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.671559 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.689541 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.707107 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.724247 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.740951 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.757209 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.773010 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.788346 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.803208 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.817585 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.831470 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.844854 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.857729 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.870087 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.881921 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.893224 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.903989 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.914210 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.923880 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.932993 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.941544 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.949528 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.956940 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.963776 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.970031 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.975702 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.980785 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.985278 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.989177 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.992480 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.995185 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.997290 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.998795 * ((double)ALT_I2FX( 1 ))+0.5), 
	(ALTIA_FIXPT)(        0.999699 * ((double)ALT_I2FX( 1 ))+0.5), 
}; 

/* This routine does a degree based sin. The algorithm and above table
 * was taken from work done by Jaap Suter in the
 * Socrates game boy advance development kit.
 */
ALTIA_FIXPT fxsind(ALTIA_FIXPT angle)
{
    const ALTIA_FIXPT *g_Sine = &g_SineAndCosineTable[0];
    ALTIA_FIXPT index = ALT_MULFX(ALT_DIVFX(ALT_I2FX(256),ALT_I2FX(360)),angle);
    int lindex = ALT_FX2I(index);
    long err = index - ALT_I2FX(lindex);
    /* err is the difference between the value in the table we are
     * pulling out versus how far beyond that entry we should be using.
     * Here we use linear interpolation of the values in the table to
     * try and provide a more accurate answer since our table values are
     * based on 256 entries in a circle instead of 360.  256 uses less
     * table space and allows for us just to take and the lower 8 bits of
     * the index.
     */
    if (err != 0)
    {
	int hindex = lindex+1;
	ALTIA_FIXPT low = g_Sine[lindex & 0xFF];
	ALTIA_FIXPT high = g_Sine[hindex & 0xFF];
	ALTIA_FIXPT interp = ALT_MULFX(ALT_DIVFX(err, (1 << ALT_FSHIFT)),
                                   (high - low));
        return low + interp;
    }else
        return g_Sine[lindex & 0xFF];
}
/* This function wants radians */
ALTIA_FIXPT fxsin(ALTIA_FIXPT angle)
{
    ALTIA_FIXPT degrees = ALT_MULFX(angle, ALT_C2FX(180.0f/A_PI));
    return fxsind(degrees);
}

/* This routine does a degree based cos. The algorithm
 * was taken from work done by Jaap Suter in the
 * Socrates game boy advance development kit.
 */
ALTIA_FIXPT fxcosd(ALTIA_FIXPT angle)
{
    const ALTIA_FIXPT *g_Cosine = &g_SineAndCosineTable[64];
    ALTIA_FIXPT index = ALT_MULFX(ALT_DIVFX(ALT_I2FX(256),ALT_I2FX(360)),angle);
    int lindex = ALT_FX2I(index);
    long err = index - ALT_I2FX(lindex);
    /* err is the difference between the value in the table we are
     * pulling out versus how far beyond that entry we should be using.
     * Here we use linear interpolation of the values in the table to
     * try and provide a more accurate answer since our table values are
     * based on 256 entries in a circle instead of 360.  256 uses less
     * table space and allows for us just to take and the lower 8 bits of
     * the index.
     */
    if (err != 0)
    {
	int hindex = lindex+1;
	ALTIA_FIXPT low = g_Cosine[lindex & 0xFF];
	ALTIA_FIXPT high = g_Cosine[hindex & 0xFF];
	ALTIA_FIXPT interp = ALT_MULFX(ALT_DIVFX(err, (1 << ALT_FSHIFT)),
                                   (high - low));
        return low + interp;
    }else
        return g_Cosine[lindex & 0xFF];
}

/* This function wants radians */
ALTIA_FIXPT fxcos(ALTIA_FIXPT angle)
{
    ALTIA_FIXPT degrees = ALT_MULFX(angle, ALT_C2FX(180.0f/A_PI));
    return fxcosd(degrees);
}

/* This routine does a fixed point arc tangent returning a fixed
 * point radian value.  It is derived from a fixed point 
 * atan2 algorithm with self normalization from Jim Shima posted
 * on the web and release to the public domain. Notice that is
 * uses the fxdivide function since that seemed to give better
 * results then the windows 64 bit divide.
 */

ALTIA_FIXPT fxatan2(ALTIA_FIXPT y, ALTIA_FIXPT x)
{
    ALTIA_CONST ALTIA_FIXPT coeff1 = ALT_DIVFX(ALT_C2FX(A_PI), ALT_C2FX(4));
    ALTIA_CONST ALTIA_FIXPT coeff2 = ALT_MULFX(ALT_C2FX(3), coeff1);
    ALTIA_FIXPT r;
    ALTIA_FIXPT angle;
    int neg = 0;

    if (y == 0 || y == x)
        y++;
    if (y < 0)
    {
       y = -y;
       neg = 1;
    }
    if (x >= 0)
    {
	ALTIA_FIXPT addv = x+y;
	/* Here we use our divide instead of the builtin 64 bit
	 * (if we have one) since we do a better job in this case
	 */
	if (addv == 1)
	    r = x - y;
	else
	    r = fxdivide(x - y, addv);
	angle = ALT_MULFX(ALT_MULFX(ALT_MULFX(ALT_C2FX(0.1963),r),r),r);
	angle -= ALT_MULFX(ALT_C2FX(0.9817),r);
	angle += coeff1;
    }else
    {
	ALTIA_FIXPT subv = y-x;
	/* Here we use our divide instead of the builtin 64 bit
	 * (if we have one) since we do a better job in this case
	 */
	if (subv == 1)
	    r = x + y;
	else
	    r = fxdivide(x + y, subv);
	angle = ALT_MULFX(ALT_MULFX(ALT_MULFX(ALT_C2FX(0.1963),r),r),r);
	angle -= ALT_MULFX(ALT_C2FX(0.9817),r);
	angle += coeff2;
    }
    if (neg)
        return -angle;
    else
        return angle;
}


