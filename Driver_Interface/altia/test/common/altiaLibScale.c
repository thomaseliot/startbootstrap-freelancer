/* $Revision: 1.37 $    $Date: 2010-03-31 16:59:09 $
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

/** FILE:  altiaLibScale.c *************************************************
 **
 ** This file contains the routines used during the rendering of
 ** scaled/stretched/rotated color and monochrome bitmaps.  These
 ** routines are only required if code was generated for one or more
 ** Altia raster, stencil, or text objects AND one or more of these
 ** objects have a scale, stretch, or rotation AND the scaled bitmaps
 ** option was enabled when code was generated.  Otherwise (i.e., no
 ** scaled/stretched/rotated rasters, stencils, or text), this file
 ** contains no code.
 **
 ** Monochrome and color bitmap scaling/stretching/rotating are definitely
 ** the most difficult operations to port from one target to another.  The
 ** comment header for each routine in this file identifies the degree of
 ** modification that may be required to support a different target.
 **
 ** ------------------------------
 ** INDEXED COLOR SUPPORT WARNING!
 ** ------------------------------
 ** Indexed color support (for example, 256 color mode) has
 ** been tested on some targets (WindML and X11 as of 12/12/2002),
 ** but maybe not others.  As a result, some code in this file is
 ** not tested on all targets. Search for "INDEXED COLOR SUPPORT WARNING"
 ** for more details.
 ***************************************************************************/




#include "egl_Wrapper.h"
#include <string.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaImageAccess.h"
#include "altiaLibDraw.h"



extern AltiaDevInfo *altiaLibGetDevInfo(void);

extern int altiaPaletteSize(
#ifdef Altiafp
int numBits
#endif
);

/* Need a simple definition for a point (x,y) data type */
typedef struct local_Altia_PT
{
    int x;
    int y;
} LOCAL_ALTIA_PT;

#define NoTx   0
#define Rot90  1
#define Rot180 2
#define Rot270 3

/* The following macros allow for determining the number of bytes
 * in a row of image data given the number of bits of image data
 * in the row.  The first macro gives the number of bytes aligned
 * on a 32-bit boundary.  The second macro gives the number of
 * bytes aligned on an 8-bit byte boundary.  Some targets require
 * the alignment of each row of data on a 32-bit boundary and the
 * BITSTOINT32(bits) macro should be used in these cases.  Others
 * targets, like this one, only require that a row of image data
 * align on a byte boundary and the BITSTOBYTES(bits) is used.
 */
#define BITSTOINT32(bits)    (((bits) + 31) / 32 * 4)
#define BITSTOBYTES(bits)    (((bits) + 7) / 8)

/* Bilinear
 *       y      s01     s11
 *     1 - y    s00     s10
 *             1 - x     x
 */


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


static ALTIA_UBYTE computeBilinear(ALTIA_UINT32 s00, ALTIA_UINT32 s10,
                                   ALTIA_UINT32 s01, ALTIA_UINT32 s11,
                                   ALTIA_DOUBLE dx, ALTIA_DOUBLE dy)
{
    ALTIA_FIXPT fx = ALT_F2FX(dx);
    ALTIA_FIXPT fy = ALT_F2FX(dy);
    ALTIA_FIXPT oneminusdy = ALT_C2FX(1) - fy;
    ALTIA_FIXPT fs00 = ALT_I2FX(s00);
    ALTIA_FIXPT fs01 = ALT_I2FX(s01);
    ALTIA_FIXPT fs10 = ALT_I2FX(s10);
    ALTIA_FIXPT fs11 = ALT_I2FX(s11);
    ALTIA_FIXPT res = ALT_MULFX(ALT_MULFX(fx, fs10 - fs00) + fs00, oneminusdy);

    res += ALT_MULFX(ALT_MULFX(fx, fs11 - fs01) + fs01, fy);
    return (ALTIA_UBYTE)ALT_FX2I(res);
}


#define GetBit(ptr, widthbytes, x, y) \
    (ptr[((widthbytes) * (y)) + ((x) / 8)] & (1 << (7 - ((x)%8))))

#define PutBitSet(ptr, widthbytes, x, y) \
    (ptr[((widthbytes) * (y)) + ((x) / 8)] |= (1 << (7 - ((x)%8))))

#define PutBitClear(ptr, widthbytes, x, y) \
    (ptr[((widthbytes) * (y)) + ((x) / 8)] &= ~(1 << (7 - ((x)%8))))



#define GetPixel24(result, ptr, widthbytes, x, y, temp) \
    temp = ((widthbytes) * (y)) + ((x) * 3); \
    result = (ptr[temp] << 16) | (ptr[temp+1] << 8) | ptr[temp+2];

#define GetPixel32(result, ptr, alpha, widthbytes, x, y, temp, width) \
    temp = ((widthbytes) * (y)) + ((x) * 4); \
    result = (ptr[temp] << 24)|(ptr[temp+1] << 16)|(ptr[temp+2]<< 8)|ptr[temp+3];


#define SetPixel24(ptr, widthbytes, x, y, r, g, b, temp) \
    (temp = ((widthbytes) * (y)) + ((x) * 3), \
     ptr[temp] = (r), ptr[temp+1] = (g), ptr[temp+2] = (b))

#define SetPixel32(ptr, widthbytes, x, y, a, r, g, b, temp) \
    (temp = ((widthbytes) * (y)) + ((x) * 4), \
    *((EGL_ARGB *) ((ALTIA_UBYTE *) ptr + temp)) \
        = EGL_MAKE_ARGB((a), (r), (g), (b)))


/*--- altia_rotation_offset() ---------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated rasters, stencils, or text.
 * It computes the x and y offset of the transformed bitmap relative to
 * the original x and y of the bitmap.  To get the x and y offset, a
 * translation is done on the incoming transform and the translation is
 * removed before the function returns.  This function should NOT require
 * changes between targets.
 *-------------------------------------------------------------------------*/
void altia_rotation_offset(int width, int height,
                           Altia_Transform_type *trans,
                           int *xoffset, int *yoffset)
{
    ALTIA_COORD X[4];
    ALTIA_COORD Y[4];
    ALTIA_COORD xorigin,yorigin;
    int i;

    altiaLibTransform(trans, 0, 0, &xorigin, &yorigin);
    altiaLibTranslate(trans, ALTIA_I2D(-xorigin),ALTIA_I2D(-yorigin));

    X[0] = 0; Y[0] = 0;
    X[1] = width; Y[1] = 0;
    X[2] = width; Y[2] = height;
    X[3] = 0; Y[3] = height;

    for (i = 0; i < 4; i++)
	altiaLibTransform(trans, X[i], Y[i], &X[i], &Y[i]);

    *xoffset = ALTIA_MIN(X[0],(ALTIA_MIN(X[1],(ALTIA_MIN(X[2],X[3])))));
    *yoffset = ALTIA_MIN(Y[0],(ALTIA_MIN(Y[1],(ALTIA_MIN(Y[2],Y[3])))));
    if (*xoffset < 0)
        (*xoffset)++;
    if (*yoffset < 0)
        (*yoffset)++;

    altiaLibTranslate(trans, ALTIA_I2D(xorigin),ALTIA_I2D(yorigin));
}


#if !defined(EGL_HAS_POLYREGIONCREATE) || (EGL_HAS_POLYREGIONCREATE < 1)
/*--- localRegionInclude() ------------------------------------------------
 *
 * Local utility function that makes the actual target graphics library
 * calls to add another rectangle to the given clip region.  This code is
 * very target specific.
 *-------------------------------------------------------------------------*/
static void localRegionInclude(EGL_REGION_ID region,
                               int x0, int y0, int x1, int y1)
{
    EGL_RECT includeRect;

    /* On this target, the region rectangle is inclusive of
     * the rectangle's coordinates.
     */
    includeRect.left = x0;
    includeRect.top = y0;
    includeRect.right = x1;
    includeRect.bottom = y1;

    egl_RegionRectInclude(region, &includeRect);
}
#endif  /* NOT EGL_HAS_POLYREGIONCREATE >= 0 */


#if !defined(EGL_HAS_POLYREGIONCREATE) || (EGL_HAS_POLYREGIONCREATE < 1)

/* Macro to init the Bresenham values for a line used to scan convert
 * the polygon sides to determine the clip mask. This could be used
 * as a basis to scan convert a polygon, but here we only need to worry
 * about a 4 sided convex polygon.
 */
#define INIT_INCS(dx, dy, m, m1, i1, i2, d) \
   m = (dx) / (dy); \
   if ((dx) < 0) \
   { \
       m1 = m - 1; \
       i1 = (-2 * (dx)) + (2 * (dy) * m1); \
       i2 = (-2 * (dx)) + (2 * (dy) * m); \
       d = (2 * m * (dy)) - (2 * (dx)) - (2 * (dy)); \
   } else  \
   { \
       m1 = m + 1; \
       i1 = (2 * (dx)) - (2 * (dy) * m1); \
       i2 = (2 * (dx)) - (2 * (dy) * m); \
       d = (-2 * m * (dy)) + (2 * (dx)); \
   }

/* Macro to determine each x value given a step in the y value.  Here
 * we assume that the y step is in the positive (increasing direction).
 * This algorithm will give the correct left side value and one beyond
 * the correct right side value so the right side will need to be
 * reduced by one.
 */
#define DOINCX(m, m1, i1, i2, d, x) \
   if ((m1) > 0) \
   { \
       if ((d) > 0) \
       { \
	   x += (m1); \
	   d += (i1); \
       }else \
       { \
	   x += (m); \
	   d += (i2); \
       } \
   }else \
   { \
       if ((d) >= 0) \
       { \
	   x += (m1); \
	   d += (i1); \
       }else \
       { \
	   x += (m); \
	   d += (i2); \
       } \
   }

/*--- localAddRegion() ----------------------------------------------------
 *
 * Local utility function used during altia_clip_rotate() if we have to
 * create our own polygon regions.  This function creates small rectangles
 * to approximate a polygon and adds them to the existing set of
 * rectangle regions.   This function should NOT require changes between
 * targets except for modifying the type for the polyRgn argument.  The
 * localRegionInclude() function makes the target specific calls for adding
 * another rectangle to the existing clip region.
 *-------------------------------------------------------------------------*/
