/* $Revision: 1.26 $    $Date: 2009-11-01 01:54:31 $
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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaData.h"

/* Windows only */
#if defined(WIN32) && !defined(MICROWIN)
#pragma warning( disable: 4244 4305 )
#endif


#define INTERPOLATE(percent,lower,upper) (ALT_MULFX((upper - lower),percent))
#ifndef NO_ROTATE
extern ALTIA_FIXPT fxcosd(ALTIA_FIXPT);
extern ALTIA_FIXPT fxsind(ALTIA_FIXPT);
#endif






/*----------------------------------------------------------------------*/
void altiaLibTransformF(ALTIA_CONST Altia_Transform_type *trans,
                        ALTIA_DOUBLE x,
                        ALTIA_DOUBLE y,
                        ALTIA_FLOAT *newx,
                        ALTIA_FLOAT *newy)
{

    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        fx = ALT_MULFX(ALT_D2FX(x),trans->a00);
        fx += ALT_MULFX(ALT_D2FX(y),trans->a10) + trans->a20;
        fy = ALT_MULFX(ALT_D2FX(x),trans->a01);
        fy += ALT_MULFX(ALT_D2FX(y),trans->a11) + trans->a21;
    }
    *newx = ALT_FX2F(fx);
    *newy = ALT_FX2F(fy);


}


/*----------------------------------------------------------------------*/
void altiaLibTransformFix(ALTIA_CONST Altia_Transform_type *trans,
                          ALTIA_FIXPT x,
                          ALTIA_FIXPT y,
                          ALTIA_FIXPT *newx,
                          ALTIA_FIXPT *newy)
{
    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        fx = ALT_MULFX(x,trans->a00);
        fx += ALT_MULFX(y,trans->a10) + trans->a20;
        fy = ALT_MULFX(x,trans->a01);
        fy += ALT_MULFX(y,trans->a11) + trans->a21;
    }
    *newx = fx;
    *newy = fy;
}

/*----------------------------------------------------------------------*/
/* Determine if an object will be transformed by transforming the
 * corners and see what the difference is.
 */
ALTIA_BOOLEAN altiaLibTransformed(ALTIA_CONST Altia_Transform_type *trans,
                                  ALTIA_INT x,
                                  ALTIA_INT y)
{
 
 
    ALTIA_FIXPT fx, fy;
    ALTIA_FIXPT x1, y1, x2, y2, x3, y3;
    fx = ALT_I2FX(x);
    fy = ALT_I2FX(y);
    altiaLibTransformFix(trans, 0, 0, &x1, &y1);
    altiaLibTransformFix(trans, 0, fy, &x2, &y2);
    if (ALT_ROUND_FX2I(x2 - x1) != 0)
        return true;
    if (ALT_ROUND_FX2I(y2 - y1 - fy) != 0)
        return true;
    altiaLibTransformFix(trans, fx, 0, &x3, &y3);
    if (ALT_ROUND_FX2I(x3 - x1 - fx) != 0)
        return true;
    if (ALT_ROUND_FX2I(y3 - y1) != 0)
        return true;

    return false;
 
}

/*----------------------------------------------------------------------*/
void altiaLibTransform(ALTIA_CONST Altia_Transform_type *trans,
                       ALTIA_INT x,
                       ALTIA_INT y,
                       ALTIA_COORD *newx,
                       ALTIA_COORD *newy)
{

    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        fx = ALT_MULFX(ALT_I2FX(x),trans->a00);
        fx += ALT_MULFX(ALT_I2FX(y),trans->a10) + trans->a20;
        fy = ALT_MULFX(ALT_I2FX(x),trans->a01);
        fy += ALT_MULFX(ALT_I2FX(y),trans->a11) + trans->a21;
    }
    *newx = (ALTIA_COORD)(ALT_ROUND_FX2I(fx));
    *newy = (ALTIA_COORD)(ALT_ROUND_FX2I(fy));


}

/*----------------------------------------------------------------------*/
void altiaLibTransformDistF(ALTIA_CONST Altia_Transform_type *trans,
                            ALTIA_FLOAT x,
                            ALTIA_FLOAT y,
                            ALTIA_FLOAT *newx,
                            ALTIA_FLOAT *newy)
{

    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        fx = ALT_MULFX(x,trans->a00);
        fx += ALT_MULFX(y,trans->a10);
        fy = ALT_MULFX(x,trans->a01);
        fy += ALT_MULFX(y,trans->a11);
        *newx = fx;
        *newy = fy;
    }


}

