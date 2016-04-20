/* $Revision: 1.114 $    $Date: 2010-10-01 18:39:34 $
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



#include <string.h>
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"

#include "altiaData.h"
#include "altiaLibDraw.h"

#include "altiaImageAccess.h"

/* For debug output... */
#ifdef DEBUG_WINDOW
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#endif
/* debug output end */

#ifdef ALTIAGL
/* For embedded target defines like ALTIA_NO_BACKGROUND_FILL
 * and EGL_HAS_RASTER_TRANSFORMATION_LIB, include the
 * embedded target header file.
 */
#include "egl_Wrapper.h"
#endif /* ALTIAGL */

/*----------------------------------------------------------------------
 * Default settings for PAD macros EGL_HAS_RASTER_TRANSFORMATION_PAD,
 * EGL_HAS_RASTER_TRANSFORMATION_SNAPSHOT_PAD, and ANTIALIAS_PAD are
 * defined below.  These paddings are for target render engines that draw
 * images (Rasters, Image objects, Snapshot objects), Stencils, and/or
 * vectors outside the calculated extents of these objects.  The calculated
 * extents are adjusted to include the padding defined by these PAD macros.
 *
 * As an alternative to setting the above PAD macros to non-zero values
 * (such as from a target egl_md.h or compiler options), consider setting
 * ALTIA_MERGE_EXTENTS_PAD to a non-zero value (from egl_md.h or a compiler
 * option).  As object extents are added to the damage extent list, they
 * will be padded on the left/right/top/bottom by the chosen non-zero value.
 * With this approach, no calculated object extents are changed.  This
 * improves the positioning of objects in a Clip for targets that need
 * padding.  Note: the damage area for every object (not just images,
 * Stencils, and/or vectors) is several pixels larger (e.g., 4 pixels wider
 * and 4 pixels taller for setting ALTIA_MERGE_EXTENTS_PAD to 2).
 *----------------------------------------------------------------------*/

#ifdef EGL_HAS_RASTER_TRANSFORMATION_LIB
    /* Specify transformation pad as 0 if not previously defined */
    #ifndef EGL_HAS_RASTER_TRANSFORMATION_PAD
        #define EGL_HAS_RASTER_TRANSFORMATION_PAD 0
    #endif
    /* And Snapshot object has its own version of the pad defaulted to 0 */
    #ifndef EGL_HAS_RASTER_TRANSFORMATION_SNAPSHOT_PAD
        #define EGL_HAS_RASTER_TRANSFORMATION_SNAPSHOT_PAD 0
    #endif
#endif

#ifdef ANTIALIAS
    /* Specify anti-alias pad if not previously defined */
    #ifndef ANTIALIAS_PAD
        #ifndef ALTIA_MERGE_EXTENTS_PAD
            /* If ALTIA_MERGE_EXTENTS_PAD not defined, default AA pad is 2 */
            #define ANTIALIAS_PAD 2
        #else
            /* If ALTIA_MERGE_EXTENTS_PAD is defined, it specifies the pad */
            #define ANTIALIAS_PAD 0
        #endif
    #endif
#endif






ALTIA_WINDOW altiaTimerWin = NULL;





























/* Used with altiaCacheOutput builtin */
ALTIA_BOOLEAN _altiaOverrideUpdate = ALTIA_FALSE;

/*----------------------------------------------------------------------*
 * Load lists for managing stencils and rasters when using the NO_PRELOAD
 * option.  This code previously existed in altiaLibStencil.c and
 * altiaLibRaster.c but was moved to draw.c.  This allows us to perform
 * duplicate raster/stencil checks as well as commonize all the
 * NO_PRELOAD logic into a single file.  (JJM 3-7-2009).
 *----------------------------------------------------------------------*/
#define ALTIA_INVALID_INDEX     -1

#ifdef NO_PRELOAD
#ifndef PRELOAD_CNT
#define PRELOAD_CNT 0
#endif /* PRELOAD_CNT */

#if PRELOAD_CNT > 0
extern ALTIA_CONST ALTIA_INT preload_raster_count;
extern ALTIA_INDEX preLoadRaster[];
static int lastLoadRaster;
#endif /* PRELOAD_CNT > 0 */
#endif /* NO_PRELOAD */




/*----------------------------------------------------------------------*
 * Forward function declarations
 *----------------------------------------------------------------------*/
static void _altiaUpdateExtents(ALTIA_WINDOW win,
                                ALTIA_UINT32 dc,
                                ALTIA_INT    copy);

/*----------------------------------------------------------------------*
 * Allow all subsystems to have access to state of API caching and the
 * need for updating.  Put these variables here to eliminate references
 * into API code if API functions are not used.  Caching state and need
 * for updating are especially useful to the DAO object to coordinate
 * its updates with API caching mode.
 *----------------------------------------------------------------------*/
#ifdef ALTIA_TASKING
int _altiaAPICaching = 1;
#else
int _altiaAPICaching = 0;
#endif
int _altiaAPINeedUpdate = 0;

#ifdef ALTIA_TASKING
/*----------------------------------------------------------------------
 * This section contains declarations for operating Altia DeepScreen in
 * a tasking execution model.  The task is run from the API using
 * altiaTaskRun(milliSeconds).  An optional time limit is specified
 * which exits the task when the limit is exceeded.
 *
 * Execution of the task is performed in altiaUtils.c
 *----------------------------------------------------------------------*/

/* Current index into object list */
static ALTIA_INT _objcnt;

/* Flag indicating if a build is in progress */
static ALTIA_BOOLEAN _objBuild = ALTIA_FALSE;
#endif /* ALTIA_TASKING */


/*----------------------------------------------------------------------*/
ALTIA_UINT32 _altiaFindFont(ALTIA_CHAR *name)
{
    ALTIA_INDEX i;
    for (i = 0; i < test_fonts_count; i++)
    {
        if (ALT_STRCMP(test_fonts[i].name, name) == 0)
            return test_fonts[i].id;
    }
    return 0;
}

/*----------------------------------------------------------------------*/
void _altiaLoadFonts(void)
{
    ALTIA_INDEX i;
    for (i = 0; i < test_fonts_count; i++)
    {
        test_fonts[i].id = altiaLibLoadFont(test_fonts[i].name);
    }
}

/*----------------------------------------------------------------------*/
static Altia_Font_type *curFontArray = NULL;
Altia_Font_type * _altiaSetFontArray(Altia_Font_type *array)
{
    Altia_Font_type *retPtr = curFontArray;
    curFontArray = array;
    if (retPtr == NULL)
        retPtr = test_fonts;
    return retPtr;
}
/*----------------------------------------------------------------------*/
ALTIA_UINT32 _altiaGetFont(ALTIA_INDEX font)
{
    if (font == -1 || font >= test_fonts_count)
        return 0;
    return test_fonts[font].id;
}



/*----------------------------------------------------------------------*/
static void _altiaLoadPatterns(void)
{
    ALTIA_INDEX i;
    for (i = 0; i < test_patterns_count; i++)
    {
         altiaLibLoadPattern(&test_patterns[i]);
    }
}

static Altia_Pattern_type *curPatternArray = NULL;

/*----------------------------------------------------------------------*/
Altia_Pattern_type *_altiaSetPatternArray(Altia_Pattern_type *array)
{
    Altia_Pattern_type *retPtr = curPatternArray;
    curPatternArray = array;
    if (retPtr == NULL)
        retPtr = test_patterns;
    return retPtr;
}

/*----------------------------------------------------------------------*/
Altia_Pattern_type *_altiaGetPatternEntry(ALTIA_INDEX pat)
{
    if (pat == -1)
        return 0;
    if (curPatternArray == NULL)
        curPatternArray = test_patterns;
    return &curPatternArray[pat];
}