static void localAddRegion(EGL_REGION_ID polyRgn,
                           LOCAL_ALTIA_PT *p01, LOCAL_ALTIA_PT *p02,
                           LOCAL_ALTIA_PT *p11, LOCAL_ALTIA_PT *p12)
{

    /* This uses the algorithm from X11's CreatePolyRegion. But without
     * the edge table since we are assuming convex polygons with 4 sides.
     * We want to choose pixels that are inside the polygon.  To do this
     * we add .5 to the starting x coord for both the left and right edges.
     * This will give us the first pixel which is inside the polygon for
     * the left edge and the first pixel outside the polygon on the right
     * edge. So if the edge on the right then subtract one from the result.
     */
    int y;
    int dy;
    int x2right; /* flag to indicate if line p1 is on the right side */
    LOCAL_ALTIA_PT p0l, p1l;
    LOCAL_ALTIA_PT p0r, p1r;

    int xl;  /* current x for left side. */
    int dxl; /* x distance for left size */
    int ml; /* whole number slope for left side */
    int m1l; /* whole number slope for left side +/- 1 */
    int i1l; /* increment amount 1 for left side */
    int i2l; /* increment aount 2 for left side */
    int dl; /* error term for left side */

    int xr;  /* current x for right side. */
    int dxr; /* x distance for right size */
    int mr; /* whole number slope for right side */
    int m1r; /* whole number slope for right side +/- 1 */
    int i1r; /* increment amount 1 for right side */
    int i2r; /* increment aount 2 for right side */
    int dr; /* error term for right side */

    int txl, txr; /* temp x values */

    dy = p02->y - p01->y;
    y = p01->y;

    if (dy == 0)
    { /* only one y value so only 1 region spaning the x's */
	int x3 = p02->x;
	int x4 = p12->x;
	int xmin = ALTIA_MIN(p01->x, ALTIA_MIN(p02->x,
	           ALTIA_MIN(p11->x,p12->x)));
	int xmax = ALTIA_MAX(p01->x, ALTIA_MAX(p02->x,
	           ALTIA_MAX(p11->x,p12->x)));

        y = p01->y;
	localRegionInclude(polyRgn, xmin, y, xmax, y);
        return;
    }
    /* Determine if the p1 line is on the right side so we can
     * adjust to insure our points are inside the lines.
     */
    if (p12->x > p02->x)
    {
        x2right = 1;
    }else
    {
        if (p12->x == p02->x)
	{
	    if (p11->x > p01->x)
	        x2right = 1;
	    else
	        x2right = 0;
	}else
	{
	    x2right = 0;
	}
    }
    /* to use less code copy the points to temp values that indicate
     * the left and the right.
     */
    if (x2right)
    {
       p0r = *p11;
       p1r = *p12;
       p0l = *p01;
       p1l = *p02;
    }else
    {
       p0l = *p11;
       p1l = *p12;
       p0r = *p01;
       p1r = *p02;
    }
   dxl = p1l.x - p0l.x;
   xl = p0l.x;
   INIT_INCS(dxl, dy, ml, m1l, i1l, i2l, dl);
   dxr = p1r.x - p0r.x;
   xr = p0r.x;
   INIT_INCS(dxr, dy, mr, m1r, i1r, i2r, dr);
   /* Lets include the first block */
   txl = p0l.x;
   txr = p0r.x - 1;
   if (txr >= txl)
       localRegionInclude(polyRgn, txl, y, txr, y);
   for (y = y+1; y < p02->y; y++)
   {
       DOINCX(ml, m1l, i1l, i2l, dl, xl);
       txl = xl;
       DOINCX(mr, m1r, i1r, i2r, dr, xr);
       txr = xr - 1;
       if (txr >= txl)
	   localRegionInclude(polyRgn, txl, y, txr, y);
   }
   txl = p1l.x;
   txr = p1r.x - 1;
   if (txr >= txl)
       localRegionInclude(polyRgn, txl, p02->y, txr, p02->y);
}
#endif  /* NOT EGL_HAS_POLYREGIONCREATE >= 0 */


#if !defined(EGL_HAS_POLYREGIONCREATE) || (EGL_HAS_POLYREGIONCREATE < 1)
/*--- localIntersectPt() --------------------------------------------------
 *
 * Local utility function used during altia_clip_rotate() if we have to
 * create our own polygon regions.  This function should NOT require
 * changes between targets.
 *-------------------------------------------------------------------------*/
static void localIntersectPt(int y, LOCAL_ALTIA_PT *p1,
                             LOCAL_ALTIA_PT *p2, LOCAL_ALTIA_PT *pt)
{
    /* If we have a vertical line, x of the intersection is easy and
     * we want to take the easy route to avoid a divide by 0 trying
     * to compute the slope of a vertical line.
     */
    if (p2->x == p1->x)
	pt->x = p1->x;
    else
    {
	ALTIA_DOUBLE slope = FDIV(ALTIA_I2F(p2->y - p1->y) ,
	                          ALTIA_I2F(p2->x - p1->x));
	int x = ALTIA_ROUND(FDIV(FADD(ALTIA_I2F(y - p1->y),
	              FMULT(slope, ALTIA_I2F(p1->x))), slope));

	pt->x = x;
    }

    pt->y = y;
}
#endif  /* NOT EGL_HAS_POLYREGIONCREATE >= 0 */


#if !defined(EGL_HAS_POLYREGIONCREATE) || (EGL_HAS_POLYREGIONCREATE < 1)
/*--- localCreateMyPolygonRgn() -------------------------------------------
 *
 * Local utility function used during altia_clip_rotate() if we have to
 * create our own polygon regions by merging lots of rectangle regions
 * to approximate polygon regions.  This function should require only small
 * changes between targets.  In particular, it may be necessary to modify
 * the type for the polyRgn variable and the call that creates a clip region
 * because these items may be target specific.  The overall logic of the
 * function should NOT require changing between targets.
 *-------------------------------------------------------------------------*/
static EGL_REGION_ID localCreateMyPolygonRgn(LOCAL_ALTIA_PT *P)
{
    EGL_REGION_ID polyRgn;
    int minx, miny;
    int maxx, maxy;

    minx = ALTIA_MIN(P[0].x, ALTIA_MIN(P[1].x, ALTIA_MIN(P[2].x, P[3].x)));
    miny = ALTIA_MIN(P[0].y, ALTIA_MIN(P[1].y, ALTIA_MIN(P[2].y, P[3].y)));
    maxx = ALTIA_MAX(P[0].x, ALTIA_MAX(P[1].x, ALTIA_MAX(P[2].x, P[3].x)));
    maxy = ALTIA_MAX(P[0].y, ALTIA_MAX(P[1].y, ALTIA_MAX(P[2].y, P[3].y)));

    if ((P[0].x == P[1].x || P[0].y == P[1].y) &&
        (P[0].x == P[3].x || P[0].y == P[3].y) &&
        (P[2].x == P[3].x || P[2].y == P[3].y) &&
        (P[2].x == P[1].x || P[2].y == P[1].y))
    {
        /* We are still a rectangle */
        polyRgn = egl_RegionCreate();
        localRegionInclude(polyRgn, minx, miny, maxx, maxy);
        return polyRgn;
    }
    else
    {
        /* We have to create the region line by line since this target does
         * not have polygon regions.
         */
        int i, j;
        LOCAL_ALTIA_PT OP[4];
        LOCAL_ALTIA_PT P1[3];
        LOCAL_ALTIA_PT P2[3];
        LOCAL_ALTIA_PT PT;
        LOCAL_ALTIA_PT PT2;
        int E1x,E1y,E2x,E2y;

        /* First order the points so 0 is bottom, 1 left, 2 right, 3 top.
         * Since the original points are always in CW or CCW order, the
         * trick is to find the bottom point and then determine the direction.
         * After that the re-order can be exactly determined.
         */

        /* Find bottom most point, erroring to left side if multiple points
         * are at miny position.
         */
        j = -1;
        for (i = 0; i < 4; i++)
        {
            if (P[i].y == miny)
            {
                if (j < 0)
                    j = i;
                else if (P[i].x < P[j].x)
                    j = i;
            }
        }
        OP[0] = P[j];   /* bottom */

        /* Determine CW or CCW rotation */
        E1x = P[0].x - P[1].x;		/* E1x = P1x-P2x */
        E1y = P[0].y - P[1].y;		/* E1y = P1y-P2y */
        E2x = P[2].x - P[1].x;		/* E2x = P3x-P2x */
        E2y = P[2].y - P[1].y;		/* E2y = P3y-P2y */

        if ((E1x * E2y - E1y * E2x) < 0)
        {
            /* CLOCKWISE */
            OP[1] = P[((j + 1) % 4)];   /* left */
            OP[2] = P[((j + 3) % 4)];   /* right */
            OP[3] = P[((j + 2) % 4)];   /* top */
        }
        else
        {
            /* COUNTER-CLOCKWISE */
            OP[1] = P[((j + 3) % 4)];   /* left */
            OP[2] = P[((j + 1) % 4)];   /* right */
            OP[3] = P[((j + 2) % 4)];   /* top */
        }

        P1[0] = P2[0] = OP[0];
        P1[2] = P2[2] = OP[3];
        P1[1] = OP[1];
        P2[1] = OP[2];

        polyRgn = egl_RegionCreate();

        if (P1[1].y < P2[1].y)
        {
            localIntersectPt(P1[1].y, &P2[0], &P2[1], &PT);
            localAddRegion(polyRgn, &P1[0], &P1[1], &P2[0], &PT);
	    if (P2[1].y > P1[2].y)
	    {
		localIntersectPt(P1[2].y, &P2[0], &P2[1], &PT2);
		localAddRegion(polyRgn, &P1[1], &P1[2], &PT, &PT2);
		localAddRegion(polyRgn, &P1[2], &P2[1], &PT2, &P2[1]);
	    }
	    else
	    {
		localIntersectPt(P2[1].y, &P1[1], &P1[2], &PT2);
		localAddRegion(polyRgn, &P1[1], &PT2, &PT, &P2[1]);
		localAddRegion(polyRgn, &PT2,  &P1[2], &P2[1], &P2[2]);
	    }
        }
        else
        {
            localIntersectPt(P2[1].y, &P1[0], &P1[1], &PT);
            localAddRegion(polyRgn, &P1[0], &PT, &P2[0], &P2[1]);
	    if (P1[1].y > P2[2].y)
	    {
		localIntersectPt(P2[2].y, &P1[0], &P1[1], &PT2);
		localAddRegion(polyRgn, &PT, &PT2, &P2[1], &P2[2]);
		localAddRegion(polyRgn, &PT2, &P1[1], &P2[2], &P1[1]);
	    }else
	    {
		localIntersectPt(P1[1].y, &P2[1], &P2[2], &PT2);
		localAddRegion(polyRgn, &PT, &P1[1], &P2[1], &PT2);
		localAddRegion(polyRgn, &P1[1], &P1[2], &PT2, &P2[2]);
	    }
        }
    }
    return polyRgn;
}
#endif  /* NOT EGL_HAS_POLYREGIONCREATE >= 0 */