/*----------------------------------------------------------------------*/
void altiaLibTransformDist(ALTIA_CONST Altia_Transform_type *trans,
                           ALTIA_INT x,
                           ALTIA_INT y,
                           ALTIA_COORD *newx,
                           ALTIA_COORD *newy)
{

    ALTIA_FLOAT fx;
    ALTIA_FLOAT fy;

    altiaLibTransformDistF(trans, ALT_I2F(x), ALT_I2F(y), &fx, &fy);
    *newx = (ALTIA_COORD)(ALT_ROUND_FX2I(fx));
    *newy = (ALTIA_COORD)(ALT_ROUND_FX2I(fy));


}

/*----------------------------------------------------------------------*/
void altiaLibInvTransformDistF(ALTIA_CONST Altia_Transform_type *trans,
                               ALTIA_DOUBLE x,
                               ALTIA_DOUBLE y,
                               ALTIA_FLOAT *newx,
                               ALTIA_FLOAT *newy)
{

    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        ALTIA_FIXPT d;
        ALTIA_FIXPT a;
        ALTIA_FIXPT b;
        d = ALT_MULFX(trans->a00, trans->a11);
        d -= ALT_MULFX(trans->a01, trans->a10);
        a = ALT_DIVFX(ALT_D2FX(x),d);
        b = ALT_DIVFX(ALT_D2FX(y),d);
        fx = ALT_MULFX(a, trans->a11);
        fx -= ALT_MULFX(b,trans->a10);
        fy = ALT_MULFX(b, trans->a00);
        fy -= ALT_MULFX(a,trans->a01);
    }
    *newx = ALT_FX2F(fx);
    *newy = ALT_FX2F(fy);


}

/*----------------------------------------------------------------------*/
void altiaLibInvTransformF(ALTIA_CONST Altia_Transform_type *trans,
                           ALTIA_DOUBLE x,
                           ALTIA_DOUBLE y,
                           ALTIA_FLOAT *newx,
                           ALTIA_FLOAT *newy)
{

    ALTIA_FIXPT fx;
    ALTIA_FIXPT fy;

    {
        ALTIA_FIXPT d;
        ALTIA_FIXPT a;
        ALTIA_FIXPT b;
        d = ALT_MULFX(trans->a00, trans->a11);
        d -= ALT_MULFX(trans->a01, trans->a10);
        a = ALT_DIVFX(ALT_D2FX(x) - trans->a20,d);
        b = ALT_DIVFX(ALT_D2FX(y) - trans->a21,d);
        fx = ALT_MULFX(a, trans->a11);
        fx -= ALT_MULFX(b,trans->a10);
        fy = ALT_MULFX(b, trans->a00);
        fy -= ALT_MULFX(a,trans->a01);
    }
    *newx = ALT_FX2F(fx);
    *newy = ALT_FX2F(fy);


}

/*----------------------------------------------------------------------*/
void altiaLibTranslate(Altia_Transform_type *trans,
                       ALTIA_DOUBLE x,
                       ALTIA_DOUBLE y)
{
    if (trans)
    {

        ALTIA_FIXPT fixx = ALT_D2FX(x);
        ALTIA_FIXPT fixy = ALT_D2FX(y);

        {
            trans->a20 += fixx;
            trans->a21 += fixy;

            trans->xoffset += fixx;
            trans->yoffset += fixy;

        }


    }
}

/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaLibTransformInvertable(ALTIA_CONST Altia_Transform_type *trans)
{

    return ((ALT_MULFX(trans->a00,trans->a11) - ALT_MULFX(trans->a01,trans->a10)) != 0);


}