/*----------------------------------------------------------------------*
 * Return true if object is currently hidden by being unmapped or in a
 * hidden card of a deck.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaIsHidden(ALTIA_INDEX obj, ALTIA_SHORT objType)
{


    ALTIA_BOOLEAN hidden = ALTIA_FALSE;
    ALTIA_BOOLEAN mapped;
    ALTIA_INDEX parent;

    if (objType == AltiaDynamicObject)
    {
        mapped = ALTIA_DOBJ_MAPPED(obj);
        parent = ALTIA_DOBJ_PARENT(obj);

    }
    else
    {

        mapped = ALTIA_SOBJ_MAPPED(obj);
        parent = ALTIA_SOBJ_PARENT(obj);


    }

    if (ALTIA_FALSE == mapped)
    {
        return ALTIA_TRUE;
    }

    /* Check parents of this object */
    while ((parent != -1) && (AltiaDisplayDraw != ALTIA_DOBJ_DRAWTYPE(parent)))
    {
        if (ALTIA_DOBJ_MAPPED(parent) == ALTIA_FALSE)
        {
            /* The parent is hidden */
            hidden = ALTIA_TRUE;
            break;
        }

        /* Validate the object is in the current child list.  It may not be
        ** if the current object is a deck and the object is in a hidden card.
        */
        if (AltiaAlphaMaskDraw != ALTIA_DOBJ_DRAWTYPE(parent))
        {
            ALTIA_INT i;
            ALTIA_BOOLEAN found = ALTIA_FALSE;
            ALTIA_INT     count = ALTIA_DOBJ_CHILDCNT(parent);
            ALTIA_INDEX   first = ALTIA_DOBJ_FIRSTCHILD(parent);

            for (i = first; i < first + count; i++)
            {
                if ((ALTIA_INDEX)test_children[i].child == obj)
                {
                    /* the child is in the parent child list */
                    found = ALTIA_TRUE;
                    break;
                }
            }
            if (!found)
            {
                hidden = ALTIA_TRUE;
                break;
            }
        }

        /* Next parent */
        obj = parent;
        objType = AltiaDynamicObject;
        parent = ALTIA_DOBJ_PARENT(parent);
    }
    return hidden;

}




/*----------------------------------------------------------------------*/
void _altiaLoadRaster(ALTIA_INDEX index)
{
    /* Don't load the raster if it's already loaded */
    if (ALTIA_RASTER_ID(index))
        return;

    /* Check that the raster is not a placeholder for an image object */
    if (ALTIA_RASTER_RPIXELS(index) != -1)
    {
        Altia_Raster_type * raster;
#ifndef ALTIA_NO_RASTER_MATCH
        ALTIA_INDEX i;
#endif


        /* Load the raster */
        raster = altiaLibRasterFromIndex(index);
        if (raster)
        {
            /* Load this raster */
            altiaLibLoadRaster(raster);
            altiaLibRasterToIndex(raster, index);

            /* Find all matching rasters and reuse the pixel data */
#ifndef ALTIA_NO_RASTER_MATCH
            /* We cannot guarantee load order when using NO_PRELOAD so we must
            ** search all objects for a match.
            */
#ifdef NO_PRELOAD
            for (i = 0; i < test_rasters_count; i++)
#else
            for (i = index+1; i < test_rasters_count; i++)
#endif
            {
                if (ALTIA_RASTER_WIDTH(i)    == ALTIA_RASTER_WIDTH(index)    &&
                    ALTIA_RASTER_HEIGHT(i)   == ALTIA_RASTER_HEIGHT(index)   &&
                    ALTIA_RASTER_RPIXELS(i)  == ALTIA_RASTER_RPIXELS(index)  &&
                    ALTIA_RASTER_GPIXELS(i)  == ALTIA_RASTER_GPIXELS(index)  &&
                    ALTIA_RASTER_BPIXELS(i)  == ALTIA_RASTER_BPIXELS(index)  &&
                    ALTIA_RASTER_APIXELS(i)  == ALTIA_RASTER_APIXELS(index)  &&
                    ALTIA_RASTER_MASK(i)     == ALTIA_RASTER_MASK(index)     &&
                    ALTIA_RASTER_COLORS(i)   == ALTIA_RASTER_COLORS(index)   &&
                    ALTIA_RASTER_RCNT(i)     == ALTIA_RASTER_RCNT(index)     &&
                    ALTIA_RASTER_GCNT(i)     == ALTIA_RASTER_GCNT(index)     &&
                    ALTIA_RASTER_BCNT(i)     == ALTIA_RASTER_BCNT(index)     &&
                    ALTIA_RASTER_ACNT(i)     == ALTIA_RASTER_ACNT(index)     &&
                    ALTIA_RASTER_MASKCNT(i)  == ALTIA_RASTER_MASKCNT(index)  &&
                    ALTIA_RASTER_COLORCNT(i) == ALTIA_RASTER_COLORCNT(index) &&
                    ALTIA_RASTER_ID(i)       == 0)
                {


                    /* Use the same bitmap data as the one we just created */
                    ALTIA_SET_RASTER_ID(i,ALTIA_RASTER_ID(index));
                    ALTIA_SET_RASTER_MASKID(i,ALTIA_RASTER_MASKID(index));

                    ALTIA_SET_RASTER_SAVEBITS(i,ALTIA_RASTER_SAVEBITS(index));
                    ALTIA_SET_RASTER_SAVEMASKBITS(i,ALTIA_RASTER_SAVEMASKBITS(index));

                }
            }
#endif /* !ALTIA_NO_RASTER_MATCH */
        }
    }
}