/*--- altia_clip_rotate() -------------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated rasters, stencils, or text.  It
 * creates a clip region to bound the transformed raster, stencil, or text
 * and sets the graphics context to use the clip region for upcoming drawing
 * operations.  If the target graphics library supports the creation of a
 * single polygon region, this function's job is easy.  Otherwise, the
 * target graphics library must at least support merging of rectangle clip
 * regions.  In such cases, this function, along with other utility functions
 * in this file, will approximate a polygon clip region by merging multiple
 * rectangular regions.  This function may need some changes between targets.
 * For example, the calls to merge a clip region with the clip rectangle
 * may be target specific.
 *-------------------------------------------------------------------------*/
void altia_clip_rotate(Altia_GraphicState_type *gs, Altia_Extent_type *wex,
                       ALTIA_UINT32 dc, Altia_Transform_type *trans,
                       ALTIA_COORD x, ALTIA_COORD y,
                       int width, int height)
{

#if EGL_HAS_POLYREGIONCREATE
    EGL_POS poly[8];
    EGL_POLYREGION_ID polyRgn;
    int maxy;
    int maxx;
#else
    LOCAL_ALTIA_PT poly[4];
    EGL_REGION_ID polyRgn;
#endif

    ALTIA_FLOAT xArray[4], yArray[4];
    int i;

    xArray[0] = ALTIA_I2F(x);         yArray[0] = ALTIA_I2F(y);
    xArray[1] = ALTIA_I2F(x + width); yArray[1] = ALTIA_I2F(y);
    xArray[2] = ALTIA_I2F(x + width); yArray[2] = ALTIA_I2F(y + height);
    xArray[3] = ALTIA_I2F(x);         yArray[3] = ALTIA_I2F(y + height);

    for (i = 0; i < 4; i++)
    {
	altiaLibTransformF(trans,
	                   xArray[i], yArray[i], &xArray[i], &yArray[i]);

	/* On this target, the top of the screen is the origin
	 * (whereas Altia's origin is the bottom) and a clip rectangle
	 * is inclusive of its points so y is (wex->y1 - (int) yArray[i]).
	 */

#if EGL_HAS_POLYREGIONCREATE
	poly[i * 2] = (EGL_POS) ALTIA_ROUND(xArray[i]);
	poly[(i * 2) + 1] = wex->y1 - (EGL_POS) ALTIA_ROUND(yArray[i]);
#else
	poly[i].x = ALTIA_ROUND(xArray[i]);
	poly[i].y = wex->y1 -  ALTIA_ROUND(yArray[i]);
#endif

    }

#if EGL_HAS_POLYREGIONCREATE
    /* Figure out the maxy and add one */
    maxy = ALTIA_MAX(poly[1], ALTIA_MAX(poly[3],
                               ALTIA_MAX(poly[5], poly[7])));
    maxx = ALTIA_MAX(poly[0], ALTIA_MAX(poly[2],
                               ALTIA_MAX(poly[4], poly[6])));
    for (i = 1; i < 8; i+=2)
    {
	if (poly[i] == maxy)
	    poly[i] += 1;
    }
    for (i = 0; i < 8; i+=2)
    {
	if (poly[i] == maxx)
	    poly[i] += 1;
    }
    /* This target supports polygon clip regions so it is easy for
     * us to define the clip region for the scaled/rotated/stretched image.
     */
    polyRgn = egl_PolyRegionCreate();
    egl_PolyRegionInclude(polyRgn, 4, poly);

    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    {
	/* Region should be intersection of clip rectangle
	 * and rotated region.
	 */
	EGL_RECT rect;

	/* On this target, the top of the screen is the origin
	 * (whereas Altia's origin is the bottom) and a clip rectangle
	 * is inclusive of its points so a y coord is (wex->y1 - y).
	 */
	rect.left = gs->clip.x0;
	rect.top = wex->y1 - gs->clip.y1;
	rect.right = gs->clip.x1,
	rect.bottom = wex->y1 - gs->clip.y0;

	/* Intersect the polygon region to the clip rectangle. */
	egl_PolyRegionIntersectRect(polyRgn, &rect);
    }

    /* Set the GC to use the polygon region for clipping. */
    egl_PolyRegionSet((EGL_GC_ID) dc, polyRgn);

#else /* NO EGL_HAS_POLYREGIONCREATE */
    /* This target only supports a set of rectangle clip regions
     * so we must approximate a polygon region with local code.
     */
    polyRgn = localCreateMyPolygonRgn(poly);

    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    {
	/* Region should be intersection of clip rectangle
	 * and rotated region.
	 */
	EGL_RECT rect;

	/* On this target, the top of the screen is the origin
	 * (whereas Altia's origin is the bottom) and a clip rectangle
	 * is inclusive of its points so a y coord is (wex->y1 - y).
	 */
	rect.left = gs->clip.x0;
	rect.top = wex->y1 - gs->clip.y1;
	rect.right = gs->clip.x1,
	rect.bottom = wex->y1 - gs->clip.y0;

	/* Clip the region to the rectangle. */
	egl_RegionClipToRect(polyRgn, &rect);
    }

    /* Set the GC to use the rectangle regions for clipping. */
    egl_ClipRegionSet((EGL_GC_ID) dc, polyRgn);

#endif /* EGL_HAS_POLYREGIONCREATE */

}


/*--- altia_clip_clear() --------------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated rasters, stencils, or text.  It
 * clears out the clip region previously set by altia_clip_rotate().
 * It may need some changes between targets.  For example, the
 * calls to clear the clip region may be target specific.
 *-------------------------------------------------------------------------*/
void altia_clip_clear(ALTIA_UINT32 dc)
{

#if EGL_HAS_POLYREGIONCREATE
    /* This target supports polygon clip regions and now it is time
     * to clear the current region.
     */
    EGL_POLYREGION_ID polyRgn = NULL;

    egl_PolyRegionGet((EGL_GC_ID) dc, &polyRgn);
    egl_PolyRegionSet((EGL_GC_ID) dc, NULL);
    if (polyRgn != NULL)
	egl_PolyRegionDestroy(polyRgn);

#else /* NO EGL_HAS_POLYREGIONCREATE */
    /* This target supports rectangle clip regions and now it is time
     * to clear the current region.
     */

    /* Trb altia_gl deletes the region when the clip region
     * is set to NULL
     * egl_ClipRegionGet((EGL_GC_ID) dc, &rectRgn);
     */
    egl_ClipRegionSet((EGL_GC_ID) dc, NULL);
    /* if (rectRgn != NULL)
     * 	egl_RegionDestroy(rectRgn);
     */

#endif /* NO EGL_HAS_POLYREGIONCREATE */

}


/*--- altia_get_dest_bounds() ---------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated rasters, stencils, or text.  It
 * gets the position and dimensions for the destination of the rendering
 * operation.  This function may need some minor changes between targets.
 * For example, if a target's screen origin is at the top of the screen,
 * the code must adjust Altia y coordinates because they use the bottom as
 * the origin.
 *-------------------------------------------------------------------------*/
void altia_get_dest_bounds(Altia_GraphicState_type *gs,
                           Altia_Transform_type *trans, Altia_Extent_type *wex,
                           ALTIA_COORD x, ALTIA_COORD y,
                           int width, int height,
                           int *dx, int *dy,
                           int *destWidth, int *destHeight, int *yoff,
                           ALTIA_BOOLEAN *clipped,
                           int *totalWidth, int *totalHeight)
{
    /* To get the true extent of the destination we need to transform
     * both the last pixel (which is width or height -1) and the
     * the next pixel which without the -1.  We then use where the points
     * fall to know if to subtract or add to get to the edge.
     */
    ALTIA_COORD x1 = x;
    ALTIA_COORD y1 = y;
    ALTIA_COORD x2 = x;
    ALTIA_COORD y2 = y + height;
    ALTIA_COORD x3 = x + width;
    ALTIA_COORD y3 = y + height;
    ALTIA_COORD x4 = x + width;
    ALTIA_COORD y4 = y;
    int dxmin;
    int dxmin2;
    int dxmax;
    int dymin;
    int dymin2;
    int dymax;
    int temp;
    int savex;
    int savey;
    int savemax;
    int left;
    int bottom;
    int right;
    int top;

    altiaLibTransform(trans, x1, y1, &x1, &y1);
    altiaLibTransform(trans, x2, y2, &x2, &y2);
    altiaLibTransform(trans, x3, y3, &x3, &y3);
    altiaLibTransform(trans, x4, y4, &x4, &y4);

    dxmin = ALTIA_MIN(x1,ALTIA_MIN(x2,ALTIA_MIN(x3,x4)));
    dxmax = ALTIA_MAX(x1,ALTIA_MAX(x2,ALTIA_MAX(x3,x4)));
    dymin = ALTIA_MIN(y1,ALTIA_MIN(y2,ALTIA_MIN(y3,y4)));
    dymax = ALTIA_MAX(y1,ALTIA_MAX(y2,ALTIA_MAX(y3,y4)));

    x1 = x;
    y1 = y;
    x2 = x;
    y2 = y + height-1;
    x3 = x + width-1;
    y3 = y + height-1;
    x4 = x + width-1;
    y4 = y;

    altiaLibTransform(trans, x1, y1, &x1, &y1);
    altiaLibTransform(trans, x2, y2, &x2, &y2);
    altiaLibTransform(trans, x3, y3, &x3, &y3);
    altiaLibTransform(trans, x4, y4, &x4, &y4);

    dxmin2 = ALTIA_MIN(x1,ALTIA_MIN(x2,ALTIA_MIN(x3,x4)));
    dymin2 = ALTIA_MIN(y1,ALTIA_MIN(y2,ALTIA_MIN(y3,y4)));

    if (dxmin < dxmin2)
        dxmin++;
    else
        dxmax--;
    if (dymin < dymin2)
        dymin++;
    else
        dymax--;

    temp = dymax;

    /* On this target, the top of the screen is the origin
     * (whereas Altia's origin is the bottom) so a target
     * y coord is (wex->y1 - y).
     */
    dymax = wex->y1 - dymin;
    dymin = wex->y1 - temp;

    savex = dxmin;
    savey = dymin;
    savemax = dymax;

    *totalWidth = dxmax - dxmin + 1;
    *totalHeight = dymax - dymin + 1;

    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    {
	/* On this target, the top of the screen is the origin
	 * (whereas Altia's origin is the bottom) and the clip
	 * rect is inclusive of the rectangle's coordinates so
	 * a target y coord is (wex->y1 - y).
	 */
	left = gs->clip.x0;
	bottom = wex->y1 - gs->clip.y1-1;
	/* TRB we need to make clip 1 pixel bigger for scaled roundoff */
	/*right = gs->clip.x1 + 1;*/
	right = gs->clip.x1;
	/* make it one bigger here as well */
	top = wex->y1 - gs->clip.y0 + 1;
    }
    else
    {
	/* No clip so clip to the window.  On this target, clip
	 * rect is inclusive of the rectangle's coordinates.
	 */
	left = 0;
	right = wex->x1;
	bottom = 0;
	top = wex->y1;
    }

    *clipped = false;

    if (dxmin < left)
    {
	*clipped = true;
	dxmin = left;
    }

    if (dxmax > right)
    {
	*clipped = true;
	dxmax = right;
    }

    if (dymin < bottom)
    {
	*clipped = true;
	dymin = bottom;
    }

    if (dymax > top)
    {
	*clipped = true;
	dymax = top;
    }

    *destWidth = dxmax - dxmin + 1;
    *destHeight = dymax - dymin + 1;

    /* dx, dy are offsets into the destination to start */
    *dx = dxmin - savex;
    *yoff = dymin - savey;
    *dy = savemax - dymax;
}