/*----------------------------------------------------------------------*/
void altiaLibInvertRel(Altia_Transform_type *trans)
{


    trans->angle = -trans->angle;
    trans->xoffset = -trans->xoffset;
    trans->yoffset = -trans->yoffset;


    {
        ALTIA_FIXPT d;
        ALTIA_FIXPT t00;

        ALTIA_FIXPT t20;
        trans->a20 += trans->xoffset;
        trans->a21 += trans->yoffset;

        d = ALT_MULFX(trans->a00, trans->a11);
        d -= ALT_MULFX(trans->a01, trans->a10);

        t00 = trans->a00;
        t20 = trans->a20;
        trans->a20 = ALT_DIVFX((ALT_MULFX(trans->a10,trans->a21) - 
                                ALT_MULFX(trans->a11,trans->a20)), d);
        trans->a21 = ALT_DIVFX((ALT_MULFX(trans->a01,t20) - 
                                ALT_MULFX(trans->a00,trans->a21)), d);
        trans->a00 = ALT_DIVFX(trans->a11,d);
        trans->a11 = ALT_DIVFX(t00,d);
        trans->a10 = ALT_DIVFX(-trans->a10,d);
        trans->a01 = ALT_DIVFX(-trans->a01,d);

        /* Invert offsets */
        trans->a20 += trans->xoffset;
        trans->a21 += trans->yoffset;


    }


}

/*----------------------------------------------------------------------*/
void altiaLibInvertTrans(Altia_Transform_type *trans)
{


    trans->angle = -trans->angle;


    {
        ALTIA_FIXPT d;
        ALTIA_FIXPT t00;
        ALTIA_FIXPT t20;

        d = ALT_MULFX(trans->a00, trans->a11);
        d -= ALT_MULFX(trans->a01, trans->a10);

        t00 = ALT_DIVFX((ALT_MULFX(trans->a10,trans->yoffset) - 
                         ALT_MULFX(trans->a11,trans->xoffset)), d);
        trans->yoffset = ALT_DIVFX((ALT_MULFX(trans->a01,trans->xoffset) - 
                         ALT_MULFX(trans->a00,trans->yoffset)), d);
        trans->xoffset = t00;

        t00 = trans->a00;
        t20 = trans->a20;
        trans->a20 = ALT_DIVFX((ALT_MULFX(trans->a10,trans->a21) - 
                                ALT_MULFX(trans->a11,trans->a20)), d);
        trans->a21 = ALT_DIVFX((ALT_MULFX(trans->a01,t20) - 
                                ALT_MULFX(trans->a00,trans->a21)), d);
        trans->a00 = ALT_DIVFX(trans->a11,d);
        trans->a11 = ALT_DIVFX(t00,d);
        trans->a10 = ALT_DIVFX(-trans->a10,d);
        trans->a01 = ALT_DIVFX(-trans->a01,d);
    }


}

#ifndef NO_ROTATE
/*----------------------------------------------------------------------*/
void altiaLibRotate(Altia_Transform_type *trans,
                    ALTIA_DOUBLE a)
{

    ALTIA_FIXPT m00, m01, m10, m11, m20, m21;
    ALTIA_FIXPT tmp1, tmp2;
    ALTIA_FIXPT newangle;

    newangle = ALT_D2FX(a);

    trans->angle += newangle;

    /* Note fxcosd and fxsind use degrees not radians! */
    tmp1 = fxcosd(newangle);
    tmp2 = fxsind(newangle);
    m00 = ALT_MULFX(trans->a00,tmp1);
    m01 = ALT_MULFX(trans->a01,tmp2);
    m10 = ALT_MULFX(trans->a10,tmp1);
    m11 = ALT_MULFX(trans->a11,tmp2);
    m20 = ALT_MULFX(trans->a20,tmp1);
    m21 = ALT_MULFX(trans->a21,tmp2);

    trans->a01 = ALT_MULFX(trans->a00,tmp2) + ALT_MULFX(trans->a01,tmp1);
    trans->a11 = ALT_MULFX(trans->a10,tmp2) + ALT_MULFX(trans->a11,tmp1);
    trans->a21 = ALT_MULFX(trans->a20,tmp2) + ALT_MULFX(trans->a21,tmp1);
    trans->a00 = m00 - m01;
    trans->a10 = m10 - m11;
    trans->a20 = m20 - m21;



}
#endif

/*----------------------------------------------------------------------*/
void altiaLibScale(Altia_Transform_type *trans,
                   ALTIA_DOUBLE sx,
                   ALTIA_DOUBLE sy)
{

    ALTIA_FIXPT fx, fy;

    fx = ALT_D2FX(sx);
    fy = ALT_D2FX(sy);
    trans->a00 = ALT_MULFX(trans->a00, fx);
    trans->a01 = ALT_MULFX(trans->a01, fy);
    trans->a10 = ALT_MULFX(trans->a10, fx);
    trans->a11 = ALT_MULFX(trans->a11, fy);
    trans->a20 = ALT_MULFX(trans->a20, fx);
    trans->a21 = ALT_MULFX(trans->a21, fy);



}

