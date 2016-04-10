
/* $Revision: 1.13 $    $Date: 2009-07-10 18:23:02 $
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

/** FILE:  altiaLibDraw.c ***************************************************
 **
 ** This file contains routines to prepare for drawing objects and for
 ** completing the drawing of objects.  One of its most interesting
 ** routines is altiaLibCopy() which bitblits from the offscreen memory to
 ** the display if double buffering is enabled.  There are other routines
 ** that are responsible for freeing up fonts and patterns when the
 ** software is told to close its window. In a way, this file is an
 ** extension of altiaDraw.c file.  But, it contains quite a bit more
 ** target specific graphics library calls.
 **
 ** Code is conditionally generated for this file based on the objects
 ** in the design at code generation time.  This file, however, will never
 ** be totally empty because some of the routines are needed for drawing
 ** any kind of object.
 ***************************************************************************/

#include <string.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"

extern EGL_COLOR altiaLibGetColor(
#ifdef Altiafp
    ALTIA_WINDOW devinfo,
    ALTIA_COLOR pixel
#endif
);


extern AltiaDevInfo *altiaLibGetDevInfo(void);















extern void _altiaGetLabelExtent(
#ifdef Altiafp
Altia_Label_type * label
#endif
);



/*--- localResetClipRectangle() -------------------------------------------
 *
 * Local utility function to reset the clip rectangle.  On this target,
 * that means setting it to the size of the available drawing area.
 *-------------------------------------------------------------------------*/
static void localResetClipRectangle(ALTIA_WINDOW win, ALTIA_UINT32 dc)
{
    Altia_Extent_type wex;
    altiaLibGetFullWindowExtent(win, &wex);

    /* On this target, clip rect is inclusive of the rectangle's
     * coordinates.
     */
    egl_ClipRectSet((EGL_GC_ID) dc,
                    (EGL_POS) wex.x0,
                    (EGL_POS) wex.y0,
                    (EGL_POS) wex.x1,
                    (EGL_POS) wex.y1
                   );
}


/*--- altiaLibSetOutlineDC() ----------------------------------------------
 *
 * Function called from target specific Altia code to set up the clip area,
 * foreground/background colors, and line width for drawing SOLID open
 * ended lines or outlines of closed objects (like rectangles, polygons,
 * etc.).  This normally requires calls to the target graphics library.
 * Another routine in the target specific Altia code handles the set up
 * for drawing DASHED lines and outlines.
 *-------------------------------------------------------------------------*/
void altiaLibSetOutlineDC(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                          Altia_GraphicState_type *gs, ALTIA_COORD winMaxY,
                          ALTIA_UINT32 *holdBrush, ALTIA_UINT32 *holdPen)
{
    Altia_Brush_type *abrush;

    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    { /* Clipping */

	/* On this target, clip rect is inclusive of the rectangle's
	 * coordinates.
	 */
	egl_ClipRectSet((EGL_GC_ID) dc,
	                (EGL_POS) gs->clip.x0,
	                (EGL_POS) (winMaxY - gs->clip.y1),
	                (EGL_POS) gs->clip.x1,
	                (EGL_POS) (winMaxY - gs->clip.y0)
	               );
    }

    /* We are drawing an unfilled rectangle so set the fill pattern
     * to solid and then a transparent background color gives us no fill.
     */
    egl_FillPatternSet((EGL_GC_ID) dc, NULL);

	egl_AlphaChannelSet((EGL_GC_ID) dc, gs->alpha);

    /* If there is a desired line width, use it and set the foreground
     * color to the desired color for the line.  Otherwise, turn off
     * the foreground color and line width (which might happen if we
     * have an unfilled object with a "None" brush).
     */
    if ((abrush = _altiaGetBrushEntry(gs->brush)) != NULL)
    {
	int lineWidth = (abrush->lineWidth == 0) ? 1 : abrush->lineWidth;
	egl_ForegroundColorSet((EGL_GC_ID) dc,
	                       altiaLibGetColor(win, 
				  EGL_ADD_ALPHA(gs->foreground, gs->alpha)));
	egl_LineStyleSet((EGL_GC_ID) dc, EGL_LINE_STYLE_SOLID);
	egl_LineWidthSet((EGL_GC_ID) dc, lineWidth);
    }else
    {
	egl_ForegroundColorSet((EGL_GC_ID) dc, EGL_COLOR_TRANSPARENT);
    }
}