/****************************************************************************
 ** Macros to improve the performance of the scale/stretch/rotate/distort
 ** functions.
 ****************************************************************************/





#define ALTIA_DECLARE_TRANS_DATA \
    int ychange; \
    ALTIA_FIXPT lastYVal1; \
    ALTIA_FIXPT lastYVal2;

#define ALTIA_SETUP_TRANS_DATA(inv)

#define ALTIA_TRANSFORM_TRANS_DATA(inv, x, y, sx, sy) \
    if (ychange) \
    { \
	lastYVal1 = ALT_MULFX((y), inv.a10); \
	lastYVal2 = ALT_MULFX((y), inv.a11); \
	ychange = 0; \
    } \
    sx = ALT_MULFX((x), inv.a00); \
    sx += lastYVal1 + inv.a20; \
    sy = ALT_MULFX((x), inv.a01); \
    sy += lastYVal2 + inv.a21;





/****************************************************************************
 ** Next are the local utilities and global functions specifically
 ** required for rendering scaled/stretched/rotated rasters (i.e., color
 ** bitmaps).  These function are NOT used for rendering scaled/stretched/
 ** rotated stencils or text (i.e., monochrome bitmaps)
 ****************************************************************************/


/*--- localResampleAlphaMap() ---------------------------------------------
 *
 * Local utility function used during altiaTransformBitmap() or
 * altiaTransformRaster().  It adjusts values in an alpha map according to
 * the specified rotating/stretching/scaling.  It uses a bilinear
 * algorithm to do this.
 *
 * Note:  This function is very target specific in how it gets the
 *        data from the original DIB data area and sets data in
 *        the destination DIB data area.
 *-------------------------------------------------------------------------*/
static void localResampleAlphaMap(ALTIA_UBYTE *origPtr,
                                  int origWidth, int origHeight,
                                  int origStride,
                                  ALTIA_UBYTE *destPtr, int destStride,
                                  int dx, int dy,
                                  int destWidth, int destHeight,
                                  Altia_Transform_type *trans)
{
    /* We're using an optimized bilinear filter algorithm which
    * looks only at the alpha channel.
    */
    register int temp;
    register int x, y;
    register int i, j;
    ALTIA_FLOAT sx;
    ALTIA_FLOAT sy;
    ALTIA_UBYTE s00, s10, s01, s11;
    Altia_Transform_type inv;
    ALTIA_DECLARE_TRANS_DATA

    /* READ THIS COMMENT CAREFULLY!
    * The pixels of the source image are arranged from
    * top-left to bottom-right in the data area.
    * We are dealing with Altia coordinates which have
    * their origin at the bottom-left of the screen.  We must
    * adjust an Altia y coord to a data area y coord with an
    * expression like (origMaxY - y).
    */
    int origMaxY = origHeight - 1;
    int destMaxY = destHeight - 1;

    /* NOTE:  A call to altiaLibInitTrans() requires
     * a matching altiaLibEndTrans() before returning.
     */
    altiaLibInitTrans(&inv);
    altiaLibCopyTrans(&inv, trans);

    altiaLibInvertTrans(&inv);
    ALTIA_SETUP_TRANS_DATA(inv)

    for (j = 0; j < destHeight; j++)
    {
	ychange = 1;
        for (i = 0; i < destWidth; i++)
        {
            ALTIA_DOUBLE fx;
            ALTIA_DOUBLE fy;
            int incx;
            int incy;

	    /*
            altiaLibTransformF(&inv,
                ALTIA_I2D(i + dx), ALTIA_I2D(j + dy),
                &sx, &sy);
             */
	    fx = ALTIA_I2D(i+dx);
	    fy = ALTIA_I2D(j+dy);
            ALTIA_TRANSFORM_TRANS_DATA(inv, fx, fy, sx, sy)

            x = ALTIA_F2I(sx);
            y = ALTIA_F2I(sy);

            fx = FSUB(sx,ALTIA_I2F(x));
            fy = FSUB(sy,ALTIA_I2F(y));

            /* The bilinear filter averages four points from a 2x2 grid.
            *  If the calculated x,y (i.e. sx,sy) is ourside the integer
            *  grid then shift the grid back a pixel.
            */
            if (ALTIA_LESS(fx, ALTIA_C2D(0.0)))
            {
                x--;
                fx = FADD(ALTIA_I2F(1),fx);
            }

            if (ALTIA_LESS(fy, ALTIA_C2D(0.0)))
            {
                y--;
                fy = FADD(ALTIA_I2F(1),fy);
            }

            /* Check if our 2x2 grid is complete outside the source image.
            *  An x,y value of (-1,-1) is ok because part of the grid will
            *  still be in the source image.
            */
            if ((x >= origWidth) || (x < -1) || (y >= origHeight) || (y < -1))
            {
                temp = destStride * (destMaxY - j) + i;
                destPtr[temp] =	0;
            }
            else
            {
                /* Time to calculate the alpha value for our destination pixel.
                *  We will find the alpha value by interpolating between 4 pixels
                *  of the source image.  The for pixels form a 2x2 grid that bound
                *  the subpixel location that we want an alpha value for.  The
                *  grid looks like this:
                *
                *          x    incx
                *       \-----\-----\
                *    y  \ s00 \ s10 \
                *       \-----\-----\
                *  incy \ s01 \ s11 \
                *       \-----\-----\
                *
                */

                /* Set the coordinates of the bottom right corner of our
                *  2x2 grid.
                */
                incx = x + 1;
                incy = y + 1;

                /* Check if any points from the 2x2 grid lie outside of
                *  the source image.  If so then we need to use zeros
                *  for those values in our grid.
                */
                if (x < 0)
                {
                    s00 = 0;
                    s01 = 0;

                    if (y < 0)
                    {
                        s10 = 0;
                        temp = origStride * origMaxY;
                        s11 = origPtr[temp];
                    }
                    else
                    {
                        temp = origStride * (origMaxY - y);
                        s10 = origPtr[temp];

                        if (incy < origHeight)
                        {
                            temp = origStride * (origMaxY - incy);
                            s11 = origPtr[temp];
                        }
                        else
                            s11 = 0;
                    }
                }
                else if (y < 0)
                {
                    s00 = 0;
                    s10 = 0;

                    temp = origStride * origMaxY + x;
                    s01 = origPtr[temp];

                    if (incx < origWidth)
                    {
                        temp = origStride * origMaxY + incx;
                        s11 = origPtr[temp];
                    }
                    else
                        s11 = 0;
                }
                else
                {
                    temp = origStride * (origMaxY - y) + x;
                    s00 = origPtr[temp];
                    s11 = 0xFF;

                    if (incx < origWidth)
                    {
                        temp = origStride * (origMaxY - y) + incx;
                        s10 = origPtr[temp];
                    }
                    else
                    {
                        s10 = 0;
                        s11 = 0;
                    }

                    if (incy < origHeight)
                    {
                        temp = origStride * (origMaxY - incy) + x;
                        s01 = origPtr[temp];
                    }
                    else
                    {
                        s01 = 0;
                        s11 = 0;
                    }

                    if (s11)
                    {
                        temp = origStride * (origMaxY - incy) + incx;
                        s11 = origPtr[temp];
                    }
                }

                temp = destStride * (destMaxY - j) + i;
                destPtr[temp] =	computeBilinear(s00, s10, s01, s11, fx, fy);
            }
        }
    }
    altiaLibEndTrans(&inv);
}







/*--- localResampleBilinear() ---------------------------------------------
 *
 * Local utility function used during altiaTransformRaster().  It
 * adjusts pixel values in the bitmap to be rendered to reduce the
 * undersirable jaggedness common with rotating/stretching/scaling.
 * It uses a bilinear algorigthm to do this on the assumption that
 * there are enough colors available to make the output reasonable.
 * If there are not enough colors (for example, on a 256 color target),
 * localResampleNeighborRR() should be used instead.
 *
 * Note:  This function is very target specific in how it gets the
 *        data from the original DIB data area and sets data in
 *        the destination DIB data area.
 *-------------------------------------------------------------------------*/