/*----------------------------------------------------------------------*/
void altiaLibRoundExtent(ALTIA_FLOAT x00, ALTIA_FLOAT y00,
                         ALTIA_FLOAT x11, ALTIA_FLOAT y11,
                         Altia_Extent_type *newe)
{

    ALTIA_COORD cval;
    ALTIA_FIXPT fval;
    ALTIA_COORD rval;

    /* If we are within a tolerance of a whole number use that
     * number else round down on the lower left corner and up
     * on the upper right.
     */
    cval = (ALTIA_COORD) ALT_FX2I(x00);
    fval = ALT_I2FX(cval);
    rval = (ALTIA_COORD)(ALT_ROUND_FX2I(x00));

    /* if (x00 <= fval + 1 && x00 >= fval -1)
     *     newe->x0 = rval;
     * else
     */
    if (x00 < fval)
        newe->x0 = cval - 1;
    else
        newe->x0 = cval;

    cval = (ALTIA_COORD) ALT_FX2I(y00);
    fval = ALT_I2FX(cval);
    rval = (ALTIA_COORD)(ALT_ROUND_FX2I(y00));

    /* if (y00 <= fval + 1 && y00 >= fval -1)
     *     newe->y0 = rval;
     * else
     */
    if (y00 < fval)
        newe->y0 = cval - 1;
    else
        newe->y0 = cval;

    cval = (ALTIA_COORD) ALT_FX2I(x11);
    fval = ALT_I2FX(cval);
    rval = (ALTIA_COORD)(ALT_ROUND_FX2I(x11));

    /* if (x11 <= fval + 1 && x11 >= fval -1)
     *     newe->x1 = rval;
     * else 
     */
    if (x11 > fval)
        newe->x1 = cval + 1;
    else
        newe->x1 = cval;

    cval = (ALTIA_COORD) ALT_FX2I(y11);
    fval = ALT_I2FX(cval);
    rval = (ALTIA_COORD)(ALT_ROUND_FX2I(y11));

    /* if (y11 <= fval + 1 && y11 >= fval -1)
     *     newe->y1 = rval;
     * else 
     */
    if (y11 > fval)
        newe->y1 = cval + 1;
    else
        newe->y1 = cval;


}

/* If float precision is required for the extent set AltiaFloatExtent to
 * one before calling altiaLibTransformExtent.  It will then put
 * the floating point values of the extent in the AltiaExtent* globals.
 * The user should then set AltiaFloatExtent back to zero after the call.
 */
ALTIA_INT AltiaFloatExtent = 0;
ALTIA_FLOAT AltiaExtentX0;
ALTIA_FLOAT AltiaExtentY0;
ALTIA_FLOAT AltiaExtentX1;
ALTIA_FLOAT AltiaExtentY1;

/*----------------------------------------------------------------------*/
void altiaLibTransformExtent(ALTIA_CONST Altia_Transform_type *trans,
                             ALTIA_CONST Altia_Extent_type *olde,
                             Altia_Extent_type *newe)
{

    ALTIA_FLOAT tx00, ty00, tx10, ty10, tx11, ty11, tx01, ty01;
    ALTIA_FIXPT x00, y00, x11, y11;

    altiaLibTransformF(trans, ALT_I2F(olde->x0), ALT_I2F(olde->y0),
                      &tx00, &ty00);
    altiaLibTransformF(trans, ALT_I2F(olde->x1), ALT_I2F(olde->y0), 
                      &tx10, &ty10);
    altiaLibTransformF(trans, ALT_I2F(olde->x1), ALT_I2F(olde->y1), 
                      &tx11, &ty11);
    altiaLibTransformF(trans, ALT_I2F(olde->x0), ALT_I2F(olde->y1), 
                      &tx01, &ty01);
    x00 = ALTIA_MIN(ALT_F2FX(tx00), ALTIA_MIN(ALT_F2FX(tx01), 
                ALTIA_MIN(ALT_F2FX(tx10), ALT_F2FX(tx11))));
    y00 = ALTIA_MIN(ALT_F2FX(ty00), ALTIA_MIN(ALT_F2FX(ty01), 
                ALTIA_MIN(ALT_F2FX(ty10), ALT_F2FX(ty11))));
    x11 = ALTIA_MAX(ALT_F2FX(tx00), ALTIA_MAX(ALT_F2FX(tx01), 
                ALTIA_MAX(ALT_F2FX(tx10), ALT_F2FX(tx11))));
    y11 = ALTIA_MAX(ALT_F2FX(ty00), ALTIA_MAX(ALT_F2FX(ty01),
                ALTIA_MAX(ALT_F2FX(ty10), ALT_F2FX(ty11))));
    if (AltiaFloatExtent)
    {
        AltiaExtentX0 = x00;
        AltiaExtentY0 = y00;
        AltiaExtentX1 = x11;
        AltiaExtentY1 = y11;
    }
    altiaLibRoundExtent(x00, y00, x11, y11, newe);


}