/*--- altiaLibSetTextDC() -------------------------------------------------
 *
 * Function called from target specific Altia code to set up the clip area,
 * foreground/background colors, and font for drawing text.
 * This normally requires calls to the target graphics library.
 *-------------------------------------------------------------------------*/
void altiaLibSetTextDC(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       Altia_GraphicState_type *gs, ALTIA_COORD winMaxY,
                       ALTIA_UINT32 *holdFont, ALTIA_UINT32 font)
{
    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    { /* Clipping */

	/* On this target, clip rect is inclusive of the rectangle's
	 * coordinates.
	 */
	egl_ClipRectSet((EGL_GC_ID) dc,
	                (EGL_POS) gs->clip.x0,
	                (EGL_POS) (winMaxY - gs->clip.y1),
	                (EGL_POS) gs->clip.x1,
	                (EGL_POS) (winMaxY - gs->clip.y0)
	               );
    }

    if (holdFont != NULL)
    {
	EGL_FONT_ID oldFont;

	egl_FontGet((EGL_GC_ID) dc, &oldFont);
	*holdFont = (ALTIA_UINT32) oldFont;
    }

    /* Set the font, choose a solid fill with no color in the fill,
     * set the current foreground color to the desired color for the text.
     */
    egl_FontSet((EGL_GC_ID) dc, (EGL_FONT_ID) font);
    egl_FillPatternSet((EGL_GC_ID) dc, NULL);


#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
    /*
     * Sometimes the driver layer uses the background color to treat
     * text (alpha raster) images differently.  The background color
     * doesn't have any real impact on the drawing and would only be
     * used as a flag.  Here we set it accordingly.  NOTE: we don't
     * set it to transparent so the driver must deal with this.
     */
    egl_BackgroundColorSet((EGL_GC_ID) dc, gs->background);
#else

    egl_BackgroundColorSet((EGL_GC_ID) dc, EGL_COLOR_TRANSPARENT);

#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */


    egl_ForegroundColorSet((EGL_GC_ID) dc,
                           altiaLibGetColor(win, 
	                      EGL_ADD_ALPHA(gs->foreground, gs->alpha)));
    egl_AlphaChannelSet((EGL_GC_ID) dc, gs->alpha);
}



/*--- altiaLibClearDC() ---------------------------------------------------
 *
 * Function called from target specific Altia code to reset the drawing
 * modes back to their defaults after drawing an object is completed if
 * any such action really needs to be taken.  On this target, we have
 * call ClearDC.  The next time any drawing is performed, drawing modes will
 * be changed as needed.  This applies even to resetting the clip rectangle.
 * Sometimes, a significant performance improvement can be achieved by only
 * changing the clip rectangle when it is absolutedly necessary which is
 * before drawing individual objects, before clearing screen areas with
 * altiaLibClearRect(), or before a blit with altiaLibCopy().  It is
 * assumed that before the next draw, the other modes (pattern,
 * foreground/background colors, line width and style, font, etc.) will be
 * explicitly set for the particular drawing actions to be performed.
 *-------------------------------------------------------------------------*/
void altiaLibClearDC(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                     ALTIA_UINT32 holdBrush, ALTIA_UINT32 holdPen)
{
    egl_ClearDC((EGL_GC_ID)dc);
}