static void localResampleBilinear(ALTIA_UBYTE *origPtr,
                                  int origWidth, int origHeight,
                                  ALTIA_UBYTE *destPtr, int dx, int dy,
                                  int destWidth, int destHeight,
                                  Altia_Transform_type *trans,
                                  ALTIA_UBYTE *maskPtr, ALTIA_UBYTE *alphaPtr,
                                  ALTIA_BOOLEAN isAlpha, int sxoff, int syoff,
                                  int dstartx, int dstarty)
{
    register int temp;
    register int x, y;
    register int i, j;
    ALTIA_FLOAT sx;
    ALTIA_FLOAT sy;
    ALTIA_UINT32 s00, s10, s01, s11;
    ALTIA_UBYTE da, dr, dg, db;
    Altia_Transform_type inv;
    int widthBits = BITSTOBYTES(origWidth);
    int origMaxY = origHeight - 1;
    int destMaxY = destHeight - 1;
    int origWidthBytes, destWidthBytes;
    int incx;
    int incy;
    int overxy;
    int bwzone;
    int bhzone;
    ALTIA_DECLARE_TRANS_DATA
    int dendx, dendy;
    int use_dstart = 0;
    int use_soff = 0;
    ALTIA_FLOAT fsxoff = 0;
    ALTIA_FLOAT fsyoff = 0;


    /* NOTE:  A call to altiaLibInitTrans() requires
     * a matching altiaLibEndTrans() before returning.
     */
    altiaLibInitTrans(&inv);
    altiaLibCopyTrans(&inv, trans);

    if(isAlpha)
    {
        origWidthBytes = BITSTOBYTES(origWidth * 32);
        destWidthBytes = BITSTOBYTES(destWidth * 32);
    }
    else
    {
        origWidthBytes = BITSTOBYTES(origWidth * 24);

#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
        /* Some targets always need 32-bit DIBS for full color bitmaps. */
        destWidthBytes = BITSTOBYTES(destWidth * 32);
#else
        destWidthBytes = BITSTOBYTES(destWidth * 24);
#endif
    }


    /* There could be a error between how we scale and now we clip.
     * To handle this error we want to draw a little beyond the raster with
     * pixels incase the clip shows these pixels.  If we go too far we
     * will slow the draw down by rendering pixels that will not be shown.
     * If we don't go far enough we get stray pixels on the borders of
     * the rasters.  So here we try and figure out our error range.
     * Scale factors less than one create more error.
     */
    bwzone = ((origWidth + origWidth -1) / destWidth) + 1;
    bhzone = ((origHeight + origHeight -1) / destHeight) + 1;

    /* READ THIS COMMENT CAREFULLY!
     * At this point, we are ready to get RGB values from the
     * original DIB data area to set values in the destination
     * DIB data area.  On this target, the pixels of the image
     * are arranged from top-left to bottom-right in the data
     * area.  We are dealing with Altia coordinates which have
     * their origin at the bottom-left of the screen.  We must
     * adjust an Altia y coord to a data area y coord with an
     * expression like (origMaxY - y).
     */

    altiaLibInvertTrans(&inv);
    ALTIA_SETUP_TRANS_DATA(inv)

    dendx = dstartx+destWidth;
    dendy = dstarty+destHeight;
    for (j = dstarty; j < dendy; j++)
    {
        ychange = 1;
        for (i = dstartx; i < dendx; i++)
        {
            ALTIA_DOUBLE fx;
            ALTIA_DOUBLE fy;

            fx = ALTIA_I2D(i+dx);
            fy = ALTIA_I2D(j+dy);
            ALTIA_TRANSFORM_TRANS_DATA(inv, fx, fy, sx, sy)

            if (use_soff)
            {
                sx = FSUB(sx, fsxoff);
                sy = FSUB(sy, fsyoff);
            }
            x = ALTIA_F2I(sx);
            y = ALTIA_F2I(sy);

            fx = FSUB(sx,ALTIA_I2F(x));
            fy = FSUB(sy,ALTIA_I2F(y));

            /* The bilinear filter averages four points from a 2x2 grid.
             *  If the calculated x,y (i.e. sx,sy) is outside the integer
             *  grid then shift the grid back a pixel.
             */
            if (ALTIA_LESS(fx, ALTIA_C2D(0.0)))
            {
                x--;
                fx = FADD(ALTIA_I2F(1),fx);
            }

            if (ALTIA_LESS(fy, ALTIA_C2D(0.0)))
            {
                y--;
                fy = FADD(ALTIA_I2F(1),fy);
            }

            /* Check if our 2x2 grid is completely outside the source image.
             *  A x,y value of (-1,-1) is ok because part of the grid will
             *  still be in the source image.
             */
            if ((x >= origWidth) || (x < -1) || (y >= origHeight) || (y < -1))
            {
                /* If x or y not within error zone of raster then
                 * just continue as the pixel will be clipped.
                 */
                if (x - bwzone >= origWidth)
                    continue;
                if (y - bhzone >= origHeight)
                    continue;
                if (x + bwzone < 0)
                    continue;
                if (y + bhzone < 0)
                    continue;
                /* The pixel is within our error zone so we need to draw it
                 * incase its not clipped.
                 */
                if(isAlpha)
                {
                    if (use_dstart)
                        SetPixel32(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            0, 0, 0, 0, temp);
                    else
                        SetPixel32(destPtr, destWidthBytes, i, destMaxY - j,
                            0, 0, 0, 0, temp);
                    continue;
                }
                else
                {
                    /* Lets use the closest pixel color of the raster */
                    int tx = x;
                    int ty = y;
                    if (x >= origWidth)
                        tx = origWidth-1;
                    else if (x < 0)
                        tx = 0;
                    if (y >= origHeight)
                        ty = origHeight-1;
                    else if (y < 0)
                        ty = 0;
                    GetPixel24(s00, origPtr, origWidthBytes, tx,
                        origMaxY - ty, temp);
                    db = (ALTIA_UBYTE)(s00 & 0xff);
                    dg = (ALTIA_UBYTE)((s00 >> 8) & 0xff);
                    dr = (ALTIA_UBYTE)((s00 >> 16) & 0xff);
#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
                   /* Some targets always need 32-bit DIBS for full color. */
                    if (use_dstart)
                        SetPixel32(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            0x0ff, dr, dg, db, temp);
                    else
                        SetPixel32(destPtr, destWidthBytes, i, destMaxY - j,
                            0x0ff, dr, dg, db, temp);
#else
                    if (use_dstart)
                        SetPixel24(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            dr, dg, db, temp);
                    else
                        SetPixel24(destPtr, destWidthBytes, i, destMaxY - j,
                            dr, dg, db, temp);
#endif
                }
            }
            else
            {
                /* Time to calculate the alpha value for our destination pixel.
                 *  We will find the alpha value by interpolating between 4
                 *  pixels of the source image.  The four pixels form a 2x2
                 *  grid that bound the subpixel location that we want an
                 *  alpha value for.  The grid looks like this:
                 *
                 *          x    incx
                 *       \-----\-----\
                 *    y  \ s00 \ s10 \
                 *       \-----\-----\
                 *  incy \ s01 \ s11 \
                 *       \-----\-----\
                 *
                 */

                /* Set the coordinates of the bottom right corner of our
                 *  2x2 grid.
                 */
                incx = x + 1;
                incy = y + 1;

                /* To avoid color bleed do this check:
                 * if (maskPtr == NULL	|| (GetBit(maskPtr, widthBits, x, origMaxY - incy)))
                 */

                /* Check if any points from the 2x2 grid lie outside of
                 * the source image.  If so then we need to use zeros
                 * for those values in our grid.
                 */
                if (x < 0)
                {
                    if (y < 0)
                    {
                        if (isAlpha)
                        {
                            GetPixel32(s11, origPtr, alphaPtr, origWidthBytes, 0,
                                origMaxY, temp, origWidth);

                        }
                        else
                        {
                            GetPixel24(s11, origPtr, origWidthBytes, 0,
                                origMaxY, temp);
                        }
                        s00 = s11;
                        s10 = s00;
                        s01 = s00;
                    }
                    else
                    {
                        if (isAlpha)
                        {
                            GetPixel32(s10, origPtr, alphaPtr, origWidthBytes,
                                       0, origMaxY - y, temp, origWidth);

                            if (incy >= origHeight)
                            {
                                s11 = s10;
                            }
                            else
                            {
                                if ((maskPtr == NULL)
                                    || GetBit(maskPtr, widthBits, 0,
                                              origMaxY - incy))
                                {
                                    GetPixel32(s11, origPtr, alphaPtr,
                                               origWidthBytes, 0,
                                               origMaxY - incy, temp,
                                               origWidth);
                                }
                                else
                                    s11 = s10;
                            }

                            if (maskPtr != NULL)
                            {
                                if (!GetBit(maskPtr, widthBits, 0,
                                            origMaxY - y))
                                    s10 = s11;
                            }

                            s00 = s10;
                            s01 = s11;
                        }
                        else
                        {
                            GetPixel24(s10, origPtr, origWidthBytes, 0,
                                       origMaxY - y, temp);

                            if (incy >= origHeight)
                            {
                                s11 = s10;
                            }
                            else
                            {
                                if ((maskPtr == NULL)
                                    || GetBit(maskPtr, widthBits, 0,
                                              origMaxY - incy))
                                {
                                    GetPixel24(s11, origPtr, origWidthBytes,
                                               0, origMaxY - incy, temp);
                                }
                                else
                                    s11 = s10;
                            }

                            if (maskPtr != NULL)
                            {
                                if (!GetBit(maskPtr, widthBits, 0,
                                            origMaxY - y))
                                    s10 = s11;
                            }

                            s00 = s10;
                            s01 = s11;
                        }
                    }
                }
                else if (y < 0)
                {
                    if (isAlpha)
                    {
                        GetPixel32(s01, origPtr, alphaPtr, origWidthBytes, x,
                                   origMaxY, temp, origWidth);

                        if (incx >= origWidth)
                        {
                            s11 = s01;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx, origMaxY))
                            {
                                GetPixel32(s11, origPtr, alphaPtr,
                                           origWidthBytes, incx,
                                           origMaxY, temp, origWidth);
                            }
                            else
                                s11 = s01;
                        }

                        if (maskPtr != NULL)
                        {
                            if (!GetBit(maskPtr, widthBits, x, origMaxY))
                                s01 = s11;
                        }

                        s00 = s01;
                        s10 = s11;
                    }
                    else
                    {
                        GetPixel24(s01, origPtr, origWidthBytes, x,
                                   origMaxY, temp);

                        if (incx >= origWidth)
                        {
                            s11 = s01;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx, origMaxY))
                            {
                                GetPixel24(s11, origPtr, origWidthBytes, incx,
                                           origMaxY, temp);
                            }
                            else
                                s11 = s01;
                        }

                        if (maskPtr != NULL)
                        {
                            if (!GetBit(maskPtr, widthBits, x, origMaxY))
                                s01 = s11;
                        }

                        s00 = s01;
                        s10 = s11;
                    }
                }
                else
                {
                    overxy = 0;

                    if (isAlpha)
                    {
                        GetPixel32(s00, origPtr, alphaPtr, origWidthBytes, x,
                                   origMaxY - y, temp, origWidth);

                        if (incx >= origWidth)
                        {
                            overxy = 1;
                            s10 = s00;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx,
                                          origMaxY - y))
                            {
                                GetPixel32(s10, origPtr, alphaPtr,
                                           origWidthBytes, incx,
                                           origMaxY - y, temp, origWidth);
                            }
                            else
                                s10 = s00;
                        }

                        if (incy >= origHeight)
                        {
                            overxy = 1;
                            s01 = s00;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, x,
                                          origMaxY - incy))
                            {
                                GetPixel32(s01, origPtr, alphaPtr,
                                           origWidthBytes, x,
                                           origMaxY - incy, temp, origWidth);
                            }
                            else
                                s01 = s00;
                        }

                        if (overxy)
                            s11 = s00;
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx,
                                          origMaxY - incy))
                            {
                                GetPixel32(s11, origPtr, alphaPtr,
                                           origWidthBytes, incx,
                                           origMaxY - incy, temp, origWidth);
                            }
                            else
                                s11 = s00;
                        }
                    }
                    else
                    {
                        GetPixel24(s00, origPtr, origWidthBytes, x,
                                   origMaxY - y, temp);

                        if (incx >= origWidth)
                        {
                            overxy = 1;
                            s10 = s00;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx,
                                          origMaxY - y))
                            {
                                GetPixel24(s10, origPtr,
                                           origWidthBytes, incx,
                                           origMaxY - y, temp);
                            }
                            else
                                s10 = s00;
                        }

                        if (incy >= origHeight)
                        {
                            overxy = 1;
                            s01 = s00;
                        }
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, x,
                                          origMaxY - incy))
                            {
                                GetPixel24(s01, origPtr, origWidthBytes, x,
                                           origMaxY - incy, temp);
                            }
                            else
                                s01 = s00;
                        }

                        if (overxy)
                            s11 = s00;
                        else
                        {
                            if ((maskPtr == NULL)
                                || GetBit(maskPtr, widthBits, incx,
                                          origMaxY - incy))
                            {
                                GetPixel24(s11, origPtr, origWidthBytes, incx,
                                           origMaxY - incy, temp);
                            }
                            else
                                s11 = s00;
                        }
                    }
                }

                /* Write filtered sub-pixel color to the destination image */
                if(isAlpha)
                {
                    /* Need to check for pixels that are transparent. We
                     * don't want to use their color since the user might
                     * not have defined it correctly since its a transparent
                     * pixel.  Also some altia objs could have transparent
                     * pixels that have a red color (converted from transparent
                     * masks). This also cleans up the edges of transparent
                     * rasters
                     */
                    unsigned char as00;
                    unsigned char as01;
                    unsigned char as10;
                    unsigned char as11;

                    as00 = (ALTIA_UBYTE)((s00) & 0xff);
                    as01 = (ALTIA_UBYTE)((s01) & 0xff);
                    as10 = (ALTIA_UBYTE)((s10) & 0xff);
                    as11 = (ALTIA_UBYTE)((s11) & 0xff);
                    if (as00 == 0)
                    {
                        /* primary pixel is alpha'ed out */
                        if (as01 == 0 && as10 == 0 && as11 == 0)
                        {
                            /* all pixels are transparent so keep transparent */
                            if (use_dstart)
                                SetPixel32(destPtr, destWidthBytes,
                                    i - dstartx,
                                    destMaxY - (j - dstarty),
                                    0, 0, 0, 0, temp);
                            else
                                SetPixel32(destPtr, destWidthBytes, i,
                                    destMaxY - j, 0, 0, 0, 0, temp);
                            continue;
                        }
                        else
                        {
                            if (as01 == 0)
                            {
                                if (as10 == 0)
                                {
                                    s00 = s11;
                                    s01 = s00;
                                    s10 = s00;
                                }
                                else
                                {
                                    s00 = s10;
                                    if (as11 == 0)
                                    {
                                        s01 = s00;
                                        s11 = s00;
                                    }
                                    else
                                        s01 = s11;
                                }
                            }
                            else
                            {
                                if (as10 == 0)
                                {
                                    s00 = s01;
                                    if (as11 == 0)
                                    {
                                        s11 = s00;
                                        s10 = s00;
                                    }
                                    else
                                        s10 = s11;
                                }
                                else
                                {
                                    s00 = s10;
                                    if (as11 == 0)
                                        s11 = s01;
                                }
                            }
                        }
                    }
                    else
                    {
                        /* primary pixel has a valid alpha so lets
                         * only use valid alpha's for the rest
                         */
                        if (as01 == 0)
                        {
                            if (as11 == 0)
                            {
                                s01 = s00;
                                if (as10 == 0)
                                    s11 = s01;
                                else
                                    s11 = s10;
                            }
                            else
                                s01 = s11;
                            if (as10 == 0)
                                s10 = s00;
                        }
                        else
                        {
                            if (as10 == 0)
                            {
                                s10 = s00;
                                if (as11 == 0)
                                    s11 = s01;
                            }else
                            {
                                if (as11 == 0)
                                    s11 = s01;
                            }
                        }
                    }
                    db = computeBilinear((s00 >> 24) & 0xff, (s10 >> 24) & 0xff,
                        (s01 >> 24) & 0xff, (s11 >> 24) & 0xff, fx, fy);
                    dg = computeBilinear((s00 >> 16) & 0xff, (s10 >> 16) & 0xff,
                        (s01 >> 16) & 0xff, (s11 >> 16) & 0xff, fx, fy);
                    dr = computeBilinear((s00 >> 8) & 0xff, (s10 >> 8) & 0xff,
                        (s01 >> 8) & 0xff, (s11 >> 8) & 0xff, fx, fy);
                    da = computeBilinear(as00, as10, as01, as11, fx, fy);
                    if (use_dstart)
                        SetPixel32(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            da, dr, dg, db, temp);
                    else
                        SetPixel32(destPtr, destWidthBytes, i, destMaxY - j,
                            da, dr, dg, db, temp);
                }
                else
                {
                    dr = computeBilinear((s00 >> 16) & 0xff, (s10 >> 16) & 0xff,
                        (s01 >> 16) & 0xff, (s11 >> 16) & 0xff, fx, fy);
                    dg = computeBilinear((s00 >> 8) & 0xff, (s10 >> 8) & 0xff,
                        (s01 >> 8) & 0xff, (s11 >> 8) & 0xff, fx, fy);
                    db = computeBilinear((s00) & 0xff, (s10) & 0xff,
                        (s01) & 0xff, (s11) & 0xff, fx, fy);
#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
                    /* Some targets always need 32-bit DIBS for full color. */
                    if (use_dstart)
                        SetPixel32(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            0x0ff, dr, dg, db, temp);
                    else
                        SetPixel32(destPtr, destWidthBytes, i, destMaxY - j,
                            0x0ff, dr, dg, db, temp);
#else
                    if (use_dstart)
                        SetPixel24(destPtr, destWidthBytes, i - dstartx,
                            destMaxY - (j - dstarty),
                            dr, dg, db, temp);
                    else
                        SetPixel24(destPtr, destWidthBytes, i, destMaxY - j,
                            dr, dg, db, temp);
#endif
                }
            }
        }
    }
    altiaLibEndTrans(&inv);
}


