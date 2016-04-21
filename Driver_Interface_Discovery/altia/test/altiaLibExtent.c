/* $Revision: 1.8 $    $Date: 2009-02-23 21:50:03 $
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


extern ALTIA_INT AltiaFloatExtent;
extern ALTIA_FLOAT AltiaExtentX0;
extern ALTIA_FLOAT AltiaExtentY0;
extern ALTIA_FLOAT AltiaExtentX1;
extern ALTIA_FLOAT AltiaExtentY1;

/*----------------------------------------------------------------------*/
#ifdef Altiafp
void altiaLibRectExtent(Altia_Extent_type *olde, ALTIA_SHORT bwidth,
                        Altia_Transform_type *trans,
                        Altia_Extent_type *newe)
#else
void altiaLibRectExtent(olde, bwidth, trans, newe)
Altia_Extent_type *olde;
ALTIA_SHORT bwidth;
Altia_Transform_type *trans;
Altia_Extent_type *newe;
#endif
{
    ALTIA_FLOAT tol;
    int prevFloat = 0;

    if (bwidth > 1)
    {
	if (AltiaFloatExtent)
	{
	    prevFloat = 1;
	}else
	{
	    AltiaFloatExtent = 1;
	    prevFloat = 0;
        }
    }
    altiaLibTransformExtent(trans, olde, newe);
    if (bwidth > 1)
    {
	if (prevFloat == 0)
	    AltiaFloatExtent = 0;


        tol = ALT_MULFX(ALT_I2FX(bwidth), ALT_C2FX(0.5f));
	AltiaExtentX0 = FSUB(AltiaExtentX0, tol);
	AltiaExtentY0 = FSUB(AltiaExtentY0, tol);
	AltiaExtentX1 = FADD(AltiaExtentX1, tol);
	AltiaExtentY1 = FADD(AltiaExtentY1, tol);
	altiaLibRoundExtent(AltiaExtentX0, AltiaExtentY0,
	                    AltiaExtentX1, AltiaExtentY1, newe);

    }
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN altiaLibIntersect(Altia_Extent_type *e1, Altia_Extent_type *e2)
#else
ALTIA_BOOLEAN altiaLibIntersect(e1, e2)
Altia_Extent_type *e1;
Altia_Extent_type *e2;
#endif
{
    if ((e1->x0 <= e2->x1 && e2->x0 <= e1->x1 && 
         e1->y0 <= e2->y1 && e2->y0 <= e1->y1)
        ||
        (e2->x0 <= e1->x1 && e1->x0 <= e2->x1 && 
         e2->y0 <= e1->y1 && e1->y0 <= e2->y1))
    {
        return true;
    }else
        return false;

}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
void altiaLibSubExtent(Altia_Extent_type *e1, Altia_Extent_type *e2,
                       Altia_Extent_type *extent)
#else
void altiaLibSubExtent(e1, e2, extent)
Altia_Extent_type *e1;
Altia_Extent_type *e2;
Altia_Extent_type *extent;
#endif
{
    if (altiaLibIntersect(e1, e2))
    {
        extent->x0 = ALTIA_MAX(e1->x0, e2->x0);
        extent->y0 = ALTIA_MAX(e1->y0, e2->y0);
        extent->x1 = ALTIA_MIN(e1->x1, e2->x1);
        extent->y1 = ALTIA_MIN(e1->y1, e2->y1);
    }else
    {
	extent->x0 = extent->x1 = extent->y0 = extent->y1 = 0;
    }
}

/*----------------------------------------------------------------------*/
void altiaLibCoordExtent(ALTIA_CONST Altia_Coord_type *coords, ALTIA_SHORT cnt,
                         ALTIA_SHORT bwidth, Altia_Transform_type *trans,
                         Altia_Extent_type *newe)
{
    int i;
    ALTIA_COORD minx, miny;
    ALTIA_COORD maxx, maxy; 
    ALTIA_COORD tempx, tempy;
    Altia_Extent_type org;

    minx = maxx = coords[0].x;
    miny = maxy = coords[0].y;
    for (i = 1; i < cnt; i++)
    {
	tempx = coords[i].x;
	tempy = coords[i].y;
	if (tempx < minx)
	    minx = tempx;
	if (tempx > maxx)
	    maxx = tempx;
	if (tempy < miny)
	    miny = tempy;
	if (tempy > maxy)
	    maxy = tempy;
    }
    org.x0 = minx;
    org.y0 = miny;
    org.x1 = maxx;
    org.y1 = maxy;
    altiaLibRectExtent(&org, bwidth, trans, newe);
}