#if !defined(ALTIA_NO_BACKGROUND_FILL) || (0 == ALTIA_NO_BACKGROUND_FILL)
/*--- altiaLibClearRect() -------------------------------------------------
 *
 * Function called from generic Altia code to clear an area that is
 * about to be redrawn (presumably because some object(s) changed in
 * the area).  The clear is done by drawing a filled rectangle into
 * the area in the specified color.  This normally requires calls to the
 * target graphics library.  After the generic code clears the area, it
 * typically begins drawing objects into the area from back to front.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibClearRect(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       ALTIA_COLOR color, Altia_Extent_type *extent)
{
    Altia_Extent_type wex;
    EGL_POS left, top, right, bottom;

    /* We don't want any existing clip rectangle limiting the
     * clear so reset the clip rectangle.
     */
    localResetClipRectangle(win, dc);

    altiaLibGetFullWindowExtent(win, &wex);

    /* On this target, the top of the screen is the origin
     * (whereas Altia's origin is the bottom) and clip rect is
     * inclusive of the rectangle's coordinates.
     */
    bottom = wex.y1 - extent->y0;
    top = wex.y1 - extent->y1;
    left = extent->x0;
    right = extent->x1;

    /* We want to draw a filled rectangle in the given color so set the
     * fill pattern to solid and then set the background color to the
     * given color.  Turn off the foreground color and set the line
     * width to 0 so we don't get any outline on the rectangle.
     */
    egl_FillPatternSet((EGL_GC_ID) dc, NULL);
    egl_BackgroundColorSet((EGL_GC_ID) dc, 
                            altiaLibGetColor(win, 
			         EGL_ADD_ALPHA(color, EGL_OPAQUE_ALPHA)));
    egl_ForegroundColorSet((EGL_GC_ID) dc, EGL_COLOR_TRANSPARENT);
    egl_AlphaChannelSet((EGL_GC_ID) dc, EGL_OPAQUE_ALPHA);

    egl_Rectangle(win, (EGL_GC_ID) dc, left, top, right, bottom, TRUE, TRUE);
}
#endif /* !ALTIA_NO_BACKGROUND_FILL */


/*--- altiaLibCopy() ------------------------------------------------------
 *
 * Function called from generic Altia code to bit blit an area of 
 * offscreen memory to the actual display.  Normally, all drawing is
 * done to an offscreen drawing area.  When all objects have been correctly
 * rendered for the area, the generic Altia code calls this function to
 * copy the offscreen area to the actual display.  This is called double
 * buffering and it helps minimize screen flicker as objects on the
 * display change their appearance.  Double buffering is only possible
 * if the target graphics library allows the specification of an offscreen
 * drawing area and has a function to atomically copy some portion of
 * the offscreen area to the appropriate area of the actual display.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibCopy(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                  Altia_Extent_type *extent)
{
    Altia_Extent_type wex;
    EGL_BITMAP_ID memBitmap = ((AltiaDevInfo *) win)->memBitmap;

    /* If our source bitmap is the display, then we have been
     * drawing directly to the display already (i.e., double buffering
     * was intentionally disabled or we were unable to allocate offscreen
     * memory for an offscreen drawing area) and there's no
     * extra work to do.
     */
    if (((AltiaDevInfo *)win)->drawable == ((AltiaDevInfo *)win)->display)
        return;

    /* We don't want any existing clip rectangle limiting the
     * blit so reset the clip rectangle.
     */
    localResetClipRectangle(win, dc);

    /* We need the window's extent to translate Altia's
     * bottom-left origin to the target's top-left origin.
     */
    altiaLibGetFullWindowExtent(win, &wex);

	egl_AlphaChannelSet((EGL_GC_ID)dc, (EGL_INT16)EGL_OPAQUE_ALPHA);

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
    egl_BackgroundColorSet((EGL_GC_ID) dc, 0);
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

    /* We want to blit from the offscreen bitmap to the display,
     * but only the area that was determined to be damaged.
     * On this target, the top of the screen is the origin
     * (whereas Altia's origin is the bottom) and blit area is
     * inclusive of its coordinates.
     */
    egl_BitmapBlt((EGL_GC_ID) dc,
                  memBitmap  /* source */,
                  extent->x0 /* left  */, 
		  (EGL_POS) (wex.y1 - extent->y1)  /* top    */,
                  extent->x1 /* right */,  
		  (EGL_POS) (wex.y1 - extent->y0)  /* bottom */,
                  ((AltiaDevInfo*)win)->display /* dest   */,
                  (EGL_FLOAT)ALTIA_I2F(extent->x0) /* left  */,  
		  (EGL_FLOAT)ALTIA_I2F(wex.y1 - extent->y1)  /* top    */, 0);
}