/*--- localFastGet() ------------------------------------------------------
 *
 * Local utility function used during altiaTransformRaster().
 * This is usually much faster than getting the RGB value since no color
 * lookup is required for DIB's that have color tables, just use color
 * indexes since the color tables will match.
 *
 * Note:  This function is very target specific in how it gets the
 *        data from the original DIB data area.
 *-------------------------------------------------------------------------*/
static ALTIA_UINT32 localFastGet(ALTIA_UBYTE *origPtr,
                                 int numBits, int origWidth,
                                 int x, int y)
{
    register int offset;
    int temp;
    ALTIA_UINT32 pixel = 0;
    int origWidthBytes = BITSTOBYTES(origWidth * numBits);

    if (numBits == 24)
    {
        GetPixel24(pixel, origPtr, origWidthBytes, x, y, temp);
        return pixel;
    }

    offset = (origWidthBytes * y) + ((x * numBits)/8);

    if (numBits == 8)
	pixel = (ALTIA_UINT32) origPtr[offset];
    else if (numBits == 16)
	pixel = (origPtr[offset] << 8) | origPtr[offset + 1];
    else if (numBits == 32)
    {
	pixel = *((ALTIA_UINT32 *) (&origPtr[offset]));
    }
    else if (numBits < 8)
    {
	/* We have to modify part of a byte */
	int shift = 8 - numBits - ((x * numBits) % 8);
	ALTIA_UINT32 mask = 0;
	int i;

	for (i = 0; i < numBits; i++)
	    mask = (mask << 1) | 1;

	mask = mask << shift;
	pixel = (ALTIA_UINT32) origPtr[offset];
	pixel &= mask;
	pixel = pixel >> shift;
    }

    return pixel;
}


/*--- localFastPut() ------------------------------------------------------
 *
 * Local utility function used during altiaTransformRaster().
 *
 * Note:  This function is very target specific in how it puts the
 *        data into the destination DIB data area.
 *-------------------------------------------------------------------------*/