/*----------------------------------------------------------------------*/
void altiaLibPostMultiplyTrans(Altia_Transform_type *trans,
                               ALTIA_CONST Altia_Transform_type *trans2)
{

    ALTIA_FIXPT tmp;

    trans->angle += trans2->angle;


    {

        tmp = ALT_MULFX(trans->xoffset, trans2->a00) + 
              ALT_MULFX(trans->yoffset, trans2->a10) + trans2->xoffset;
        trans->yoffset = ALT_MULFX(trans->xoffset,trans2->a01) + 
                         ALT_MULFX(trans->yoffset, trans2->a11) + trans2->yoffset;
        trans->xoffset = tmp;

        /* body of postmultiply */
        tmp = ALT_MULFX(trans->a00, trans2->a01) + 
              ALT_MULFX(trans->a01,trans2->a11);
        trans->a00 = ALT_MULFX(trans->a00, trans2->a00) + 
                     ALT_MULFX(trans->a01, trans2->a10);
        trans->a01 = tmp;
        tmp = ALT_MULFX(trans->a10, trans2->a01) + 
              ALT_MULFX(trans->a11,trans2->a11);
        trans->a10 = ALT_MULFX(trans->a10, trans2->a00) + 
                     ALT_MULFX(trans->a11, trans2->a10);
        trans->a11 = tmp;
        tmp = ALT_MULFX(trans->a20, trans2->a01) + 
              ALT_MULFX(trans->a21,trans2->a11);
        trans->a20 = ALT_MULFX(trans->a20, trans2->a00) + 
                     ALT_MULFX(trans->a21, trans2->a10);
        trans->a21 = tmp;
        trans->a20 += trans2->a20;
        trans->a21 += trans2->a21;
        /* end of body */
    }


}

/*----------------------------------------------------------------------*/
void altiaLibPostMultiplyRel(Altia_Transform_type *trans,
                             ALTIA_CONST Altia_Transform_type *trans2)
{

    ALTIA_FIXPT tmp;

    ALTIA_FIXPT savex, savey;

    savex = trans->xoffset;
    savey = trans->yoffset;
    trans->angle += trans2->angle;
    trans->xoffset += trans2->xoffset;
    trans->yoffset += trans2->yoffset;


    {

        trans->a20 -= savex;
        trans->a21 -= savey;

        /* body of postmultiply */
        tmp = ALT_MULFX(trans->a00, trans2->a01) + 
              ALT_MULFX(trans->a01,trans2->a11);
        trans->a00 = ALT_MULFX(trans->a00, trans2->a00) + 
                     ALT_MULFX(trans->a01, trans2->a10);
        trans->a01 = tmp;

        tmp = ALT_MULFX(trans->a10, trans2->a01) + 
              ALT_MULFX(trans->a11,trans2->a11);
        trans->a10 = ALT_MULFX(trans->a10, trans2->a00) + 
                     ALT_MULFX(trans->a11, trans2->a10);
        trans->a11 = tmp;

        tmp = ALT_MULFX(trans->a20, trans2->a01) + 
              ALT_MULFX(trans->a21,trans2->a11);
        trans->a20 = ALT_MULFX(trans->a20, trans2->a00) + 
                     ALT_MULFX(trans->a21, trans2->a10);
        trans->a21 = tmp;

        trans->a20 += trans2->a20;
        trans->a21 += trans2->a21;
        /* end of body */

        trans->a20 += savex;
        trans->a21 += savey;

    }



}