/*----------------------------------------------------------------------*/
static void _altiaDeleteRaster(ALTIA_INDEX index)
{
    Altia_Raster_type * raster = altiaLibRasterFromIndex(index);
    if (raster)
    {
        ALTIA_INDEX i;
        ALTIA_UINT32 id;
        ALTIA_UINT32 maskId;
        id = ALTIA_RASTER_ID(index);
        maskId = ALTIA_RASTER_MASKID(index);

        /* Delete the original raster */
        altiaLibRasterDelete(raster);
        altiaLibRasterToIndex(raster, index);

        /* Clean up other rasters using the same pixel data */
        if (id != 0)
        {
#ifdef NO_PRELOAD
            for (i = 0; i < test_rasters_count; i++)
#else
            for (i = index + 1; i < test_rasters_count; i++)
#endif
            {
                if ((ALTIA_RASTER_ID(i) == id) && (ALTIA_RASTER_MASKID(i) == maskId))
                {
                    ALTIA_SET_RASTER_MASKID(i,0);
                    ALTIA_SET_RASTER_ID(i,0);

                    /* Delete the matching raster */
                    raster = altiaLibRasterFromIndex(i);
                    if (raster)
                    {
                        altiaLibRasterDelete(raster);
                        altiaLibRasterToIndex(raster, i);
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------*/
Altia_ColorEntry_type *_altiaRasterColors(ALTIA_INDEX index)
{


    return NULL;

}



/*----------------------------------------------------------------------*/
static Altia_Brush_type *curBrushArray = NULL;
Altia_Brush_type *_altiaSetBrushArray(Altia_Brush_type *array)
{
    Altia_Brush_type *retPtr = curBrushArray;
    curBrushArray = array;
    if (retPtr == NULL)
        retPtr = test_brushes;
    return retPtr;
}


/*----------------------------------------------------------------------*/
Altia_Brush_type *_altiaGetBrushEntry(ALTIA_INDEX br)
{
    if (br == -1)
        return 0;

    if (curBrushArray == NULL)
        curBrushArray = test_brushes;
    return &curBrushArray[br];


}



/*----------------------------------------------------------------------*/
static void _altiaInitBSS(void)
{
    /* Initialization of BSS memory blocks */

    ALTIA_INDEX i;

    for (i = 0; i < test_dobjs_count; i++)
    {
        ALTIA_SET_DOBJ_CURRENTFUNC(i,-1);
#ifndef ALTIA_NO_BSS_INIT
        ALTIA_SET_DOBJ_CURRENTVALUE(i,0);
#endif /* !ALTIA_NO_BSS_INIT */
    }

#ifndef ALTIA_NO_BSS_INIT





    memset(test_rasterid, 0, test_rasters_count * sizeof(Altia_RasterID_type));







    memset(test_textiosBSS, 0, test_textios_count * sizeof(Altia_TextioBSS_type));

    memset(test_textioInputsBSS, 0, test_textioInputsBSS_count * sizeof(Altia_TextioInputBSS_type));



#endif /* !ALTIA_NO_BSS_INIT */
}
/*----------------------------------------------------------------------
 * This is called first to Initialize things like fonts, etc.
 * Below is a global flag that is set to ALTIA_TRUE during the animating
 * of "altiaInitDesign" so that other subsystems (like control)
 * are aware that we are in the process of initializing.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaInitializingDesign = ALTIA_FALSE;
ALTIA_BOOLEAN _altiaInit(ALTIA_WINDOW win)
{

#if(!defined(NO_PRELOAD) || PRELOAD_CNT > 0 || PRELOAD_STENCIL_CNT > 0)
    ALTIA_INDEX i;
#endif

    _altiaInitBSS();

    /* Initialize the Image Access Engine */
    if (ALTIA_FALSE == altiaImageInitialize())
        return ALTIA_FALSE;

    altiaTimerWin = win;


    _altiaLoadFonts();


    _altiaLoadPatterns();


#ifndef NO_PRELOAD
    /* Load all the rasters */
    for (i=0; i<test_rasters_count; i++)
        _altiaLoadRaster(i);

#else
#if PRELOAD_CNT > 0
    if (preload_raster_count > 0)
    {
        /* Initialize preload array */
        lastLoadRaster = 0;
        for (i=0; i<preload_raster_count; i++)
            preLoadRaster[i] = ALTIA_INVALID_INDEX;
    }
#endif /* PRELOAD_CNT > 0 */
#endif /* !NO_PRELOAD */






    altiaLibInitDrawLib();








    /* Set global flag to ALTIA_TRUE during the animating
     * of "altiaInitDesign" so that other subsystems (like control)
     * are aware that we are in the process of initializing.
     */
    altiaInitializingDesign = ALTIA_TRUE;
    AltiaAnimate(ALT_TEXT("altiaInitDesign"), 0);
    altiaInitializingDesign = ALTIA_FALSE;

    /* Finished initialization */
    if (ALTIA_FALSE == altiaImageLoadComplete())
        return ALTIA_FALSE;

    /* Success */
    return ALTIA_TRUE;
}

/*----------------------------------------------------------------------
 * This is called last to close things like fonts, etc.
 *----------------------------------------------------------------------*/
void _altiaClose(void)
{

    ALTIA_INDEX i;




    for (i=0; i < test_rasters_count; i++)
        _altiaDeleteRaster(i);










    altiaLibCloseDrawLib();

    /* Terminate the Image Access Engine */
    altiaImageTerminate();
}

/*----------------------------------------------------------------------
 * Merge the passed in extent into the global list of extents.  Return ALTIA_TRUE
 * If the new extent overlapped an existing extent and a merge was done.
 * NOTICE that this function users the global altiaExtentLayer if
 * layers are set.  This should be set to the layer that the extent
 * applies to.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaMergeExtents(Altia_Extent_type *extent)
{
    int i = 0;
    ALTIA_BOOLEAN res = ALTIA_FALSE;
    Altia_Extent_type * next;
    Altia_Extent_type * temp;
    ALTIA_BOOLEAN done = ALTIA_FALSE;
    ALTIA_BOOLEAN merge;

    if (test_extents_count == 0)
    {
        done = ALTIA_TRUE;
    }

    /* Bounce this extent off of all exiting extents for intersection */
    while (done == ALTIA_FALSE)
    {
        next = &test_extents[i];

        {
            if ((next->x0 <= extent->x1) && (extent->x0 <= next->x1) &&
                (next->y0 <= extent->y1) && (extent->y0 <= next->y1))
            {
                /* We have intersection */
                if ((next->x0 <= extent->x0) && (next->y0 <= extent->y0) &&
                    (next->x1 >= extent->x1) && (next->y1 >= extent->y1))
                {
                    /* extent is wholly contained within next */
                    res = ALTIA_TRUE;
                }
                else
                {
                    int j = 0;
                    int nextIdx = i;
                    next->x0 = ALTIA_MIN(extent->x0, next->x0);
                    next->x1 = ALTIA_MAX(extent->x1, next->x1);
                    next->y0 = ALTIA_MIN(extent->y0, next->y0);
                    next->y1 = ALTIA_MAX(extent->y1, next->y1);
                    merge = ALTIA_TRUE;
                    while (merge)
                    {

                        {
                            temp = &test_extents[j];
                            if ((temp != next) &&
                                (next->x0 <= temp->x1) && (temp->x0 <= next->x1) &&
                                (next->y0 <= temp->y1) && (temp->y0 <= next->y1))
                            {
                                /* We have intersection */
                                int cnt;
                                if ((next->x0 > temp->x0) || (next->y0 > temp->y0) ||
                                    (next->x1 < temp->x1) || (next->y1 < temp->y1))
                                {
                                    /* temp is not wholly contained within next */
                                    next->x0 = ALTIA_MIN(temp->x0, next->x0);
                                    next->x1 = ALTIA_MAX(temp->x1, next->x1);
                                    next->y0 = ALTIA_MIN(temp->y0, next->y0);
                                    next->y1 = ALTIA_MAX(temp->y1, next->y1);
                                }
                                if (next > temp)
                                {
                                    /* we are going to move our base to temp */
                                    /* since we are srinking the array */
                                    int tempIdx = j;
                                    temp->x0 = next->x0;
                                    temp->x1 = next->x1;
                                    temp->y0 = next->y0;
                                    temp->y1 = next->y1;
                                    next = temp;
                                    j = nextIdx;
                                    nextIdx = tempIdx;
                                }

                                /* Extent has been merged, remove it from list */
                                cnt =  test_extents_count - j - 1;
                                if (cnt > 0)
                                {
                                    memmove(&test_extents[j], &test_extents[j+1], sizeof(Altia_Extent_type) * cnt);

                                }

                                /* Reduce extent count and reset our index (-1 because we'll increment below) */
                                test_extents_count--;
                                j = -1;
                            }
                        }

                        /* Next extent in list */
                        j++;
                        if (j >= test_extents_count)
                           merge = ALTIA_FALSE;
                    }

                    /* A merge occured */
                    res = ALTIA_TRUE;
                }

                /* list has been fully processed */
                done = ALTIA_TRUE;
            }
        }

        /* Next extent in list */
        i++;
        if (i >= test_extents_count)
            done = ALTIA_TRUE;
    }

    return res;
}

/*----------------------------------------------------------------------
 * Add the extent into the global list of extents.
 *----------------------------------------------------------------------*/
static void _altiaAppendExtent(Altia_Extent_type * extent)
{
    /* Parameter guaranteed to be non-NULL */
    if (test_extents_count < test_extents_max)
    {
        test_extents[test_extents_count] = *extent;

        test_extents_count++;
    }
    else
    {


        test_extents_count = 1;
        altiaLibGetWindowExtent(altiaGetMainWin(), test_extents);

        _altiaErrorMessage(ALT_TEXT("exceeded max number of extents - performing full redraw"));
    }
}


/*----------------------------------------------------------------------
 *  Get the brush that will be used for drawing which is the highest level
 *  parents brush
 *----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetBrush(ALTIA_SHORT objType,
                           ALTIA_INDEX obj)
{
    ALTIA_INDEX tempBrush = -1;
    ALTIA_INDEX tempBrush2;
    ALTIA_INDEX parent = -1;

    if (objType == AltiaDynamicObject)
    {

        tempBrush = ALTIA_DOBJ_BRUSH(obj);
        parent = ALTIA_DOBJ_PARENT(obj);

    }
    else
    {

        tempBrush = ALTIA_SOBJ_BRUSH(obj);
        parent = ALTIA_SOBJ_PARENT(obj);

    }

    /* Check parents of this object */
    if (parent != -1)
    {
        tempBrush2 = _altiaGetBrush(AltiaDynamicObject, parent);
        if (tempBrush2 != -1)
            tempBrush = tempBrush2;
    }

    return tempBrush;
}


/*----------------------------------------------------------------------*/
void _altiaGetLabelExtent(Altia_Label_type * label)
{
    Altia_Font_type *fptr = &test_fonts[label->font];

    /* Font must be valid and label's string cannot contain a
    * newline which implies it belongs to a multi-line text object.
    * Multi-line text object extents are fixed in another loop.
    */
    if (fptr->id != 0 && ALT_STRCHR(label->name, '\n') == NULL)
    {
        int width, height, offset;

        altiaLibFontExtent(fptr, label->name, (int)ALT_STRLEN(label->name),
                           &width, &height, &offset);

        label->extent.x0 = (ALTIA_COORD)offset;
        label->extent.x1 = (ALTIA_COORD)(width + offset);
        label->extent.y0 = (ALTIA_COORD)0;
        label->extent.y1 = (ALTIA_COORD)height;
    }
}


/*----------------------------------------------------------------------
 * Get the objects extent by computing it via the object type and based upon
 * the total tranform passed in.
 *----------------------------------------------------------------------*/
static void _altiaGetObjExtent(ALTIA_SHORT drawType,
                               ALTIA_INDEX drawIndex,
                               ALTIA_SHORT objType,
                               ALTIA_INDEX obj,
                               Altia_Transform_type *total,
                               Altia_Extent_type    *extent)
{
    ALTIA_SHORT bw = 0;
    ALTIA_INDEX bIndex;

    Altia_Extent_type rect;


    Altia_Extent_type *eptr;


    bIndex = _altiaGetBrush(objType, obj);
    if (bIndex == -1)
        bw = 0;
    else
        bw = test_brushes[bIndex].lineWidth;

    /*
     * NOTICE:
     *  All objects are required to provide a floating point extent
     *  if AltiaFloatExtent is set.  Most objects will either call
     *  altiaLibRectExtent or altiaLibTransformExtent which will
     *  automatically provide a floating point extent.  If an object is
     *  added that does not end up calling one of these functions you must
     *  provide a floating point extent.
     */
    switch (drawType)
    {



        case AltiaRectDraw:
            rect.x0 = test_rects[drawIndex].x0;
            rect.y0 = test_rects[drawIndex].y0;
            rect.x1 = test_rects[drawIndex].x1;
            rect.y1 = test_rects[drawIndex].y1;
            altiaLibRectExtent(&rect, bw, total, extent);
#ifdef ANTIALIAS
#if ANTIALIAS_PAD > 0
            if (extent->x0 > 0)
                extent->x0 = ALTIA_MAX(0, (extent->x0 - ANTIALIAS_PAD));
            if (extent->y0 > 0)
                extent->y0 = ALTIA_MAX(0, (extent->y0 - ANTIALIAS_PAD));
            extent->x1 += ANTIALIAS_PAD;
            extent->y1 += ANTIALIAS_PAD;
#endif /* ANTIALIAS_PAD > 0 */
#endif /* ANTIALIAS */
            break;







        case AltiaLabelDraw:
            altiaLibTransformExtent(total,
                                    &test_labels[drawIndex].extent,
                                    extent);
            break;



        case AltiaRasterDraw:
            rect.x0 = ALTIA_RASTER_X(drawIndex);
            rect.y0 = ALTIA_RASTER_Y(drawIndex);
            rect.x1 = ALTIA_RASTER_X(drawIndex) + ALTIA_RASTER_WIDTH(drawIndex);
            rect.y1 = ALTIA_RASTER_Y(drawIndex) +ALTIA_RASTER_HEIGHT(drawIndex);
            altiaLibRectExtent(&rect, 0, total, extent);
#if EGL_HAS_RASTER_TRANSFORMATION_LIB
#if EGL_HAS_RASTER_TRANSFORMATION_PAD > 0
                if (extent->x0 > 0)
                    extent->x0 = ALTIA_MAX(0, (extent->x0 - EGL_HAS_RASTER_TRANSFORMATION_PAD));
                if (extent->y0 > 0)
                    extent->y0 = ALTIA_MAX(0, (extent->y0 - EGL_HAS_RASTER_TRANSFORMATION_PAD));
                extent->x1 += EGL_HAS_RASTER_TRANSFORMATION_PAD;
                extent->y1 += EGL_HAS_RASTER_TRANSFORMATION_PAD;
#endif /* EGL_HAS_RASTER_TRANSFORMATION_PAD > 0 */
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */
            break;














        case AltiaTextioDraw:
            bIndex = ALTIA_TEXT_LABELINDEX(drawIndex),
            eptr = &(test_labels[bIndex].extent);
            altiaLibTransformExtent(total,
                                    eptr,
                                    extent);
            break;










        default:
            /* Don't know how to draw object.  For example, this can
             * happen if code gen was done on an Image object that
             * was not showing an image because the image file was
             * missing.  For these situations, make sure the extent
             * is empty.  This is important if caller passed an
             * an uninitialized extent structure.
             *
             * This can also occur for special objects that don't
             * draw such as language and skin objects.
             */
            extent->x0 = 0;
            extent->y0 = 0;
            extent->x1 = 0;
            extent->y1 = 0;
            break;
    }
}


/*----------------------------------------------------------------------
 * Draw the children of given object in the given window.
 *----------------------------------------------------------------------*/
static void _altiaChildrenDraw(ALTIA_WINDOW   win,
                               ALTIA_UINT32   dc,
                               Altia_GraphicState_type *gs,
                               ALTIA_INDEX obj)
{
    ALTIA_SHORT drawType;
    ALTIA_INDEX drawIndex;
    int count = 0;
    ALTIA_INDEX first;
    int i;

    if (ALTIA_DOBJ_MAPPED(obj) == ALTIA_FALSE)
        return;

    count = ALTIA_DOBJ_CHILDCNT(obj);
    first = ALTIA_DOBJ_FIRSTCHILD(obj);

    {
        if (altiaLibIntersect(altiaGetCurrentExtent(obj, AltiaDynamicObject), &gs->clip) == ALTIA_FALSE)
        {
            return;
        }
    }

    drawType = ALTIA_DOBJ_DRAWTYPE(obj);
    drawIndex = ALTIA_DOBJ_DRAWINDEX(obj);

    if (gs->foreground == (ALTIA_COLOR)LONG_MINUS_ONE)
        gs->foreground = ALTIA_DOBJ_FOREGROUND(obj);

    if (gs->background == (ALTIA_COLOR)LONG_MINUS_ONE)
        gs->background = ALTIA_DOBJ_BACKGROUND(obj);

    if (gs->brush == -1)
        gs->brush = ALTIA_DOBJ_BRUSH(obj);

    if (gs->pattern == -1)
        gs->pattern = ALTIA_DOBJ_PATTERN(obj);

    if (gs->font == -1)
    {
        if ((drawType == AltiaLabelDraw) || (drawType == AltiaSoundObjDraw))
        {
            /* Get the font */
            gs->font = test_labels[drawIndex].font;
        }
    }




    /* alpha channel support */
    if (gs->alpha != -1 && ALTIA_DOBJ_ALPHA(obj) != -1)
    {
        /* merge the alphas */
        gs->alpha = gs->alpha * ALTIA_DOBJ_ALPHA(obj) / 255;
    }
    else if (ALTIA_DOBJ_ALPHA(obj) != -1)
        gs->alpha = ALTIA_DOBJ_ALPHA(obj);

    if (count > 0)
    {
        for (i = 0; i < count; i++)
        {
            ALTIA_SHORT ctype = (ALTIA_SHORT)test_children[first + i].type;
            ALTIA_INDEX child = (ALTIA_INDEX)test_children[first + i].child;
            Altia_GraphicState_type gstemp = *gs;

            /* Child index is -1 if child is popped to top. */
            if (child != -1)
                _altiaDraw(win, dc, &gstemp, ctype, child);
        }
    }
}


/*----------------------------------------------------------------------
 * Draw the object passed in using the total transformation and the object
 * coordinates in the objects draw array.
 *----------------------------------------------------------------------*/
static void _altiaObjDraw(ALTIA_WINDOW win,
                          ALTIA_UINT32 dc,
                          Altia_GraphicState_type *gs,
                          ALTIA_SHORT drawType,
                          ALTIA_INDEX drawIndex,
                          ALTIA_SHORT objType,
                          ALTIA_INDEX obj,
                          Altia_Transform_type *total)
{
    int brwidth = 0;
    Altia_Brush_type *br;
    Altia_Pattern_type *pat;


    br = _altiaGetBrushEntry(gs->brush);
    if (br != 0)
        brwidth = br->lineWidth;


    pat = _altiaGetPatternEntry(gs->pattern);

    switch (drawType)
    {



        case AltiaRectDraw:
            if (test_rects[drawIndex].filled)
            {
                if (gs->pattern != -1)
                    altiaLibRectDraw(win, dc, gs, &test_rects[drawIndex],
                                     (ALTIA_BYTE)1, total);
                if (gs->brush != -1)
                {
                    if (gs->pattern == -1)
                    {
                        altiaLibRectDraw(win, dc, gs,
                                          &test_rects[drawIndex],
                                         (ALTIA_BYTE)0, total);
                    }else
                    {
                        if ((gs->background != gs->foreground)
                            || brwidth > 0)
                            altiaLibRectDraw(win, dc, gs,
                                             &test_rects[drawIndex],
                                             (ALTIA_BYTE)0, total);
                    }
                }
            }else
            {
                if (gs->brush != -1)
                {
                    altiaLibRectDraw(win, dc, gs, &test_rects[drawIndex],
                                        (ALTIA_BYTE)0, total);

                }
            }
            break;







        case AltiaLabelDraw:
            altiaLibLabelDraw(win, dc, gs, &test_labels[drawIndex],
                                   total);
            break;



        case AltiaRasterDraw:
        {
            Altia_Raster_type * raster;
#ifdef NO_PRELOAD
            if (ALTIA_RASTER_ID(drawIndex) == 0)
            {
#if PRELOAD_CNT > 0
                if (preload_raster_count > 0)
                {
                    /* Increment index in circular load list */
                    lastLoadRaster++;
                    if (lastLoadRaster >= preload_raster_count)
                        lastLoadRaster = 0;

                    /* Delete previous raster if it exists */
                    if (preLoadRaster[lastLoadRaster] != ALTIA_INVALID_INDEX)
                        _altiaDeleteRaster(preLoadRaster[lastLoadRaster]);
                    preLoadRaster[lastLoadRaster] = drawIndex;
                }
#endif /* PRELOAD_CNT > 0 */
                /* Load new raster */
                _altiaLoadRaster(drawIndex);
            }
#endif /* NO_PRELOAD */
            raster = altiaLibRasterFromIndex(drawIndex);
            if (raster)
            {
                altiaLibRasterDraw(win, dc, gs, raster, total);
                altiaLibRasterToIndex(raster, drawIndex);
            }
        }
        break;














        case AltiaTextioDraw:
            altiaLibTextioDraw(win, dc, gs, drawIndex, obj, total);
            break;










        default:
            /*
            _altiaErrorMessage(
                   ALT_TEXT("Trying to get Extent of an invalid object"));
             */
            break;
    }
}


extern ALTIA_INT AltiaFloatExtent;
extern ALTIA_FLOAT AltiaExtentX0;
extern ALTIA_FLOAT AltiaExtentY0;
extern ALTIA_FLOAT AltiaExtentX1;
extern ALTIA_FLOAT AltiaExtentY1;
/*----------------------------------------------------------------------
 * Find the actual extent of the object and return
 * as well as returning it.
 *----------------------------------------------------------------------*/
void _altiaFindExtentFloat(ALTIA_SHORT objType,
                           ALTIA_INDEX obj, Altia_Extent_type *extent,
                           ALTIA_FLOAT *fx0, ALTIA_FLOAT *fy0,
                           ALTIA_FLOAT *fx1, ALTIA_FLOAT *fy1)
{
    ALTIA_SHORT drawType;
    ALTIA_INDEX drawIndex;
    Altia_Transform_type *total;
    Altia_Extent_type *eptr;
    int prevSet = 0;

    eptr = altiaGetCurrentExtent(obj, objType);
    if (objType == AltiaDynamicObject)
    {

        drawType = ALTIA_DOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_DOBJ_DRAWINDEX(obj);
        total = ALTIA_DOBJ_TOTAL_ADDR(obj);

    }
    else
    {

        drawType = ALTIA_SOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_SOBJ_DRAWINDEX(obj);
        total = ALTIA_SOBJ_TOTAL_ADDR(obj);


    }

    if (drawType == AltiaChildDraw)
    {
        ALTIA_FLOAT cfx0, cfy0, cfx1, cfy1;
        ALTIA_SHORT count = ALTIA_DOBJ_CHILDCNT(obj);
        ALTIA_INDEX first = ALTIA_DOBJ_FIRSTCHILD(obj);
        int i;

        if (count == 0)
        {
            eptr->x0 = 0;
            eptr->y0 = 0;
            eptr->x1 = 0;
            eptr->y1 = 0;
            if (AltiaFloatExtent)
                prevSet = 1;
            else
            {
                prevSet = 0;
                AltiaFloatExtent = 1;
            }
            altiaLibTransformExtent(total, eptr, eptr);
            if (prevSet == 0)
                AltiaFloatExtent = 0;
            *fx0 = AltiaExtentX0;
            *fy0 = AltiaExtentY0;
            *fx1 = AltiaExtentX1;
            *fy1 = AltiaExtentY1;
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                ALTIA_SHORT childType = (ALTIA_SHORT)test_children[first + i].type;
                ALTIA_INDEX childIndex = (ALTIA_INDEX)test_children[first + i].child;
                Altia_Extent_type cextent;

                /* Child index is -1 if child is popped to top. */
                if (childIndex == -1)
                    continue;
                _altiaFindExtentFloat(childType, childIndex, &cextent,
                                      &cfx0, &cfy0, &cfx1, &cfy1);
                if (i == 0)
                {
                    *fx0 = cfx0;
                    *fy0 = cfy0;
                    *fx1 = cfx1;
                    *fy1 = cfy1;
                    *eptr = cextent;
                }else
                {
                    *fx0 = ALTIA_MIN(*fx0, cfx0);
                    *fy0 = ALTIA_MIN(*fy0, cfy0);
                    *fx1 = ALTIA_MAX(*fx1, cfx1);
                    *fy1 = ALTIA_MAX(*fy1, cfy1);
                    eptr->x0 = ALTIA_MIN(eptr->x0, cextent.x0);
                    eptr->y0 = ALTIA_MIN(eptr->y0, cextent.y0);
                    eptr->x1 = ALTIA_MAX(eptr->x1, cextent.x1);
                    eptr->y1 = ALTIA_MAX(eptr->y1, cextent.y1);
                }
            }
        }
        *extent = *eptr;
    }
    else

    {
        if (AltiaFloatExtent)
            prevSet = 1;
        else
        {
            prevSet = 0;
            AltiaFloatExtent = 1;
        }
        _altiaGetObjExtent(drawType, drawIndex, objType, obj, total, extent);
        if (prevSet == 0)
            AltiaFloatExtent = 0;
        *eptr = *extent;
        *fx0 = AltiaExtentX0;
        *fy0 = AltiaExtentY0;
        *fx1 = AltiaExtentX1;
        *fy1 = AltiaExtentY1;
    }
}


/*----------------------------------------------------------------------
 * Find the actual extent of the object and save it in its extent field
 * as well as returning it.
 *----------------------------------------------------------------------*/
void _altiaFindExtent(ALTIA_SHORT objType,
                      ALTIA_INDEX obj,
                      Altia_Extent_type *extent)
{
    ALTIA_SHORT drawType;
    ALTIA_INDEX drawIndex;
    Altia_Transform_type *total;
    Altia_Extent_type *eptr;

    eptr = altiaGetCurrentExtent(obj, objType);
    if (objType == AltiaDynamicObject)
    {

        drawType = ALTIA_DOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_DOBJ_DRAWINDEX(obj);
        total = ALTIA_DOBJ_TOTAL_ADDR(obj);

    }
    else
    {

        drawType = ALTIA_SOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_SOBJ_DRAWINDEX(obj);
        total = ALTIA_SOBJ_TOTAL_ADDR(obj);


    }

    if (drawType == AltiaChildDraw)
    {
        ALTIA_SHORT count = ALTIA_DOBJ_CHILDCNT(obj);
        ALTIA_INDEX first = ALTIA_DOBJ_FIRSTCHILD(obj);
        int i;

        if (count == 0)
        {
            eptr->x0 = 0;
            eptr->y0 = 0;
            eptr->x1 = 0;
            eptr->y1 = 0;
            altiaLibTransformExtent(total, eptr, eptr);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                ALTIA_SHORT childType = (ALTIA_SHORT)test_children[first + i].type;
                ALTIA_INDEX childIndex = (ALTIA_INDEX)test_children[first + i].child;
                Altia_Extent_type cextent;
                /* Child index is -1 if child is popped to top. */
                if (childIndex == -1)
                    continue;
                _altiaFindExtent(childType, childIndex, &cextent);
                if (i == 0)
                {
                    *eptr = cextent;
                }else
                {
                    eptr->x0 = ALTIA_MIN(eptr->x0, cextent.x0);
                    eptr->y0 = ALTIA_MIN(eptr->y0, cextent.y0);
                    eptr->x1 = ALTIA_MAX(eptr->x1, cextent.x1);
                    eptr->y1 = ALTIA_MAX(eptr->y1, cextent.y1);
                }
            }
        }
        *extent = *eptr;
    }
    else

    {
        _altiaGetObjExtent(drawType, drawIndex, objType, obj, total, extent);
        *eptr = *extent;
    }
}

/*----------------------------------------------------------------------
 * Get the last saved extent of the object.
 *----------------------------------------------------------------------*/
void _altiaGetExtent(ALTIA_SHORT objType,
                     ALTIA_INDEX obj,
                     Altia_Extent_type *extent)
{
    Altia_Extent_type *eptr = altiaGetCurrentExtent(obj, objType);

    /* In the unlikely event that we did not match an object, return. */
    if (eptr == NULL)
    {
        extent->x0 = 0;
        extent->y0 = 0;
        extent->x1 = 0;
        extent->y1 = 0;
    } else
        *extent = *eptr;
}

/*----------------------------------------------------------------------
 * Get the last final extent of the object.  The final extent is
 * the extent with any parents restrictions applied. Use this to get
 * the objects extent as seen on the draw window
 *----------------------------------------------------------------------*/
static void _altiaGetFinalExtent(ALTIA_SHORT objType,
                                 ALTIA_INDEX obj,
                                 Altia_Extent_type *extent)
{
    Altia_Extent_type *eptr;
    ALTIA_INDEX parent = -1;
    Altia_Transform_type *total;

    eptr = altiaGetCurrentExtent(obj, objType);
    if (objType == AltiaDynamicObject)
    {

        parent = ALTIA_DOBJ_PARENT(obj);
        total = ALTIA_DOBJ_TOTAL_ADDR(obj);

    }
    else
    {

        parent = ALTIA_SOBJ_PARENT(obj);
        total = ALTIA_SOBJ_TOTAL_ADDR(obj);

    }

    /* In the unlikely event that we did not match an object, return. */
    if (eptr == NULL)
    {
        extent->x0 = 0;
        extent->y0 = 0;
        extent->x1 = 0;
        extent->y1 = 0;

        return;
    }

    /* We now have to check to see if any parent restricts our extent.
     * If they do then use the restricted extent.
     */
    if (parent != -1)
    {
        Altia_Extent_type lastpe = *eptr;
        ALTIA_INDEX pidx = obj;
        ALTIA_SHORT ptype = objType;

        /* Check parents of this object */
        while (parent != -1 && AltiaDisplayDraw != ALTIA_DOBJ_DRAWTYPE(parent))
        {
            if ((ALTIA_DOBJ_DRAWTYPE(parent) == AltiaClipDraw) ||
                (ALTIA_DOBJ_DRAWTYPE(parent) == AltiaAlphaMaskDraw))
            {
                /* take the intersection of all parents and self */
                altiaLibSubExtent(altiaGetCurrentExtent(parent, AltiaDynamicObject), 
                                  &lastpe, &lastpe);
            }


            else
            {
                /* verify we are in the child list if not we are a swapped out
                 * card so we don't have an extent
                 */
                register int i;
                register int first = ALTIA_DOBJ_FIRSTCHILD(parent);
                int count = ALTIA_DOBJ_CHILDCNT(parent);
                int found = 0;
                for (i = 0; i < count; i++)
                {
                    if ((ALTIA_SHORT)test_children[first + i].type == ptype &&
                        (ALTIA_INDEX)test_children[first + i].child == pidx)
                    {
                        /* we are in child list */
                        found = 1;
                        break;
                    }
                }

                if (found == 0)
                { /* we aren't in the child list of our parent so are
                   * card is not showing so we don't have an extent
                   */
                    extent->x0 = 0;
                    extent->y0 = 0;
                    extent->x1 = 0;
                    extent->y1 = 0;
                    altiaLibTransformExtent(total, extent, extent);

                    return;
                }
            }


            pidx = parent;
            ptype = AltiaDynamicObject;
            parent = ALTIA_DOBJ_PARENT(parent);
        }

        *extent = lastpe;
        return;
    }

    *extent = *eptr;
}


/*----------------------------------------------------------------------
 * Update parent extents.  This called once a childs extent has been changed.
 * It allows for the parents of an object to update their extents based
 * upon a change in one of the children.
 *----------------------------------------------------------------------*/
void _altiaUpdateParentExtent(ALTIA_SHORT objType, ALTIA_INDEX obj)
{
    if (obj >= 0)
    {
        ALTIA_INDEX parent = -1;

        if (AltiaDynamicObject == objType)
        {
            parent = ALTIA_DOBJ_PARENT(obj);
        }


        if (AltiaStaticObject == objType)
        {
            parent = ALTIA_SOBJ_PARENT(obj);
        }

        if (parent >= 0)
        {
            ALTIA_SHORT drawType = ALTIA_DOBJ_DRAWTYPE(parent);
            ALTIA_INDEX drawIndex = ALTIA_DOBJ_DRAWINDEX(parent);

            if (drawType == AltiaChildDraw)
            {
                int i;
                Altia_Extent_type extent = { 0, 0, 0, 0 };

                ALTIA_SHORT count = ALTIA_DOBJ_CHILDCNT(parent);
                ALTIA_INDEX first = ALTIA_DOBJ_FIRSTCHILD(parent);

                for (i = 0; i < count; i++)
                {
                    ALTIA_SHORT childType = (ALTIA_SHORT)test_children[first + i].type;
                    ALTIA_INDEX childIndex = (ALTIA_INDEX)test_children[first + i].child;
                    Altia_Extent_type cextent;

                    /* Child index is -1 if child is popped to top. */
                    if (childIndex == -1)
                        continue;

                    _altiaGetExtent(childType, childIndex, &cextent);
                    if (i == 0)
                        extent = cextent;
                    else
                    {
                        extent.x0 = ALTIA_MIN(extent.x0, cextent.x0);
                        extent.y0 = ALTIA_MIN(extent.y0, cextent.y0);
                        extent.x1 = ALTIA_MAX(extent.x1, cextent.x1);
                        extent.y1 = ALTIA_MAX(extent.y1, cextent.y1);
                    }
                }

                if (count > 0)
                {
                    Altia_Extent_type * eptr = altiaGetCurrentExtent(parent, AltiaDynamicObject);
                    *eptr = extent;
                }
            }

            else
            {
                _altiaGetObjExtent(drawType, drawIndex, AltiaDynamicObject,
                                   parent, ALTIA_DOBJ_TOTAL_ADDR(parent),
                                   altiaGetCurrentExtent(parent, AltiaDynamicObject));
            }

            _altiaUpdateParentExtent(AltiaDynamicObject, parent);
        }
    }
}


/*----------------------------------------------------------------------
 * Add the saved extent of this object to the list of extents, merging
 * as possible.  If the max extent count is reached then an error message
 * is generated.
 *----------------------------------------------------------------------*/
void _altiaAddExtent(ALTIA_SHORT objType,
                     ALTIA_INDEX obj)
{
    Altia_Extent_type extent;

    _altiaGetFinalExtent(objType, obj, &extent);

    if (extent.x0 == extent.x1 && extent.y0 == extent.y1)
        return;   /* No extent */

    if (_altiaIsHidden(obj, objType))
        return; /* Don't add the extent if obj is hidden */

#if defined(ALTIA_MERGE_EXTENTS_PAD) && ALTIA_MERGE_EXTENTS_PAD
    /* Temporarily pad object's extent (e.g., to account for anti-aliasing) */
    if (extent.x0 > 0)
        extent.x0 = ALTIA_MAX(0, (extent.x0 - ALTIA_MERGE_EXTENTS_PAD));
    if (extent.y0 > 0)
        extent.y0 = ALTIA_MAX(0, (extent.y0 - ALTIA_MERGE_EXTENTS_PAD));
    extent.x1 += ALTIA_MERGE_EXTENTS_PAD;
    extent.y1 += ALTIA_MERGE_EXTENTS_PAD;
#endif /* ALTIA_MERGE_EXTENTS_PAD > 0 */

    if (_altiaMergeExtents(&extent) == ALTIA_FALSE)
    {
        _altiaAppendExtent(&extent);
    }
}

/*----------------------------------------------------------------------
 * Add the this extent to the list of extents, merging
 * as possible.  If the max extent count is reached then an error message
 * is generated.
 * IMPORTANT NOTICE
 * If layers are defined then altiaExtentLayer must be set to the layer
 * that this extent applies to.  It can be set to -1 if this extent applies
 * to all layers.  Normally an objects layer must be found via altiaFindLayer
 * and altiaExtentLayer set to that layer before calling this function.
 *----------------------------------------------------------------------*/
void _altiaAddMyExtent(ALTIA_SHORT objType,
                       ALTIA_INDEX obj,
                       Altia_Extent_type *extent)
{
#if defined(ALTIA_MERGE_EXTENTS_PAD) && ALTIA_MERGE_EXTENTS_PAD
    /* Temporarily pad object's extent (e.g., to account for anti-aliasing) */
     Altia_Extent_type pad_extent = *extent;
    extent = &pad_extent;
    if (extent->x0 > 0)
        extent->x0 = ALTIA_MAX(0, (extent->x0 - ALTIA_MERGE_EXTENTS_PAD));
    if (extent->y0 > 0)
        extent->y0 = ALTIA_MAX(0, (extent->y0 - ALTIA_MERGE_EXTENTS_PAD));
    extent->x1 += ALTIA_MERGE_EXTENTS_PAD;
    extent->y1 += ALTIA_MERGE_EXTENTS_PAD;
#endif /* ALTIA_MERGE_EXTENTS_PAD > 0 */

    if (_altiaMergeExtents(extent) == ALTIA_FALSE)
    {
        _altiaAppendExtent(extent);
    }
}

/*----------------------------------------------------------------------
 * Draw the given extent in the given window.
 *----------------------------------------------------------------------*/
void _altiaDrawExtent(ALTIA_WINDOW win,
                      ALTIA_UINT32 dc,
                      Altia_Extent_type *extent)
{


    if (_altiaMergeExtents(extent) == ALTIA_FALSE)
    {
        _altiaAppendExtent(extent);
    }
#if !defined(ALTIA_NO_BACKGROUND_FILL) || (0 == ALTIA_NO_BACKGROUND_FILL)

        altiaLibClearRect(win, dc, _altiaGetBackgroundColor(), extent);
#endif /* !ALTIA_NO_BACKGROUND_FILL */
    _altiaUpdateExtents(win, dc, 0);
    test_extents_count = 0;

}

/*----------------------------------------------------------------------
 * Draw the given object in the given window.
 *----------------------------------------------------------------------*/
void _altiaDraw(ALTIA_WINDOW   win,
                ALTIA_UINT32   dc,
                Altia_GraphicState_type *gs,
                ALTIA_SHORT objType,
                ALTIA_INDEX obj)
{
    ALTIA_SHORT drawType;
    ALTIA_INDEX drawIndex;
    ALTIA_COLOR foreground;
    ALTIA_COLOR background;
    ALTIA_INDEX brush;
    ALTIA_INDEX pattern;
    ALTIA_SHORT alpha; /* alpha channel support */
    int count = 0;
    Altia_Transform_type *total;



    {
        if (altiaLibIntersect(altiaGetCurrentExtent(obj, objType), &gs->clip) == ALTIA_FALSE)
        {
            return;
        }
    }

    if (objType == AltiaDynamicObject)
    {

        if (ALTIA_DOBJ_MAPPED(obj) == ALTIA_FALSE)
            return;

        count      = ALTIA_DOBJ_CHILDCNT(obj);
        drawType   = ALTIA_DOBJ_DRAWTYPE(obj);
        drawIndex  = ALTIA_DOBJ_DRAWINDEX(obj);
        foreground = ALTIA_DOBJ_FOREGROUND(obj);
        background = ALTIA_DOBJ_BACKGROUND(obj);
        brush      = ALTIA_DOBJ_BRUSH(obj);
        pattern    = ALTIA_DOBJ_PATTERN(obj);
        total      = ALTIA_DOBJ_TOTAL_ADDR(obj);
        alpha      = ALTIA_DOBJ_ALPHA(obj);



    }
    else
    {

        if (ALTIA_SOBJ_MAPPED(obj) == ALTIA_FALSE)
            return;

        drawType   = ALTIA_SOBJ_DRAWTYPE(obj);
        drawIndex  = ALTIA_SOBJ_DRAWINDEX(obj);
        foreground = ALTIA_SOBJ_FOREGROUND(obj);
        background = ALTIA_SOBJ_BACKGROUND(obj);
        brush      = ALTIA_SOBJ_BRUSH(obj);
        pattern    = ALTIA_SOBJ_PATTERN(obj);
        total      = ALTIA_SOBJ_TOTAL_ADDR(obj);
        alpha      = ALTIA_SOBJ_ALPHA(obj);



    }

    if (AltiaChildDraw == drawType)
    {
        if (count > 0)
        {

            _altiaChildrenDraw(win, dc, gs, obj);

        }
    }

    else
    {



        if (gs->foreground == (ALTIA_COLOR)LONG_MINUS_ONE)
            gs->foreground = foreground;
        if (gs->background == (ALTIA_COLOR)LONG_MINUS_ONE)
            gs->background = background;
        if (gs->brush == -1)
            gs->brush = brush;
        if (gs->pattern == -1)
            gs->pattern = pattern;

        if (gs->font == -1)
        {
            if ((drawType == AltiaLabelDraw) ||
                    (drawType == AltiaSoundObjDraw))
            { /* Get the font */
                gs->font = test_labels[drawIndex].font;
            }
        }



        if (gs->font == -1)
        {
            if (drawType == AltiaTextioDraw)
            { /* Get the font */
                ALTIA_INDEX temp = ALTIA_TEXT_LABELINDEX(drawIndex);
                gs->font = test_labels[temp].font;
            }
        }




        if (gs->alpha != -1 && alpha != -1)
        { /* merge the alphas */
            gs->alpha = gs->alpha * alpha / 255;
        }else if (alpha != -1)
            gs->alpha = alpha;

#ifdef ALTIA_TASKING
        /* We need to check for special object that are really child draws. For
        ** these objects, the child draw happens in their respective draw libraries.  
        ** As a result the child draw will be called in the context of _altiaObjDraw 
        ** below.
        */
        if (_objBuild && (AltiaClipDraw != drawType) 
                      && (AltiaBlurFilterDraw != drawType))
        {
            /* Save the object in the object draw queue */
            if (_objcnt < altiaObjectQueue_count)
            {
                altiaObjectQueue[_objcnt].type = objType;
                altiaObjectQueue[_objcnt].idx = obj;
                altiaObjectQueue[_objcnt].gs = *gs;
                _objcnt++;
            }
        }
        else
#endif /* ALTIA_TASKING */
            _altiaObjDraw(win, dc, gs, drawType, drawIndex, objType, obj, total);


    }
}

/*----------------------------------------------------------------------
** Called exclusively from _altiaUpdateExtents() to draw the changed
** objects on the screen.  It will be called once for each:
**
** 1. Top level object (test_sequence)
** 2. Cloned objects (test_sequence)
** 3. Pop-to-Top objects (test_preSeq)
*/
static void doUpdate(ALTIA_WINDOW win,
                     ALTIA_UINT32 dc,
                     Altia_GraphicState_type *gs,
                     Altia_Extent_type *winExtent,
                     ALTIA_SHORT objType,
                     ALTIA_INDEX obj)
{
    int j;
    Altia_GraphicState_type gstemp;


    Altia_Extent_type *eptr = NULL;
    if (obj == -1)
    {
        return;
    }

    eptr = altiaGetCurrentExtent(obj, objType);

    /* In the unlikely event that we did not match an object, return. */
    if (eptr == NULL)
    {
        return;
    }

    for (j = 0; j < test_extents_count; j++)
    {
        Altia_Extent_type extent = test_extents[j];

        altiaLibSubExtent(&extent, winExtent, &extent);
        if (extent.x1 == extent.x0 && extent.y1 == extent.y0)
        {
            continue;
        }

        if ((eptr->x0 <= extent.x1) && (extent.x0 <= eptr->x1) &&
            (eptr->y0 <= extent.y1) && (extent.y0 <= eptr->y1))
        {
            /* Intersection */
            gs->clip = extent;
            gstemp = *gs;
            _altiaDraw(win, dc, &gstemp, objType, obj);
        }
    }

}

/*----------------------------------------------------------------------
 * Draw to all the extents and then clear out extents
 *----------------------------------------------------------------------*/
static void _altiaUpdateExtents(ALTIA_WINDOW win,
                                ALTIA_UINT32 dc,
                                ALTIA_INT    copy)
{
    int i;
    int j;
    Altia_GraphicState_type gs;
    Altia_Extent_type winExtent;

    altiaLibGetWindowExtent(win, &winExtent);

    gs.foreground = (ALTIA_COLOR)LONG_MINUS_ONE;
    gs.background = (ALTIA_COLOR)LONG_MINUS_ONE;
    gs.brush = -1;
    gs.pattern = -1;
    gs.font = -1;
    gs.alpha = -1; /* alpha channel support */

    for (i = 0; i < test_sequence_count; i++)
    {
        ALTIA_SHORT objType = (ALTIA_SHORT)test_sequence[i].objType;
        ALTIA_INDEX obj = (ALTIA_INDEX)test_sequence[i].object;
        doUpdate(win, dc, &gs, &winExtent, objType, obj);
    }

    if (copy != 0)
    { /* We drew to off screen bitmap and now we have to */
      /* copy what we drew to the window.  This is for double buffering. */
        for (j = 0; j < test_extents_count; j++)
        {

            {
                Altia_Extent_type extent = test_extents[j];
                altiaLibSubExtent(&extent, &winExtent, &extent);
                /* Don't copy an extent if there is none */
                if (extent.x1 == extent.x0 && extent.y1 == extent.y0)
                {
                    continue;
                }

                altiaLibCopy(win, dc, &extent);
            }
        }
    }
}

/*----------------------------------------------------------------------
 * Get the window background color
 *----------------------------------------------------------------------*/
ALTIA_COLOR _altiaGetBackgroundColor(void)
{

    return test_window_background;
}

/*----------------------------------------------------------------------
 * Get the window size
 *----------------------------------------------------------------------*/
void _altiaGetWindowSize(ALTIA_INT * width,
                         ALTIA_INT * height)
{

    *width  = test_window_width;
    *height = test_window_height;
}

/*----------------------------------------------------------------------
 * This should be called after AltiaAnimate returns a ALTIA_TRUE.  It draws all
 * the objects that extents overlap the extents changed by animation.
 * If builtins are enabled, it is possible to suppress updates by setting
 * the builtin animation altiaCacheOutput to non-zero which in turn
 * adjusts the _altiaOverrideUpdate flag.
 *----------------------------------------------------------------------*/
void AltiaUpdate(ALTIA_WINDOW win,
                 ALTIA_UINT32 dc,
                 ALTIA_INT copy)
{
    int i;
    Altia_Extent_type winExtent;

    if (_altiaOverrideUpdate)
        return;

    altiaLibGetWindowExtent(win, &winExtent);
    for (i = 0; i < test_extents_count; i++)
    {
        Altia_Extent_type extent = test_extents[i];


        altiaLibSubExtent(&extent, &winExtent, &extent);

        if (extent.x1 == extent.x0 && extent.y1 == extent.y0)
                continue;
#if !defined(ALTIA_NO_BACKGROUND_FILL) || (0 == ALTIA_NO_BACKGROUND_FILL)
        altiaLibClearRect(win, dc, _altiaGetBackgroundColor(), &extent);
#endif
    }
    _altiaUpdateExtents(win, dc, copy);

    test_extents_count = 0;
}

#ifdef ALTIA_TASKING
/*----------------------------------------------------------------------
 * This section contains functions for operating Altia DeepScreen in a
 * tasking execution model.  The task is run from the API using
 * altiaTaskRun(milliSeconds).  An optional time limit is specified
 * which exits the task when the limit is exceeded.
 *
 * Execution of the task is performed in altiaUtils.c
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
 * This function is used by tasking-based draw updates.  It prepares
 * the extent so it may be used to assemble the list of objects for
 * drawing.  Return ALTIA_FALSE if the extent is not valid.
 *----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN AltiaPrepareBuild(ALTIA_WINDOW win,
                                ALTIA_INT idx,
                                Altia_Extent_type * extent)
#else
ALTIA_BOOLEAN AltiaPrepareBuild(win, idx, extent)
ALTIA_WINDOW win;
ALTIA_INT idx;
Altia_Extent_type * extent;
#endif
{
    if (extent)
    {
        Altia_Extent_type winExtent;

        altiaLibGetWindowExtent(win, &winExtent);

        altiaLibSubExtent(&test_extents[idx], &winExtent, extent);

        /* Check if the intersection of extents is valid */
        if (extent->x1 != extent->x0 || extent->y1 != extent->y0)
            return ALTIA_TRUE;
    }

    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------
 * This function is used by tasking-based draw updates.  It assembles
 * the list of objects to be drawn for the specified extent.  The
 * data is stored in the altiaObjectQueue.
 *----------------------------------------------------------------------*/