static void localFastPut(ALTIA_UBYTE *destPtr, int numBits, int destWidth,
                         int x, int y, ALTIA_UINT32 pixel)
{
    int offset;

    /* The most common situation is 24 bits of data in the pixel,
     * 8 bits each of red, green, and then blue.
     */
    if (numBits == 24)
    {
	int temp;
	ALTIA_UBYTE r = (ALTIA_UBYTE) ((pixel >> 16) & 0x0ff);
	ALTIA_UBYTE g = (ALTIA_UBYTE) ((pixel >> 8) & 0x0ff);
	ALTIA_UBYTE b = (ALTIA_UBYTE) (pixel & 0x0ff);

	/* When the number of bits in the original data is 24,
	 * the output is always 32 bits with 8 bits each of alpha,
	 * red, green, and blue.
	 */

#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
        /* Some targets always need 32-bit DIBS for full color bitmaps. */
	SetPixel32(destPtr, BITSTOBYTES(destWidth * 32),
	             x, y, 0x0ff, r, g, b, temp);
#else
	SetPixel24(destPtr, BITSTOBYTES(destWidth * 24),
	             x, y, r, g, b, temp);
#endif

	return;
    }

    offset = (BITSTOBYTES(destWidth * numBits) * y) + ((x * numBits) / 8);

    if (numBits >= 8)
    {
	if (numBits == 8)
	    destPtr[offset] = (ALTIA_UBYTE) pixel;
	else if (numBits <= 16)
	{
	    destPtr[offset] = (ALTIA_UBYTE) ((pixel >> 8) & 0xff);
	    destPtr[offset + 1] = (ALTIA_UBYTE) (pixel & 0xff);
	}
	else
	{
	    int temp;
	    ALTIA_UBYTE a = (ALTIA_UBYTE) ((pixel >> 24) & 0x0ff);
	    ALTIA_UBYTE r = (ALTIA_UBYTE) ((pixel >> 16) & 0x0ff);
	    ALTIA_UBYTE g = (ALTIA_UBYTE) ((pixel >> 8) & 0x0ff);
	    ALTIA_UBYTE b = (ALTIA_UBYTE) (pixel & 0x0ff);

	    /* When the number of bits in the original data is > 16,
	     * the output is always 32 bits with 8 bits each of alpha,
	     * red, green, and blue.
	     */
	    SetPixel32(destPtr, BITSTOBYTES(destWidth * 32),
	                 x, y, a, r, g, b, temp);
	}
    }
    else
    {
	/* We have to modify part of a byte */
	int shift = 8 - numBits - ((x * numBits) % 8);
	ALTIA_UINT32 mask = 0;
	int i;

	for (i = 0; i < numBits; i++)
	    mask = (mask << 1) | 1;

	mask = mask << shift;

	destPtr[offset] &= ~mask;
	destPtr[offset] |= pixel << shift;
    }
}


/*--- localResampleNeighborRR() -------------------------------------------
 *
 * Local utility function used during altiaTransformRaster().  It
 * adjusts pixel values in the bitmap to be rendered to reduce the
 * undersirable jaggedness common with rotating/stretching/scaling.
 * It uses a nearest neighbor algorithm which works best when there
 * are a limited number of available colors (for example, on a 256
 * color target).
 *
 * Note:  This function is very target specific in how it gets the
 *        data from the original DIB data area and sets data in
 *        the destination DIB data area.
 *-------------------------------------------------------------------------*/
static void localResampleNeighborRR(void *origDIB,
                                    int origWidth, int origHeight, int numBits,
                                    void *destDIB, int dx, int dy,
                                    int destWidth, int destHeight,
                                    Altia_Transform_type *trans,
				    int sxoff, int syoff,
				    int dstartx, int dstarty)
{
    register int x, y;
    register int i, j;
    ALTIA_FLOAT sx;
    ALTIA_FLOAT sy;
    ALTIA_UINT32 original;
    ALTIA_UINT32 lastOriginal = 0;
    int lastX = -1;
    int lastY = -1;
    Altia_Transform_type inv;
    int bwzone = ((origWidth + origWidth - 1) / destWidth) + 1;
    int bhzone = ((origHeight + origHeight - 1) / destHeight) + 1;
    int origMaxY = origHeight - 1;
    int destMaxY = destHeight - 1;
    int palSize = altiaPaletteSize(numBits);
    ALTIA_UBYTE *origPtr = (ALTIA_UBYTE *) ((EGL_DIB *) origDIB)->pImage;
    ALTIA_UBYTE *destPtr = (ALTIA_UBYTE *) ((EGL_DIB *) destDIB)->pImage;
    ALTIA_DECLARE_TRANS_DATA
    int use_dstart = 0;
    int use_soff = 0;
    int dendx, dendy;

    if (dstartx != 0 || dstarty != 0)
        use_dstart = 1;
    if (sxoff != 0 || syoff != 0)
        use_soff = 1;



    /* NOTE:  A call to altiaLibInitTrans() requires
     * a matching altiaLibEndTrans() before returning.
     */
    altiaLibInitTrans(&inv);
    altiaLibCopyTrans(&inv, trans);

    /* We need to copy the color map to the dest if the DIB has one. */
    if (numBits <= 8)
    {
	ALTIA_UBYTE *palStart = (ALTIA_UBYTE *) ((EGL_DIB *) origDIB)->pClut;
	ALTIA_UBYTE *palEnd = origPtr;
	ALTIA_BOOLEAN badPal = true;

	if (palStart + palSize <= palEnd)
	{
	    /* We have a color palette */
	    ALTIA_UBYTE *destPalStart
	        = (ALTIA_UBYTE *) ((EGL_DIB *) destDIB)->pClut;
	    ALTIA_UBYTE *destPalEnd = destPtr;

	    if (destPalStart + palSize <= destPalEnd)
	    {
		/* We can copy the original color table to dest */
	        memcpy(destPalStart, palStart, palSize);
		badPal = false;
	    }
	}

	if (badPal)
	{
	    _altiaErrorMessage(ALT_TEXT("Bad Bitmap Palette"));
	    altiaLibEndTrans(&inv);
	    return;
	}
    }

    altiaLibInvertTrans(&inv);
    ALTIA_SETUP_TRANS_DATA(inv)

    dendx = dstartx + destWidth;
    dendy = dstarty + destHeight;

    for (j=dstarty; j < dendy; j++)
    {
       ychange = 1;
       for (i = dstartx; i < dendx; i++)
       {
	 ALTIA_DOUBLE fx;
	 ALTIA_DOUBLE fy;
	 /*
	 altiaLibTransformF(&inv, ALTIA_I2D(i + dx),
	                    ALTIA_I2D(j + dy),
	                    &sx, &sy);
         */
	 fx = ALTIA_I2D(i+dx);
	 fy = ALTIA_I2D(j+dy);
         ALTIA_TRANSFORM_TRANS_DATA(inv, fx, fy, sx, sy)

	 if (use_soff)
	 {
	     x = ALTIA_F2I(sx) - sxoff;
	     y = ALTIA_F2I(sy) - syoff;
	 }else
	 {
	     x = ALTIA_F2I(sx);
	     y = ALTIA_F2I(sy);
         }

	 if (x >= origWidth)
	 {
	    /* Lets try and catch the boarder roundoff */
	    if ((x - bwzone) >= origWidth)
		continue;
	    else
	        x = origWidth - 1;
	 }

	 if (y >= origHeight)
	 {
	    /* Lets try and catch the boarder roundoff */
	    if ((y - bhzone) >= origHeight)
		continue;
	    else
	        y = origHeight - 1;
	 }

	 if (x < 0)
	 {
	    /* Lets try and catch the boarder roundoff */
	    if ((x + bwzone) < 0)
		continue;
	    else
	        x = 0;
	 }

	 if (y < 0)
	 {
	    /* Lets try and catch the boarder roundoff */
	    if ((y + bhzone) < 0)
		continue;
	    else
	        y = 0;
	 }

	 /* READ THIS COMMENT CAREFULLY!
	  * At this point, we are ready to get RGB values from the
	  * original DIB data area to set values in the destination
	  * DIB data area.  On this target, the pixels of the image
	  * are arranged from top-left to bottom-right in the data
	  * area.  We are dealing with Altia coordinates which have
	  * their origin at the bottom-left of the screen.  For getting
	  * pixels from the original data area, we must adjust an Altia
	  * y coord to a data area y coord with an expression like
	  * (origMaxY - y).  For setting pixels in the destination data
	  * area, the expression looks like (destMaxY - y).
	  */

	 if (lastX != x || lastY != y)
	 {
	     original = localFastGet(origPtr, numBits,
	                             origWidth, x, origMaxY - y);
	     lastOriginal = original;
	     lastX = x;
	     lastY = y;
	 }
	 else
	     original = lastOriginal;

	 if (use_dstart)
	     localFastPut(destPtr, numBits, destWidth, i - dstartx,
	                  destMaxY - (j - dstarty),
	                  original);
	 else
	     localFastPut(destPtr, numBits, destWidth, i, destMaxY - j,
	                  original);
       }
    }
    altiaLibEndTrans(&inv);
}


/*--- altiaTransformRaster() ----------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated rasters.  It is NOT used for
 * rendering scaled/stretched/rotated stencils or text.  The dx and dy
 * arguments are offsets to use when the original image data is copied
 * into the destination image.
 *-------------------------------------------------------------------------*/
void altiaTransformRaster(void *origDIB, int origWidth, int origHeight,
                          void *destDIB, int dx, int dy,
                          int destWidth, int destHeight,
                          Altia_Transform_type *trans,
                          int interpType, void *maskDIB)
{

    int numBits;
    ALTIA_UBYTE *maskPtr = NULL;


    /* Immediately get pointers to the actual color bitmap data. */
    ALTIA_UBYTE *origPtr = (ALTIA_UBYTE *) ((EGL_DIB *) origDIB)->pImage;
    ALTIA_UBYTE *destPtr = (ALTIA_UBYTE *) ((EGL_DIB *) destDIB)->pImage;




    /* Check if this is an alpha map font character and use
     * bilinear on alpha only if true.
     */
    if (EGL_ALPHA8 == ((EGL_DIB *) origDIB)->colorFormat)
    {
        /* Get stride values so we properly set our bytes */
        int origStride = ((EGL_DIB *) origDIB)->stride;
        int destStride = ((EGL_DIB *) destDIB)->stride;

        localResampleAlphaMap(origPtr, origWidth, origHeight, origStride,
                              destPtr, destStride, dx, dy,
                              destWidth, destHeight, trans);

        return;
    }

    /* Set the mask pointer to the monochrome bitmap data if we have
     * a transparency mask.
     */
    if (maskDIB != NULL)
        maskPtr = (ALTIA_UBYTE *) ((EGL_MDIB *) maskDIB)->pImage;

    switch (((EGL_DIB *) origDIB)->imageFormat)
    {
        case EGL_DIRECT:
            switch (((EGL_DIB *) origDIB)->colorFormat)
            {
                case EGL_RGB888:
                    numBits = 24;
                    break;

                case EGL_ARGB8888:
                    numBits = 32;
                    break;

                default:
                    _altiaErrorMessage(
                    ALT_TEXT("Unknown Direct Color Format for Image Scaling"));
                    return;
            }
            break;

        case EGL_INDEXED_8:
            numBits = 8;
            break;

        default:
            _altiaErrorMessage(
            ALT_TEXT("Unknown Indexed Color Format for Image Scaling"));
            return;
    }

    if (numBits < 24 && interpType == Bilinear)
        interpType = NearestNeighbor;

    /* Even if the raster supports 24 bits the display might
     * not so lets check that.
     */
    if (interpType == Bilinear)
    {
        /* TRB Removed localVisualMaxColors, did not need the static
         * and had bug with 32 bit color (value became negative).
         */
        ALTIA_UINT32 planes = (altiaLibGetDevInfo())->colorDepth;
        if (planes <= 8)
        { /* Not enough colors for bilinear */
            interpType = NearestNeighbor;
        }
    }

    switch (interpType)
    {
        case NearestNeighbor:
            /* ------------------------------
             * INDEXED COLOR SUPPORT WARNING!
             * ------------------------------
             * Indexed color support (for example, 256 color mode) has
             * been tested on some targets (WindML and X11 as of 12/12/2002),
             * but maybe not others.  As a result, nearest neighbor
             * resampling is not tested on all targets.
             */
            localResampleNeighborRR(origDIB, origWidth, origHeight, numBits,
                                    destDIB, dx, dy,
                                    destWidth, destHeight, trans, 0, 0, 0, 0);
            break;

        case Bilinear:
            localResampleBilinear(origPtr, origWidth, origHeight,
                                  destPtr, dx, dy,
                                  destWidth, destHeight, trans, maskPtr, NULL,
                                  (ALTIA_BOOLEAN) (numBits == 32 ? true : false),
                                  0, 0, 0, 0);
            break;

        default:
            break;
    }
}





