/* $Revision: 1.36 $    $Date: 2009-09-30 20:43:06 $
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




#include "altiaData.h"

/* Windows only */
#if defined(WIN32) && !defined(MICROWIN)
#pragma warning( disable: 4244 )
#endif


















extern ALTIA_BOOLEAN _altiaTextioAnimation(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_INDEX,
ALTIA_SHORT,
ALTIA_UINT32,
ALTIA_INDEX,
AltiaEventType
#endif
);


































/*----------------------------------------------------------------------
 * When custom animation occurs, the animation value will automaticly be
 * stored in the cooresponding custX array based upon the type of animation.
 * Then this function is called to do any additional processing required for
 * that custom animation.  This usually means coping the animation data to
 * the draw structure for that custom object if the animation is complete.
 * If the animation did not require a redraw then the custom animation
 * function should return a false.  If it does require a redraw then it should
 * return a true.  Any extent or transformer changes should be handled by
 * the custom animation routine.  One of the arguments to this function is
 * a pointer to an extent.  It can point to the current extent of the object,
 * as known by the caller (i.e., prior to any changes), or it can contain
 * NULL if the caller has no knowledge of the current extent.  It is
 * passed to a custom animation function if the custom animation function
 * accepts an extent pointer argument.  In this case, the custom animation
 * function has an opportunity to adjust the current extent (usually to
 * something smaller) if it knows that some portions of its full extent
 * do not require a redraw.  For example, an MLTO might only require a
 * redraw of a single line.  It is presumed that the caller has not yet
 * added the passed extent to the damage list, but it will do so following
 * the return of this function.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaCustomAnimation(ALTIA_INDEX findex,
                                    AltiaEventType value,
                                    ALTIA_UINT32 id,
                                    ALTIA_INDEX cindex,
                                    Altia_Extent_type *extent)
{
    ALTIA_INDEX object = ALTIA_FNAME_OBJECT(findex);
    ALTIA_INDEX first = ALTIA_DOBJ_FIRSTFUNC(object);
    ALTIA_SHORT count = ALTIA_DOBJ_FUNCCNT(object);
    switch (ALTIA_DOBJ_DRAWTYPE(object))
    {









        case AltiaTextioDraw:
            return _altiaTextioAnimation(object, first, count,
                                         id, cindex, value);
















        default:
            return false;
    }
}

/*----------------------------------------------------------------------
 * This function returns the index of the FunctionIndex array that
 * refers to the FunctionNameIndex passed in (nameIndex).  This is
 * used to get the name of an animation that is driving nameIndex.
 *----------------------------------------------------------------------*/
ALTIA_INDEX _altiaFindFuncIndex(ALTIA_BYTE funcType,
                                ALTIA_INDEX nameIndex)
{
    ALTIA_INDEX i, j;

    for (i = 0; i < (ALTIA_INDEX)test_funcFIndexs_count; i++)
    { /* Start by looking in the funcFIndex array */
        if (funcType == (ALTIA_BYTE)test_funcFIndexs[i].funcType &&
            nameIndex == (ALTIA_INDEX)test_funcFIndexs[i].index)
        { /* Found the FunctionFIndex now find the FuncIndex */
            for (j = 0; j < test_funcIndex_count; j++)
            {
                if (i >= test_funcIndex[j].firstIndex &&
                    i < test_funcIndex[j].firstIndex +
                    test_funcIndex[j].count)
                { /* We found the funcIndex record */
                    return j;
                }
            }
        }
    }
    return -1;
}




/*----------------------------------------------------------------------
 * This function finds the given Id in an objects animation function list.
 * The ids are not unique between different types of objects but they are
 * unique for a single object. It returns the index of the custI animation
 * that matches the id.
 *----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomIIndex(ALTIA_UINT32 id,
                                  ALTIA_INDEX first,
                                  ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomIAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (ALTIA_IVAL_ID(cindex) == id)
                return cindex;
        }
    }
    return -1;
}

/*----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomIFuncIndex(ALTIA_UINT32 id,
                                      ALTIA_INDEX first,
                                      ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomIAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (ALTIA_IVAL_ID(cindex) == id)
                return _altiaFindFuncIndex((ALTIA_BYTE)AltiaAnimationFunc, (ALTIA_INDEX)(first + i));
        }
    }
    return -1;
}



/*----------------------------------------------------------------------
 * This function finds the given Id in an objects animation function list.
 * The ids are not unique between different types of objects but they are
 * unique for a single object. It returns the index of the custF animation
 * that matches the id.
 *----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomFIndex(ALTIA_UINT32 id,
                                  ALTIA_INDEX first,
                                  ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomFAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (ALTIA_FVAL_ID(cindex) == id)
                return cindex;
        }
    }
    return -1;
}

/*----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomFFuncIndex(ALTIA_UINT32 id,
                                      ALTIA_INDEX first,
                                      ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomFAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (ALTIA_FVAL_ID(cindex) == id)
                return _altiaFindFuncIndex((ALTIA_BYTE)AltiaAnimationFunc, (ALTIA_INDEX)(first + i));
        }
    }
    return -1;
}



/*----------------------------------------------------------------------
 * This function finds the given Id in an objects animation function list.
 * The ids are not unique between different types of objects but they are
 * unique for a single object. It returns the index of the custS animation
 * that matches the id.
 *----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomSIndex(ALTIA_UINT32 id,
                                  ALTIA_INDEX first,
                                  ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomSAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (test_custS[cindex].id == id)
                return cindex;
        }
    }
    return -1;
}

/*----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetCustomSFuncIndex(ALTIA_UINT32 id,
                                      ALTIA_INDEX first,
                                      ALTIA_SHORT count)
{
    ALTIA_INDEX i;
    for (i = 0; i < count; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(first + i) == AltiaCustomSAnimation)
        {
            ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(first + i);
            if (test_custS[cindex].id == id)
                return _altiaFindFuncIndex((ALTIA_BYTE)AltiaAnimationFunc, (ALTIA_INDEX)(first + i));
        }
    }
    return -1;
}