/*--- localUpdateLabels() -------------------------------------------------
 *
 * Local routine to update all label extents at initialization time
 * because the design has labels (static text, text i/o, tick labels, etc.).
 * This is done because it is unlikely that the target font extents are
 * going to match the font extents in the Altia editor when code was
 * generated.
 *-------------------------------------------------------------------------*/
static void localUpdateLabels(void)
{
    int i;
    Altia_Extent_type extent;

    for (i = 0; i < test_dobjs_count; i++)
    {
        int drawType = ALTIA_DOBJ_DRAWTYPE(i);
        if (drawType == AltiaLabelDraw || drawType == AltiaTextioDraw ||
            drawType == AltiaTickDraw || drawType == AltiaMLineTextDraw)
        {
            _altiaFindExtent(AltiaDynamicObject, (ALTIA_INDEX)i, &extent);

            _altiaUpdateParentExtent(AltiaDynamicObject, (ALTIA_INDEX)i);

        }
    }


    for (i = 0; i < test_sobjs_count; i++)
    {
        if (ALTIA_SOBJ_DRAWTYPE(i) == AltiaLabelDraw)
        {
            _altiaFindExtent(AltiaStaticObject, (ALTIA_INDEX)i, &extent);

            _altiaUpdateParentExtent(AltiaStaticObject, (ALTIA_INDEX)i);

        }
    }

}



/*--- altiaLibInitDrawLib() -----------------------------------------------
 *
 * Function called from generic Altia code to give the target specific
 * Altia code the opportunity to do any special initialization.
 * For example, it may be necessary to update text extents if the design
 * has labels (static text, text i/o, tick labels, etc.).  Or, it may
 * be necessary on some targets to initialize data structures used by the
 * target specific Altia code.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibInitDrawLib(void)
{
#ifdef ALTIA_INITIALIZE_LABEL_EXTENTS

    int i;



    /* Recompute all label extents since the font size might have changed
     * for this target compared to the font size in the Altia editor.
     */
    for (i = 0 ; i < test_labels_count; i++)
        _altiaGetLabelExtent(&test_labels[i]);







    localUpdateLabels();

#endif /* ALTIA_INITIALIZE_LABEL_EXTENTS */
}


/*--- altiaLibCloseDrawLib() ----------------------------------------------
 *
 * Function called from generic Altia code to give the target specific
 * Altia code the opportunity to do any resource or memory freeing after
 * the generic code is certain that no more drawing is going to be done.
 * Delete routines in other parts of the target specific Altia code
 * are explicitly called for raster (color bitmap) and stencil (monochrome
 * bitmap) objects so these don't need to be handled here.  What would
 * be typically handled here is the freeing of font, line style, and
 * fill pattern resources if required for this target.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibCloseDrawLib(void)
{


    /* Destroy any fonts that we created during execution and be
     * sure to set resident font ids to 0 so that we know the fonts
     * have to be recreated if the Altia code is started again.
     */
    {
	int i;
	for (i = 0; i < (int) test_fonts_count; i++)
	{
	    if (test_fonts[i].id != 0)
	    {
		egl_FontDestroy((EGL_FONT_ID) (test_fonts[i].id));
		test_fonts[i].id = 0;
	    }
	}
    }

    /* If there is any persistent data used to render scaled text,
     * free it now.
     */






    /* Destroy any pattern bitmaps that were created during execution and
     * be sure to set the resident bitmap ids to 0 so that we know the
     * bitmaps have to be recreated if the Altia code is started again.
     */
    {
	int i;
	for (i = 0; i < test_patterns_count; i++)
	{
	    if (test_patterns[i].patType == BITMAP_PATTERN
	        && test_patterns[i].id != 0)
	    {
		egl_MonoBitmapDestroy((altiaLibGetDevInfo())->devId,
		                      (EGL_MDDB_ID) (test_patterns[i].id));
		test_patterns[i].id = 0;
	    }
	}
    }




}