/****************************************************************************
 ** We finish with the local utilities and global functions that are
 ** specifically required to handle monochrome bitmap scaling/stretching/
 ** rotating.  Scaled/stretched/rotated stencils, text and even rasters
 ** need these functions.  Rasters need them because they can have
 ** transparent bits which are represented using a monochrome bitmap
 ** that needs to be scaled/stretched/rotated.
 ****************************************************************************/


/*--- localKnownTransform() -----------------------------------------------
 *
 * Local utility function used during altiaTransformBitmap().
 * If we know what the transform will do, we can take some shortcuts.
 *-------------------------------------------------------------------------*/
static void localKnownTransform(ALTIA_UBYTE *origPtr,
                                int origWidth, int origHeight,
                                ALTIA_UBYTE *destPtr, int dx, int dy,
                                int destWidth, int destHeight, int knownTrans)
{
    register int x, y;
    register int i, j;
    int origWidthBytes = BITSTOBYTES(origWidth);
    int destWidthBytes = BITSTOBYTES(destWidth);
    int origMaxY = origHeight - 1;
    int destMaxY = destHeight - 1;

    /* dx is the position to offset in x and dy is same for y.
     * Since destWidth and destHeight are shorted for clipping we only
     * add the dx and dy when computing the original location.
     */
    for (j = 0; j < destHeight; j++)
    {
        for (i = 0; i < destWidth; i++)
        {
            int tempx, tempy;
            x = i + dx;
            y = j + dy;

            /* READ THIS COMMENT CAREFULLY!
             * At this point, we are ready to get a bit from the original
             * data area to set a bit in the destination data area.  On this
             * target, the bits are arranged from top-left to bottom-right
             * in the data area.  We are dealing with Altia coordinates
             * which have their origin at the bottom-left of the screen.
             * For getting bits from the original data area, we must adjust
             * an Altia y coord to a data area y coord with an expression
             * like (origMaxY - y).  For setting bits in the destination
             * data area, the expression is like (destMaxY - y).
             */

            switch(knownTrans)
            {
                case Rot90:
                    tempx = y;
                    tempy = origMaxY - (origHeight - x - 1);
                    break;

                case Rot180:
                    tempx = origWidth - x - 1;
                    tempy = origMaxY - (origHeight - y - 1);
                    break;

                case Rot270:
                    tempx = origWidth - y - 1;
                    tempy = origMaxY - x;
                    break;

                default:
                    return;
            }

            if ((tempx < origWidth && tempy < origHeight) &&
                (tempx >= 0 && tempy >= 0)) {
                if (GetBit(origPtr, origWidthBytes, tempx, tempy)) {
                    PutBitSet(destPtr, destWidthBytes, i, destMaxY - j);
                }
            }
        }
    }
}


/*--- localResampleNeighborBB() -------------------------------------------
 *
 * Local utility function used during altiaTransformBitmap().
 * It uses a nearest neighbor algorithm to copy from bitmap to bitmap.
 * This technique is adequate when there are just on (1) and off (0)
 * values.
 *-------------------------------------------------------------------------*/
static void localResampleNeighborBB(ALTIA_UBYTE *origPtr,
                                    int origWidth, int origHeight,
                                    ALTIA_UBYTE *destPtr, int dx, int dy,
                                    int destWidth, int destHeight,
                                    Altia_Transform_type *trans)
{
    register int x, y;
    register int i, j;
    ALTIA_FLOAT sx;
    ALTIA_FLOAT sy;
    int original;
    int lastOriginal = 0;
    int lastX = -1;
    int lastY = -1;
    Altia_Transform_type inv;
    int origWidthBytes = BITSTOBYTES(origWidth);
    int destWidthBytes = BITSTOBYTES(destWidth);
    int origMaxY = origHeight - 1;
    int destMaxY = destHeight - 1;
    ALTIA_DECLARE_TRANS_DATA

    /* NOTE:  A call to altiaLibInitTrans() requires
     * a matching altiaLibEndTrans() before returning.
     */
    altiaLibInitTrans(&inv);
    altiaLibCopyTrans(&inv, trans);

    altiaLibInvertTrans(&inv);
    ALTIA_SETUP_TRANS_DATA(inv)

    for (j = 0; j < destHeight; j++)
    {
       ychange = 1;
       for (i = 0; i < destWidth; i++)
       {
	 ALTIA_DOUBLE fx;
	 ALTIA_DOUBLE fy;
	 /*
	 altiaLibTransformF(&inv, ALTIA_I2D(i + dx), ALTIA_I2D(j + dy),
	                    &sx, &sy);
          */
	 fx = ALTIA_I2D(i+dx);
	 fy = ALTIA_I2D(j+dy);
         ALTIA_TRANSFORM_TRANS_DATA(inv, fx, fy, sx, sy)

	 x = ALTIA_F2I(sx);
	 y = ALTIA_F2I(sy);

	 if (x >= origWidth)
	    continue;
	 if (y >= origHeight)
	    continue;
	 if (x < 0)
	    continue;
	 if (y < 0)
	    continue;

	 /* READ THIS COMMENT CAREFULLY!
	  * At this point, we are ready to get a bit from the original
	  * data area to set a bit in the destination data area.  On this
	  * target, the bits are arranged from top-left to bottom-right
	  * in the data area.  We are dealing with Altia coordinates
	  * which have their origin at the bottom-left of the screen.
	  * For getting bits from the original data area, we must adjust
	  * an Altia y coord to a data area y coord with an expression
	  * like (origMaxY - y).  For setting bits in the destination
	  * data area, the expression looks like (destMaxY - y).
	  */

	 if (lastX != x || lastY != y)
	 {
	     original = GetBit(origPtr, origWidthBytes, x, origMaxY - y);
	     lastOriginal = original;
	     lastX = x;
	     lastY = y;
	 }
	 else
	     original = lastOriginal;

	 if (original)
	     PutBitSet(destPtr, destWidthBytes, i, destMaxY - j);
       }
    }
    altiaLibEndTrans(&inv);
}




/*--- altiaTransformBitmap() ----------------------------------------------
 *
 * Function called from other Altia target specific code during the
 * rendering of scaled/stretched/rotated stencils or text.  Even
 * scaled/stretched/rotated rasters need this function because rasters can
 * have transparent bits which are represented using a monochrome bitmap
 * that needs to be scaled/stretched/rotated.  This function should require
 * only small changes between targets.  In particular, it may be necessary
 * to modify how pointers are set to the data areas of the original DIB and
 * destination DIB.
 *-------------------------------------------------------------------------*/
void altiaTransformBitmap(void *origDIB, int origWidth, int origHeight,
                          void *destDIB, int dx, int dy,
                          int destWidth, int destHeight,
                          Altia_Transform_type *trans, int mask)
{


    int knownTrans = NoTx;

    /* Immediately get pointers to the actual monochrome bitmap data. */
    ALTIA_UBYTE *origPtr = (ALTIA_UBYTE *) ((EGL_MDIB *) origDIB)->pImage;
    ALTIA_UBYTE *destPtr = (ALTIA_UBYTE *) ((EGL_MDIB *) destDIB)->pImage;

    /* Set all the bits to 0's so we only have to set the 1's when we
     * transform.
     */
    memset(destPtr, 0, BITSTOBYTES(destWidth) * destHeight);

    /* If the bitmap is not stretched or scaled, but instead is only
     * transformed a multiple of 90 degrees, it is going to be easier
     * to compute the transformed bits.
     */
    if (mask == 0 && !ALTIA_STRETCHED(trans))
    {


        if (((trans->a00 == ALT_I2FX(1) && trans->a11 == ALT_I2FX(1))
            ||(trans->a00 == ALT_I2FX(-1) && trans->a11 == ALT_I2FX(-1))
            ||(trans->a01 == ALT_I2FX(1) && trans->a10 == ALT_I2FX(-1))
            ||(trans->a01 == ALT_I2FX(-1) && trans->a10 == ALT_I2FX(1)))

            )
        {
            int angle = ALT_FX2I(trans->angle) % 360;
            if ((angle == 90)
                ||(angle == -270))
                knownTrans = Rot90;
            else if ((angle == 180)
                ||(angle == -180))
                knownTrans = Rot180;
            else if ((angle == 270)
                ||(angle == -90))
                knownTrans = Rot270;
        }

    }

    if (knownTrans != NoTx)
        localKnownTransform(origPtr, origWidth, origHeight,
                            destPtr, dx, dy, destWidth, destHeight,
                            knownTrans);
    else
        localResampleNeighborBB(origPtr, origWidth, origHeight,
                                destPtr, dx, dy, destWidth, destHeight,
                                trans);

}