/*----------------------------------------------------------------------*/
void altiaLibPreMultiplyTrans(Altia_Transform_type *trans,
                              ALTIA_CONST Altia_Transform_type *trans2)
{

    ALTIA_FIXPT tmp1, tmp2;

    trans->angle += trans2->angle;


    {
        tmp1 = trans->a00;
        tmp2 = trans->a10;

        trans->xoffset = ALT_MULFX(trans2->xoffset, trans->a00) + 
                         ALT_MULFX(trans2->yoffset, trans->a10) + trans->xoffset;
        trans->yoffset = ALT_MULFX(trans2->xoffset, trans->a01) + 
                         ALT_MULFX(trans2->yoffset, trans->a11) + trans->yoffset;

        trans->a00 = ALT_MULFX(trans2->a00, tmp1) + 
                     ALT_MULFX(trans2->a01, tmp2);
        trans->a10 = ALT_MULFX(trans2->a10, tmp1) + 
                     ALT_MULFX(trans2->a11, tmp2);
        trans->a20 += ALT_MULFX(trans2->a20, tmp1) + 
                      ALT_MULFX(trans2->a21, tmp2);
        tmp1 = trans->a01;
        tmp2 = trans->a11;
        trans->a01 = ALT_MULFX(trans2->a00, tmp1) + 
                     ALT_MULFX(trans2->a01, tmp2);
        trans->a11 = ALT_MULFX(trans2->a10, tmp1) + 
                     ALT_MULFX(trans2->a11, tmp2);
        trans->a21 += ALT_MULFX(trans2->a20, tmp1) + 
                      ALT_MULFX(trans2->a21, tmp2);
    }


}

/*----------------------------------------------------------------------*/
void altiaLibDoRelTrans(Altia_Transform_type *current,
                        ALTIA_CONST Altia_Transform_type *behave,
                        ALTIA_CONST Altia_Transform_type *last,
                        ALTIA_CONST Altia_Transform_type *group,
                        ALTIA_SHORT preMult)
{
    /* The last trans is really the objects base trans
     * (ie the base transform with all behavior removed.  We can get by
     * with using this in deepscreen since no more behavior is going to be
     * defined.
     */

    altiaLibCopyTrans(current, last);

        altiaLibPostMultiplyRel(current, behave);
}



/*----------------------------------------------------------------------*/


void altiaLibInterpolate(Altia_Transform_type *trans,
                         ALTIA_FIXPT percent,
                         ALTIA_CONST Altia_Transform_type *trans2)

{

    ALTIA_FIXPT s00,s01,s10,s11;

    s00 = trans->a00; s01 = trans->a01; s10 = trans->a10; s11 = trans->a11;
    trans->a00 += INTERPOLATE(percent,trans->a00,trans2->a00);
    trans->a01 += INTERPOLATE(percent,trans->a01,trans2->a01);
    trans->a10 += INTERPOLATE(percent,trans->a10,trans2->a10);
    trans->a11 += INTERPOLATE(percent,trans->a11,trans2->a11);
    if ((ALT_MULFX(trans->a00,trans->a11) - 
         ALT_MULFX(trans->a01,trans->a10)) == 0)
    {  /* we don't want to create a non-invertable transform */
        ALTIA_FIXPT tiny = 1;
        trans->a00 = s00; trans->a01 = s01; trans->a10 = s10; trans->a11 = s11;
        if (percent > tiny)
            percent -= tiny;
        else
            percent += tiny;
        trans->a00 += INTERPOLATE(percent,trans->a00,trans2->a00);
        trans->a01 += INTERPOLATE(percent,trans->a01,trans2->a01);
        trans->a10 += INTERPOLATE(percent,trans->a10,trans2->a10);
        trans->a11 += INTERPOLATE(percent,trans->a11,trans2->a11);
    }
    trans->a20 += INTERPOLATE(percent,trans->a20,trans2->a20);
    trans->a21 += INTERPOLATE(percent,trans->a21,trans2->a21);

    trans->angle += INTERPOLATE(percent,trans->angle,trans2->angle);
    trans->xoffset += INTERPOLATE(percent,trans->xoffset,trans2->xoffset);
    trans->yoffset += INTERPOLATE(percent,trans->yoffset,trans2->yoffset);




}