ALTIA_INT AltiaUpdateBuild(ALTIA_WINDOW win,
                           ALTIA_UINT32 dc,
                           Altia_Extent_type * extent)
{
    /* Clear current object list count */
    _objcnt = 0;

    /* Start the build */
    _objBuild = ALTIA_TRUE;
    if (extent)
    {
        ALTIA_INT i;


        for (i = 0; i < test_sequence_count; i++)
        {
            ALTIA_INDEX obj = (ALTIA_INDEX)test_sequence[i].object;
            if (obj >= 0)
            {
                ALTIA_SHORT objType = (ALTIA_SHORT)test_sequence[i].objType;
                Altia_Extent_type *eptr = altiaGetCurrentExtent(obj, objType);
                if (NULL != eptr)
                {
                    /* Check if this object intersects with the draw extent */
                    if ((eptr->x0 <= extent->x1) && (extent->x0 <= eptr->x1) &&
                        (eptr->y0 <= extent->y1) && (extent->y0 <= eptr->y1))
                    {
                        Altia_GraphicState_type gs;

                        /* Reset the GS */
                        gs.clip       = *extent;
                        gs.foreground = (ALTIA_COLOR)LONG_MINUS_ONE;
                        gs.background = (ALTIA_COLOR)LONG_MINUS_ONE;
                        gs.brush      = -1;
                        gs.pattern    = -1;
                        gs.font       = -1;
                        gs.alpha      = -1;

                        /* Process the object */
                        _altiaDraw(win, dc, &gs, objType, obj);
                    }
                }
            }
        }


    }

    /* Done with build -- return the object count */
    _objBuild = ALTIA_FALSE;
    return _objcnt;
}

/*----------------------------------------------------------------------
 * This function is used by tasking-based draw updates.  It prepares
 * to draw to the specified extent.  This is performed right before
 * the AltiaUpdateDraw function.
 *----------------------------------------------------------------------*/
void AltiaPrepareDraw(ALTIA_WINDOW win,
                      ALTIA_UINT32 dc,
                      Altia_Extent_type * extent)
{
    if (extent)
    {
#if !defined(ALTIA_NO_BACKGROUND_FILL) || (0 == ALTIA_NO_BACKGROUND_FILL)
        altiaLibClearRect(win, dc, _altiaGetBackgroundColor(), extent);
#endif
    }
}

/*----------------------------------------------------------------------
 * This function is used by tasking-based draw updates.  It performs
 * a draw for each of the objects in the altiaObjectQueue.
 *----------------------------------------------------------------------*/
void AltiaUpdateDraw(ALTIA_WINDOW win,
                     ALTIA_UINT32 dc,
                     ALTIA_INT idx)
{
    if (idx >= 0)
    {
        ALTIA_SHORT objType = (ALTIA_SHORT)altiaObjectQueue[idx].type;
        ALTIA_INDEX obj = (ALTIA_INDEX)altiaObjectQueue[idx].idx;
        Altia_GraphicState_type *gs = &altiaObjectQueue[idx].gs;

        ALTIA_SHORT drawType;
        ALTIA_INDEX drawIndex;
        Altia_Transform_type * total = NULL;

        if (objType == AltiaDynamicObject)
        {

            drawType = ALTIA_DOBJ_DRAWTYPE(obj);
            drawIndex = ALTIA_DOBJ_DRAWINDEX(obj);
            total = ALTIA_DOBJ_TOTAL_ADDR(obj);

        }
        else
        {

            drawType = ALTIA_SOBJ_DRAWTYPE(obj);
            drawIndex = ALTIA_SOBJ_DRAWINDEX(obj);
            total = ALTIA_SOBJ_TOTAL_ADDR(obj);

        }

        /* Draw the object */
        if (total)
            _altiaObjDraw(win, dc, gs, drawType, drawIndex, objType, obj, total);
    }
}

/*----------------------------------------------------------------------
 * This function is used by tasking-based draw updates.  It completes
 * the draw operation for the specified extent.  This is performed right
 * after the AltiaUpdateDraw function.
 *----------------------------------------------------------------------*/
void AltiaFinishDraw(ALTIA_WINDOW win,
                     ALTIA_UINT32 dc,
                     Altia_Extent_type * extent,
                     ALTIA_INT copy)
{
    if (extent && copy)
    {
        /* We drew to off screen bitmap and now we have to
         * copy what we drew to the window.  This is required
         * for double buffering.
         *
         * It's assumed the extent has already been intersected
         * with the full screen window extent.
         */
        if (extent->x1 != extent->x0 || extent->y1 != extent->y0)
            altiaLibCopy(win, dc, extent);
    }
}
#endif /* ALTIA_TASKING */

