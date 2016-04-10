/* $Revision: 1.81 $    $Date: 2010-09-07 19:47:39 $
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
#ifdef DEBUG_WINDOW
#include <stdio.h>
#endif

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include <math.h>
#include "altiaData.h"

#ifdef WIN32
#undef y0
#undef y1
#endif

#ifndef NO_ROTATE
#define ANIMATION_ROTATE
#endif


/* DC - 2-13-07 - Changed for clones. */
extern ALTIA_BOOLEAN _altiaExecWhen(
#ifdef Altiafp
ALTIA_INDEX    windex,
ALTIA_INDEX    findex,
ALTIA_INT      cloneNum
#endif
);

extern ALTIA_BOOLEAN _altiaExecDelayedWhen(
#ifdef Altiafp
void
#endif
);

extern ALTIA_BOOLEAN altiaInWhen;



extern ALTIA_BOOLEAN _altiaCustomAnimation(
#ifdef Altiafp
ALTIA_INDEX,
AltiaEventType,
ALTIA_UINT32,
ALTIA_INDEX,
Altia_Extent_type *extent
#endif
);




extern ALTIA_BOOLEAN altiaCheckBuiltins(
#ifdef Altiafp
ALTIA_CHAR *funcName,
AltiaEventType value
#endif
);

extern ALTIA_BOOLEAN altiaExecBuiltinId(
#ifdef Altiafp
int nameId,
AltiaEventType value
#endif
);

extern ALTIA_INDEX altiaFindBuiltinId(
#ifdef Altiafp
ALTIA_CHAR *funcName
#endif
);

extern ALTIA_CHAR * altiaFindBuiltinName(
#ifdef Altiafp
ALTIA_INDEX nameId
#endif
);




extern void altiaStartTimer(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_UINT32
#endif
);
extern void altiaStopTimer(
#ifdef Altiafp
ALTIA_INDEX
#endif
);
extern ALTIA_BOOLEAN _altiaEnabled(
#ifdef Altiafp
ALTIA_BYTE,
ALTIA_CHAR *,
AltiaEventType
#endif
);






#ifdef ALTIA_TASKING
extern ALTIA_BOOLEAN TargetAltiaTaskQueue(
#ifdef Altiafp
AltiaTaskElementType *
#endif
);

extern ALTIA_BOOLEAN AltiaTaskRunning;
#endif /* ALTIA_TASKING */

/*
 * AltiaReportFuncPtr is a function pointer who's function gets called
 * when AltiaAnimate completes starts an Animation.  If the user wants
 * know what animations are occuring then he should assign this ptr a
 * function that he wants to call.
 */
AltiaReportFuncType AltiaReportFuncPtr = NULL;

/*
 * AltiaInvalidAnimationNamePtr is a function pointer who's function
 * gets called when AltiaAnimate cannot find the specific Animation.
 * This informs the user to if the application logic is sending events
 * on Animations that don't exist.
 */
AltiaReportFuncType AltiaInvalidAnimationNamePtr = NULL;

/*
 * MAXLOOPCOUNT is the number of times that AltiaAnimate can be called
 * recursively as a result of connections.  This number should be set
 * small enought as to not overflow stacks.
 */
#define MAXLOOPCOUNT  50










/*----------------------------------------------------------------------
 * This function builds a transform from the base and offset indexes
 *----------------------------------------------------------------------*/
#ifdef Altiafp
void _altiaBldTrans(ALTIA_INDEX baseidx, ALTIA_INDEX offidx,
                           Altia_Transform_type *tptr)
#else
void _altiaBldTrans(baseidx, offidx, tptr)
ALTIA_INDEX baseidx;
ALTIA_INDEX offidx;
Altia_Transform_type *tptr;
#endif
{
    tptr->a00 = test_basetrans[baseidx].a00;
    tptr->a01 = test_basetrans[baseidx].a01;
    tptr->a10 = test_basetrans[baseidx].a10;
    tptr->a11 = test_basetrans[baseidx].a11;

    tptr->angle = test_basetrans[baseidx].angle;


    if (offidx != -1)
    {
        tptr->a20 = test_offtrans[offidx].a20;
        tptr->a21 = test_offtrans[offidx].a21;

        tptr->xoffset = test_offtrans[offidx].xoffset;
        tptr->yoffset = test_offtrans[offidx].yoffset;


    }

}




/*----------------------------------------------------------------------
 * This function takes the given object and adjusts its total transform
 * to be the total transform for that object.  Its does this assuming that
 * the parents total transform is correct.
 *----------------------------------------------------------------------*/
void _altiaTotalTrans(ALTIA_SHORT objType,
                      ALTIA_INDEX obj)
{
    ALTIA_INDEX parent;

    ALTIA_SHORT drawType;
    ALTIA_INDEX drawIndex;

    Altia_Transform_type *tptr;

    if (objType == AltiaDynamicObject)
    {

        parent = ALTIA_DOBJ_PARENT(obj);
        altiaLibCopyTrans(ALTIA_DOBJ_TOTAL_ADDR(obj),
	                      ALTIA_DOBJ_TRANS_ADDR(obj));
        tptr = ALTIA_DOBJ_TOTAL_ADDR(obj);

        drawType = ALTIA_DOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_DOBJ_DRAWINDEX(obj);


    }else
    {



        Altia_Transform_type strans;
        parent = ALTIA_SOBJ_PARENT(obj);

        /* NOTE:  For every altiaLibInitTrans(), there must be
         * a matching altiaLibEndTrans() before returning.
         */
        altiaLibInitTrans(&strans);

        _altiaBldTrans(ALTIA_SOBJ_BASETRANS(obj), (ALTIA_INDEX)-1, &strans);
        /* We have to copy the otrans since statics use the actual trans
         * instead of an offset to a offset trans array.  This is because
         * if the static gets sent a altiaMoveObj animation the offset
         * part of the transform must change so we can't share it.
         */
        strans.a20     = ALTIA_SOBJ_OTRANS(obj).a20;
        strans.a21     = ALTIA_SOBJ_OTRANS(obj).a21;

        strans.xoffset = ALTIA_SOBJ_OTRANS(obj).xoffset;
        strans.yoffset = ALTIA_SOBJ_OTRANS(obj).yoffset;




        altiaLibCopyTrans(ALTIA_SOBJ_TOTAL_ADDR(obj), &strans);
        altiaLibEndTrans(&strans);
        tptr = ALTIA_SOBJ_TOTAL_ADDR(obj);

        drawType = ALTIA_SOBJ_DRAWTYPE(obj);
        drawIndex = ALTIA_SOBJ_DRAWINDEX(obj);



    }

    if (parent != -1)
    {
        altiaLibPostMultiplyTrans(tptr, ALTIA_DOBJ_TOTAL_ADDR(parent));
    }

    /* If any objects need to clear data based upon a change in
     * its or a parents transform, put the code here.
     */


    if (drawType == AltiaRasterDraw)
    {
        Altia_Raster_type * raster = altiaLibRasterFromIndex(drawIndex);
        if (raster)
        {
            altiaLibRasterClear(raster);
            altiaLibRasterToIndex(raster, drawIndex);
        }
    }








}


/*----------------------------------------------------------------------
 * This function goes through an objects children (if any) and recursively
 * causes all the children's total transforms to be set.
 *----------------------------------------------------------------------*/
void _altiaPushTrans(ALTIA_SHORT objType, ALTIA_INDEX obj)
{
    if (objType == AltiaDynamicObject)
    {
        int count = ALTIA_DOBJ_CHILDCNT(obj);
        ALTIA_INDEX first =  ALTIA_DOBJ_FIRSTCHILD(obj);
        int i;
        ALTIA_SHORT ctype;
        ALTIA_INDEX child;

        for (i = 0; i < count; i++)
        {
            ctype = (ALTIA_SHORT)test_children[first + i].type;
            child = (ALTIA_INDEX)test_children[first + i].child;

            /* Child index is -1 if child is popped to top. */
            if (child != -1)
            {
                _altiaTotalTrans(ctype, child);
                _altiaPushTrans(ctype, child);
            }
        }
    }
}



/*----------------------------------------------------------------------
 * Given an animation state index, a animation mask, and a dynamic object,
 * this function sets the object's attributes based upon the mask.
 *----------------------------------------------------------------------*/
static void _altiaSetStateAttributes(ALTIA_INDEX i,
                                     ALTIA_UBYTE mask,
                                     ALTIA_INDEX obj)
{
    if ((mask & ALTIA_STATE_NOSAVE_FOREGROUND) == 0)
    {
        ALTIA_SET_DOBJ_FOREGROUND(obj,test_states[i].foreground);
    }
    if ((mask & ALTIA_STATE_NOSAVE_BACKGROUND) == 0)
    {
        ALTIA_SET_DOBJ_BACKGROUND(obj,test_states[i].background);
    }
    if ((mask & ALTIA_STATE_NOSAVE_PATTERN) == 0)
    {
        ALTIA_SET_DOBJ_PATTERN(obj,test_states[i].pattern);
    }
    if ((mask & ALTIA_STATE_NOSAVE_BRUSH) == 0)
    {
        ALTIA_SET_DOBJ_BRUSH(obj,test_states[i].brush);
    }
    if ((mask & ALTIA_STATE_NO_ALPHA) == 0)
    {
        ALTIA_SET_DOBJ_ALPHA(obj,test_states[i].alpha);
    }
    if ((mask & ALTIA_STATE_NO_MAP) != 0)
    {

        ALTIA_SET_DOBJ_MAPPED(obj,ALTIA_FALSE);
    }else
    {

        ALTIA_SET_DOBJ_MAPPED(obj,ALTIA_TRUE);
    }
}



/*----------------------------------------------------------------------
 * This function sets the state of an object based an animation function index
 * and a value. If there is not an exact match, this function will do the
 * interpolation as required. If an animation state change occurs, this
 * function will add the correct extents to the list and push down any
 * transform changes to the children to that the extents and total transforms
 * remain correct.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaSetState(ALTIA_INDEX findex,
                                    AltiaEventType value)
{
    ALTIA_INDEX i;
    ALTIA_INDEX first = ALTIA_FNAME_FIRST(findex);
    ALTIA_INDEX last  = first + ALTIA_FNAME_COUNT(findex) - 1;
    ALTIA_INDEX obj   = ALTIA_FNAME_OBJECT(findex);
    ALTIA_INDEX low   = (ALTIA_INDEX)-1;
    ALTIA_INDEX high  = (ALTIA_INDEX)-1;
    ALTIA_BOOLEAN match = ALTIA_FALSE;
    Altia_Extent_type extent;
    Altia_Transform_type *group = 0;
    ALTIA_SHORT preMult = 0;
    for (i = first; i <= last; i++)
    {
        AltiaEventType state = (AltiaEventType)test_states[i].state;
        ALTIA_UBYTE mask = (ALTIA_UBYTE)test_states[i].mask;

        if (ALTIA_EQUAL(value, state) ||
            ((first != last) && (mask & ALTIA_STATE_OVERFLOW_STATE) != 0 &&
             (((i == first) && ALTIA_LESS(value, state)) ||
              ((i == last) &&  ALTIA_GREATER(value, state)))))
        {
            Altia_Transform_type strans;
            ALTIA_INDEX baseidx = (ALTIA_INDEX)test_states[i].basetrans;
            ALTIA_INDEX offidx = (ALTIA_INDEX)test_states[i].offtrans;

            /* NOTE:  For every altiaLibInitTrans(), there must be
             * a matching altiaLibEndTrans() before returning.
             */
            altiaLibInitTrans(&strans);

            _altiaBldTrans(baseidx, offidx, &strans);

            _altiaAddExtent(AltiaDynamicObject, obj);


            altiaLibDoRelTrans(ALTIA_DOBJ_TRANS_ADDR(obj),
                               &strans, ALTIA_DOBJ_LASTTRANS_ADDR(obj), 
                               group, preMult);
            altiaLibEndTrans(&strans);
            _altiaTotalTrans(AltiaDynamicObject, obj);

            _altiaPushTrans(AltiaDynamicObject, obj);

            _altiaSetStateAttributes((ALTIA_INDEX)i, (ALTIA_UBYTE)test_states[i].mask, obj);
            _altiaFindExtent(AltiaDynamicObject, obj, &extent);

            _altiaUpdateParentExtent(AltiaDynamicObject, obj);

            _altiaAddExtent(AltiaDynamicObject, obj);
            match = ALTIA_TRUE;
            break;
        }
        else if (ALTIA_GREATER(value, state))
        {
            low = i;
        }
        else
        {
            high = i;
            break;
        }
    }
    if (match == ALTIA_FALSE && low != -1 && high != -1)
    {
        ALTIA_FLOAT cx, cy;
        Altia_Transform_type tl;
        Altia_Transform_type th;
        int currAlpha, newAlpha, temp1, temp2;
        int alphadiff;
        Altia_Transform_type saveTrans;
#ifdef ANIMATION_ROTATE
        ALTIA_BOOLEAN rotated = ALTIA_FALSE;
#endif


        ALTIA_FIXPT lowVal = ALT_I2FX(test_states[low].state);
        ALTIA_FIXPT highVal = ALT_I2FX(test_states[high].state);
        ALTIA_FIXPT percent = ALT_DIVFX((ALT_I2FX(value) - lowVal), (highVal - lowVal));

        ALTIA_FLOAT angle;

        /* NOTE:  For every altiaLibInitTrans(), there must be
         * a matching altiaLibEndTrans() before returning.
         */
        altiaLibInitTrans(&saveTrans);
        altiaLibInitTrans(&tl);
        altiaLibInitTrans(&th);

        altiaLibCopyTrans(&saveTrans, ALTIA_DOBJ_TRANS_ADDR(obj));

        _altiaBldTrans((ALTIA_INDEX)test_states[low].basetrans,
                       (ALTIA_INDEX)test_states[low].offtrans, &tl);

        _altiaBldTrans((ALTIA_INDEX)test_states[high].basetrans,
                       (ALTIA_INDEX)test_states[high].offtrans, &th);





        angle = ALT_FX2F(th.angle - tl.angle);


        _altiaAddExtent(AltiaDynamicObject, obj);
#ifdef ANIMATION_ROTATE
        if (!ALTIA_EQUAL(angle, ALTIA_C2F(0.0)))
        {
            altiaLibTransformF(&th,
                              ALTIA_DOBJ_BEHAVE_RPTX(obj),
                              ALTIA_DOBJ_BEHAVE_RPTY(obj),
                              &cx, &cy);
            altiaLibTranslate(&th, -cx, -cy);
            altiaLibRotate(&th, -angle);
            altiaLibTranslate(&th, cx, cy);
            rotated = ALTIA_TRUE;
        }
#endif
        altiaLibInterpolate(&tl, percent, &th);



#ifdef ANIMATION_ROTATE
        if (rotated != ALTIA_FALSE)
        {


            angle = ALT_FX2F(ALT_MULFX(ALT_F2FX(angle), percent));

            if (!ALTIA_EQUAL(angle, ALTIA_C2F(0.0)))
            {
                altiaLibTransformF(&tl,
                                   ALTIA_DOBJ_BEHAVE_RPTX(obj),
                                   ALTIA_DOBJ_BEHAVE_RPTY(obj),
                                   &cx, &cy);
                altiaLibTranslate(&tl, -cx, -cy);
                altiaLibRotate(&tl, angle);
                altiaLibTranslate(&tl, cx, cy);
            }
            altiaLibDoRelTrans(&saveTrans,&tl,
                               ALTIA_DOBJ_LASTTRANS_ADDR(obj), group,
                               preMult);
        }
        else
#endif /* ANIMATION_ROTATE */
        {
            altiaLibDoRelTrans(&saveTrans,&tl,
                               ALTIA_DOBJ_LASTTRANS_ADDR(obj), group,
                               preMult);
        }
        altiaLibCopyTrans(ALTIA_DOBJ_TRANS_ADDR(obj), &saveTrans);
        _altiaTotalTrans(AltiaDynamicObject, obj);

        _altiaPushTrans(AltiaDynamicObject, obj);

        /* alpha channel support (this has to be done prior to setting the
         *                        alpha channel to the next state).
         */
        if (test_states[low].mask & ALTIA_STATE_NO_ALPHA ||
            test_states[high].mask & ALTIA_STATE_NO_ALPHA)
        {
            currAlpha = newAlpha = ALTIA_DOBJ_ALPHA(obj);
        }
        else
        {
            newAlpha = test_states[high].alpha;
        }
        /* alpha channel support end */

        _altiaSetStateAttributes(low, (ALTIA_UBYTE)test_states[low].mask, obj);

        /* alpha channel support */
        currAlpha = ALTIA_DOBJ_ALPHA(obj);
        if (currAlpha == -1)
            temp1 = 255;
        else
            temp1 = currAlpha;
        if (newAlpha == -1)
            temp2 = 255;
        else
            temp2 = newAlpha;
        alphadiff = temp2 - temp1;
        if (alphadiff != 0)
        {
            ALTIA_SET_DOBJ_ALPHA(obj,(temp1 + ALTIA_ROUND(alphadiff * percent)));
            if (ALTIA_DOBJ_ALPHA(obj) == 255)
                ALTIA_SET_DOBJ_ALPHA(obj,-1);
        }
        /* alpha channel support end */

        _altiaFindExtent(AltiaDynamicObject, obj, &extent);

        _altiaUpdateParentExtent(AltiaDynamicObject, obj);

        _altiaAddExtent(AltiaDynamicObject, obj);
        match = ALTIA_TRUE;
        altiaLibEndTrans(&saveTrans);
        altiaLibEndTrans(&tl);
        altiaLibEndTrans(&th);
    }
    return match;
}



/*----------------------------------------------------------------------
 * This function handles the generic code for processing a custom integer
 * animation change.  This code handles basic processing and storing of the
 * animation so the custom objects don't all have to repeat this code.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaCustomIState(ALTIA_INDEX findex,
                                        AltiaEventType value,
                                        ALTIA_UINT32 *id)
{
    ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(findex);
    int newval;
    *id = ALTIA_IVAL_ID(cindex);
    newval = ALTIA_ROUND(ALTIA_E2D(value));
    if ((ALTIA_IVAL_FLAG(cindex) & ALTIA_CUSTI_SAME_BEHAVIOR) == 0)
    {
        int oldval = ALTIA_IVAL_VALUE(cindex);
        if (oldval == newval)
            return ALTIA_FALSE;
    }
    if ((ALTIA_IVAL_FLAG(cindex) & ALTIA_CUSTI_RANGE_CHECK) != 0)
    {
        if ((newval < ALTIA_IVAL_MIN(cindex)) || (newval > ALTIA_IVAL_MAX(cindex)))
            return ALTIA_FALSE;
    }
    if ((ALTIA_IVAL_FLAG(cindex) & ALTIA_CUSTI_MIN_MAX_SET) != 0)
    {
        if (newval < ALTIA_IVAL_MIN(cindex))
            newval = ALTIA_IVAL_MIN(cindex);
        else if (newval > ALTIA_IVAL_MAX(cindex))
            newval = ALTIA_IVAL_MAX(cindex);
    }
    if ((ALTIA_IVAL_FLAG(cindex) & ALTIA_CUSTI_RANGE_ZERO) != 0)
    {
        if ((newval < ALTIA_IVAL_MIN(cindex)) || (newval > ALTIA_IVAL_MAX(cindex)))
            newval = 0;
    }
    ALTIA_SET_IVAL_VALUE(cindex,newval);
    ALTIA_SET_IVAL_FLAG(cindex,ALTIA_IVAL_FLAG(cindex) | ALTIA_CUSTI_MODIFIED_VALUE);
    return ALTIA_TRUE;
}



/*----------------------------------------------------------------------
 * This function handles the generic code for processing a custom float
 * animation change.  This code handles basic processing and storing of the
 * animation so the custom objects don't all have to repeat this code.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaCustomFState(ALTIA_INDEX findex,
                                        AltiaEventType value,
                                        ALTIA_UINT32 *id)
{
    ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(findex);
    ALTIA_FLOAT newval;
    *id = ALTIA_FVAL_ID(cindex);
    newval = (ALTIA_FLOAT) (ALTIA_ROUND(ALTIA_E2D(value)));
    if ((ALTIA_FVAL_FLAG(cindex) & ALTIA_CUSTF_SAME_BEHAVIOR) == 0)
    {
        int oldval = ALTIA_F2I(ALTIA_FVAL_VALUE(cindex));
        if (oldval == newval)
            return ALTIA_FALSE;
    }
    if ((ALTIA_FVAL_FLAG(cindex) & ALTIA_CUSTF_RANGE_CHECK) != 0)
    {
        if ((newval < ALTIA_FVAL_MIN(cindex)) || (newval > ALTIA_FVAL_MAX(cindex)))
            return ALTIA_FALSE;
    }
    if ((ALTIA_FVAL_FLAG(cindex) & ALTIA_CUSTI_MIN_MAX_SET) != 0)
    {
        if (newval < ALTIA_FVAL_MIN(cindex))
            newval = ALTIA_FVAL_MIN(cindex);
        else if (newval > ALTIA_FVAL_MAX(cindex))
            newval = ALTIA_FVAL_MAX(cindex);
    }
    if ((ALTIA_FVAL_FLAG(cindex) & ALTIA_CUSTI_RANGE_ZERO) != 0)
    {
        if ((newval < ALTIA_FVAL_MIN(cindex)) || (newval > ALTIA_FVAL_MAX(cindex)))
            newval = 0;
    }
    ALTIA_SET_FVAL_VALUE(cindex,newval);
    ALTIA_SET_FVAL_FLAG(cindex,ALTIA_FVAL_FLAG(cindex) | ALTIA_CUSTF_MODIFIED_VALUE);
    return ALTIA_TRUE;
}



/*----------------------------------------------------------------------
 * This function handles the generic code for processing a custom string
 * animation change.  This code handles basic processing and storing of the
 * animation so the custom objects don't all have to repeat this code.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaCustomSState(ALTIA_INDEX findex,
                                        AltiaEventType value,
                                        ALTIA_UINT32 *id)
{
    ALTIA_CHAR newval[2];
    ALTIA_INT rem;
    ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(findex);
    Altia_CustomSValue_type *sobj = &test_custS[cindex];
    *id = sobj->id;
    if ((sobj->flag & ALTIA_CUSTS_ADD) != 0)
        return ALTIA_TRUE;  /* Don't add the char the custom obj will add it. */


    if (value == -1)

        return ALTIA_TRUE;
    newval[0] = (ALTIA_CHAR) value;
    newval[1] = 0;
    if (newval[0] == 0)
    {
        sobj->flag |= ALTIA_CUSTS_MODIFIED_VALUE;
    }
    else
    {

        newval[0] = (ALTIA_CHAR)ALTIA_ROUND(ALTIA_E2D(value));
        if ((sobj->flag & ALTIA_CUSTS_MODIFIED_VALUE) != 0)
        {

            /* Only copy character if destination is large enough */
            if (ALTIA_CUSTOMSVALUE_SIZE > ALT_STRLEN(newval))

            ALT_STRCPY(sobj->value, newval);
            sobj->flag &= ALTIA_CUSTS_CLEAR_MODIFIED;
        }
        else

        /* Only copy character if destination is large enough */
        if (ALTIA_CUSTOMSVALUE_SIZE > (ALT_STRLEN(sobj->value) + 1))

        {
            if (sobj->index == -1)
                ALT_STRCAT(sobj->value, newval);
            else
            {
                /* We are inserting into the middle of the string */
                rem = (ALTIA_INT)ALT_STRLEN(&sobj->value[sobj->index]);
                if (rem == 0)
                    ALT_STRCAT(sobj->value, newval);
                else
                {
                    /* a real insert */
                    memmove(&sobj->value[sobj->index+1],
                            &sobj->value[sobj->index],
                            (rem+1)*sizeof(ALTIA_CHAR));
                    sobj->value[sobj->index] = newval[0];
                }
            }
        }
    }
    return ALTIA_TRUE;
}



static ALTIA_BOOLEAN _checkDeckChildren(ALTIA_INDEX oidx,
                                        ALTIA_INDEX cIndex,
                                        ALTIA_SHORT cType,
                                        ALTIA_BOOLEAN *foundDeck,
                                        ALTIA_INDEX childArr[],
                                        int *arrCntPtr)
{
    int i,j,k;
    int firstFunc = ALTIA_DOBJ_FIRSTFUNC(oidx);
    int funcCnt = ALTIA_DOBJ_FUNCCNT(oidx);

    *foundDeck = ALTIA_FALSE;
    for (i = 0; i < funcCnt; i++)
    {
        if (ALTIA_FNAME_ANIMATETYPE(firstFunc+i) == AltiaDeckAnimation)
        {
            int first = ALTIA_FNAME_FIRST(firstFunc + i);
            int cnt = ALTIA_FNAME_COUNT(firstFunc + i);
            *foundDeck = ALTIA_TRUE;
            for (j = 0; j < cnt; j++)
            {
                int dcnt = (int)test_decks[first+j].childCnt;
                int dfirst = (int)test_decks[first+j].firstChild;
                for (k = 0; k < dcnt; k++)
                {
                    ALTIA_INDEX index = (ALTIA_INDEX)test_children[dfirst+k].child;
                    ALTIA_SHORT childt = (ALTIA_SHORT)test_children[dfirst+k].type;

                    if (index == cIndex && childt == cType)
                        return ALTIA_TRUE;
                    /* Since the child could be a deck with no current
                     * children showing we can't check for -1 and 0 for
                     * child index and count since that could be ALTIA_TRUE
                     * yet still have children
                     */
                    if (index != -1 && childt == AltiaDynamicObject)
                    {
                        if (*arrCntPtr >= test_children_count)
                            _altiaErrorMessage(
                                  ALT_TEXT("To many children in altiaIsChild"));
                        else
                        {
                            childArr[*arrCntPtr] = index;
                            *arrCntPtr = *arrCntPtr + 1;
                        }
                    }
                }
            }
        }
    }
    return ALTIA_FALSE;
}










/*----------------------------------------------------------------------
 * This function handles the card switching animation of the deck object.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaSwitchCard(ALTIA_INDEX findex,
                                      AltiaEventType value)
{
    int i;
    int first = ALTIA_FNAME_FIRST(findex);
    int last = first + ALTIA_FNAME_COUNT(findex);
    ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);
    ALTIA_BOOLEAN match = ALTIA_FALSE;
    Altia_Extent_type extent;

    for (i = first; i < last; i++)
    {
        if (ALTIA_EQUAL(value, ((AltiaEventType)test_decks[i].card)))
        {
    
            _altiaAddExtent(AltiaDynamicObject, obj);
    
            ALTIA_SET_DOBJ_FIRSTCHILD(obj,test_decks[i].firstChild);
            ALTIA_SET_DOBJ_CHILDCNT(obj,test_decks[i].childCnt);
    
            _altiaPushTrans(AltiaDynamicObject, obj);
    
            _altiaFindExtent(AltiaDynamicObject, obj, &extent);
    
            _altiaUpdateParentExtent(AltiaDynamicObject, obj);
    
            _altiaAddExtent(AltiaDynamicObject, obj);
            match = ALTIA_TRUE;
            break;
        }
    }
    if (match == ALTIA_FALSE)
    { /* Take away all the children for the undefined card */
    
        _altiaAddExtent(AltiaDynamicObject, obj);
        ALTIA_SET_DOBJ_FIRSTCHILD(obj,-1);
        ALTIA_SET_DOBJ_CHILDCNT(obj,0);
    
        _altiaPushTrans(AltiaDynamicObject, obj);
    
        _altiaFindExtent(AltiaDynamicObject, obj, &extent);
    
        _altiaUpdateParentExtent(AltiaDynamicObject, obj);
    
        _altiaAddExtent(AltiaDynamicObject, obj);
        match = ALTIA_TRUE; /* Always return ALTIA_TRUE for decks */
    }
    return match;
}



/*----------------------------------------------------------------------*/
ALTIA_INDEX _altiaFindAnimationIdx(ALTIA_CHAR *funcName)
{
    static ALTIA_INDEX last_index = -1;
    ALTIA_BOOLEAN done = ALTIA_FALSE;
    ALTIA_INDEX low = 0;
    ALTIA_INDEX high = (ALTIA_INDEX)(test_funcIndex_count - 1);
    ALTIA_INDEX index = high / 2;
    int res = -1;

    ALTIA_CHAR *namePtr = funcName;


    if (last_index != -1)
    {
        if (ALT_STRCMP(namePtr, test_funcIndex[last_index].name) == 0)
        {
            return last_index;
        }
    }

    while (done == ALTIA_FALSE)
    {

        res = ALT_STRCMP(namePtr, test_funcIndex[index].name);
        if (res == 0)
        {

            last_index = index;
            break;
        }
        else
        {

            if (res < 0)
                high = index - 1;
            else
                low = index + 1;
            if (low > high)
                done = ALTIA_TRUE;
            else
                index = (high + low)/2;

        }
    }
    if (done == ALTIA_TRUE)
    {
        return -1;
    }
    return index;
}


/*----------------------------------------------------------------------
 * This function finds the current value for an animation or returns 0.
 * Note:  This function must always exist for API, but can
 *        do nothing other than return 0 if there are no
 *        animations in the design.
 *----------------------------------------------------------------------*/
#ifdef Altiafp
AltiaEventType _altiaFindCurVal(ALTIA_CHAR *funcName)
#else
AltiaEventType _altiaFindCurVal(funcName)
ALTIA_CHAR *funcName;
#endif
{
    AltiaEventType retVal = 0;

    ALTIA_INDEX index;
    index = _altiaFindAnimationIdx(funcName);
    if (index >= 0)
        retVal = test_curValue[index];
    else
    {
#ifdef DEBUG_WINDOW
        /*if (ALT_STRCMP(funcName, "None") != 0)
         *    fprintf(stderr, "Animation %s not found\n", funcName);
         */
#endif
    }

    return retVal;
}


/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaIsString(ALTIA_INDEX funcIdx)
{


    int i;
    int count = test_funcIndex[funcIdx].count;
    int first = test_funcIndex[funcIdx].firstIndex;
    for (i = 0; i < count; i++)
    {
        ALTIA_BYTE funcType = (ALTIA_BYTE)test_funcFIndexs[first+i].funcType;
        if (funcType == AltiaAnimationFunc)
        {
            ALTIA_INDEX findex = (ALTIA_INDEX)test_funcFIndexs[first+i].index;

            if (ALTIA_FNAME_ANIMATETYPE(findex) ==
                 AltiaCustomSAnimation)
            {
                return ALTIA_TRUE;
            }

        }
    }


    return ALTIA_FALSE;
}




/*----------------------------------------------------------------------*/
ALTIA_CHAR *_altiaFindCurString(ALTIA_CHAR *funcName)
{
    static ALTIA_CHAR emptyRet[1] = {0};

    int i;
    ALTIA_INDEX index = _altiaFindAnimationIdx(funcName);
    if (index >= 0)
    {
        int count = test_funcIndex[index].count;
        int first = test_funcIndex[index].firstIndex;
        for (i = 0; i < count; i++)
        {
            ALTIA_BYTE funcType = (ALTIA_BYTE)test_funcFIndexs[first+i].funcType;
            if (funcType == AltiaAnimationFunc)
            {
                ALTIA_INDEX findex = (ALTIA_INDEX)test_funcFIndexs[first+i].index;
                if (ALTIA_FNAME_ANIMATETYPE(findex) ==
                     AltiaCustomSAnimation)
                {
                    ALTIA_INDEX cindex = ALTIA_FNAME_FIRST(findex);
                    Altia_CustomSValue_type *sobj = &test_custS[cindex];

                    return sobj->value;
                }
            }
        }
    }else
    {
#ifdef DEBUG_WINDOW
        /*if (ALT_STRCMP(funcName, "None") != 0)
         *    fprintf(stderr, "Animation string %s not found\n", funcName);
         */   fprintf(stderr, "Animation string %s not found\n", funcName);
#endif
    }

    return emptyRet;
}

/*----------------------------------------------------------------------
 * This function checks animation index to see if it is an OUTPUT
 * connection that is driving a float animation or a built-in
 * float animation (e.g., textio "float") and returns ALTIA_TRUE if it
 * is either of these.
 * Note:  Passing a clone's animation index does not work!  In other
 *        words, do not use API functions that take an index when cloning.
 * Note:  This function must always exist for API, but can
 *        do nothing other than return ALTIA_FALSE if there are no
 *        animations in the design.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaConnectionToFloatId(int index)
{


    if ((index >=0) && (index < test_funcIndex_count))
    {
        int i;
        int count = test_funcIndex[index].count;
        int first = test_funcIndex[index].firstIndex;
        for (i = 0; i < count; i++)
        {
            ALTIA_INDEX findex;
            ALTIA_BYTE funcType = (ALTIA_BYTE)test_funcFIndexs[first + i].funcType;
            findex = (ALTIA_INDEX)test_funcFIndexs[first + i].index;
            if (funcType == AltiaConnectionFunc)
            { /* We have connection animation */

                continue;
            }

            if (funcType != AltiaAnimationFunc)
                continue;


            if (ALTIA_FNAME_ANIMATETYPE(findex)
                == AltiaCustomFAnimation)
                return ALTIA_TRUE;

        }
    }


    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------
 * This function checks animation funcName to see if it is an OUTPUT
 * connection that is driving a float animation or a built-in
 * float animation (e.g., textio "float") and returns ALTIA_TRUE if it
 * is either of these.
 * Note:  This function must always exist for API, but can
 *        do nothing other than return ALTIA_FALSE if there are no
 *        animations in the design.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaConnectionToFloat(ALTIA_CHAR *funcName)
{
    ALTIA_BOOLEAN retval = ALTIA_FALSE;


    ALTIA_INDEX index;

    index = _altiaFindAnimationIdx(funcName);
    retval = _altiaConnectionToFloatId((int) index);


    return retval;
}

/*----------------------------------------------------------------------
 * This function does the work for _altiaConnectionFromFloatId(index)
 * and _altiaConnectionFromFloat(funcName).  It checks the animation
 * index or funcName to see if it is a built-in float animation (e.g.,
 * textio "float") or an external INPUT connection driven by a float
 * connection.  It returns ALTIA_TRUE for either.
 * Note:  Passing a clone's animation index does not work!  In other
 *        words, do not use API functions that take an index when cloning.
 * Note:  External connections have a funcIndex without a funcFIndex
 *        because their funcIndex count is 0 so this code should exist
 *        even without funcFIndexs (although it is unlikely to happen).
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaConnectionFromFloatUtil(ALTIA_INDEX index,
                                                   ALTIA_CHAR *funcName)
{

    ALTIA_BOOLEAN isexternal = ALTIA_FALSE;


    /* Get an index from funcName if possible. */
    if (funcName != NULL)
        index = _altiaFindAnimationIdx(funcName);

    /* First try checking if the animation exists and its type is float */
    if ((index >=0) && (index < test_funcIndex_count))
    {

        int i;
        int first = test_funcIndex[index].firstIndex;

        int count = test_funcIndex[index].count;

        /* If animation has no count, could be an external. */
        if (count == 0)
            isexternal = ALTIA_TRUE;

        for (i = 0; i < count; i++)
        {
            ALTIA_INDEX findex;
            ALTIA_BYTE funcType = (ALTIA_BYTE)test_funcFIndexs[first + i].funcType;
            findex = (ALTIA_INDEX)test_funcFIndexs[first + i].index;

            if (funcType == AltiaAnimationFunc
                && ALTIA_FNAME_ANIMATETYPE(findex)
                   == AltiaCustomFAnimation)
                return ALTIA_TRUE;
        }

    }

    /* The animation was not a custom float type (e.g., textio "float").
     * Now check if this animation is an external INPUT of type float.
     */



    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------
 * This function checks animation index to see if it is a built-in
 * float animation (e.g., textio "float") or an external INPUT connection
 * driven by a float connection.  It returns ALTIA_TRUE if it is either of these.
 * Note:  This function must always exist for API, but can
 *        do nothing other than return ALTIA_FALSE if there are no
 *        animations in the design.
 * Note:  Passing a clone's animation index does not work!  In other
 *        words, do not use API functions that take an index when cloning.
 * Note:  External connections have a funcIndex without a funcFIndex
 *        because their funcIndex count is 0 so this code should exist
 *        even without funcFIndexs (although it is unlikely to happen).
 *----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN _altiaConnectionFromFloatId(int index)
#else
ALTIA_BOOLEAN _altiaConnectionFromFloatId(index)
int index;
#endif
{
    ALTIA_BOOLEAN retval = ALTIA_FALSE;

    /* In this case, have an index, not a function name. */
    retval = _altiaConnectionFromFloatUtil((ALTIA_INDEX) index, NULL);

    return retval;
}

/*----------------------------------------------------------------------
 * This function checks animation funcName to see if it is a built-in
 * float animation (e.g., textio "float") or an external INPUT connection
 * driven by a float connection.  It returns ALTIA_TRUE if it is either of these.
 * Note:  This function must always exist for API, but can
 *        do nothing other than return ALTIA_FALSE if there are no
 *        animations in the design.
 * Note:  External connections have a funcIndex without a funcFIndex
 *        because their funcIndex count is 0 so this code should exist
 *        even without funcFIndexs (although it is unlikely to happen).
 *----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN _altiaConnectionFromFloat(ALTIA_CHAR *funcName)
#else
ALTIA_BOOLEAN _altiaConnectionFromFloat(funcName)
ALTIA_CHAR *funcName;
#endif
{
    ALTIA_BOOLEAN retval = ALTIA_FALSE;

    /* In this case, no index, just a function name. */
    retval = _altiaConnectionFromFloatUtil(-1, funcName);

    return retval;
}


/*----------------------------------------------------------------------
 * This function does the animation of funcName and value.  If
 * a redraw is required as a result of this animation then ALTIA_TRUE is returned
 * and the extents of what to draw are setup for the update function. This
 * function can be called as many times as needed before a draw is
 * actually done.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN AltiaAnimate(ALTIA_CHAR *funcName, AltiaEventType value)
{
    int nameId = AltiaAnimationNameToId(funcName);
    if (nameId < 0)
    {
        /*
         * Report to the user that this function was called if they want to know
         */
        if (AltiaReportFuncPtr != NULL)
            (*AltiaReportFuncPtr)(funcName, value);

        /*
         * Report to the user that this function doesn't exist if
         * they want to know
         */
        if (AltiaInvalidAnimationNamePtr != NULL)
                (AltiaInvalidAnimationNamePtr)(funcName, value);

        return ALTIA_FALSE;
    }

    return AltiaAnimateId(nameId, value);
}

/*----------------------------------------------------------------------
 * This function does the animation of nameId and value.  If
 * a redraw is required as a result of this animation then ALTIA_TRUE is returned
 * and the extents of what to draw are setup for the update function. This
 * function can be called as many times as needed before a draw is
 * actually done.
 * Note:  Animating with a clone's animation index does not work!
 *        In other words, do not use API functions that take an index
 *        when cloning.
 *----------------------------------------------------------------------*/
ALTIA_BOOLEAN AltiaAnimateId(int nameId, AltiaEventType value)
{
    ALTIA_BOOLEAN animated = ALTIA_FALSE;
#ifdef ALTIA_TASKING
    /* If we're tasking and processing the queue, don't requeue.
    ** This preserves the order of animations when an animation in 
    ** the queue triggers secondary animations through connections,
    ** control code, stimulus, or timers.
    */
    if (ALTIA_FALSE == AltiaTaskRunning)
    {
        AltiaTaskElementType task;
        task.type = ALTIA_TASK_EVENT;
        task.anim = (ALTIA_UINT32)nameId;
        task.event = value;
        animated = TargetAltiaTaskQueue(&task);
    }
    else
#endif /* ALTIA_TASKING */
    {
        /*
         * Report to the user that this function was called if they want to know
         */
        if (AltiaReportFuncPtr != NULL)
        {
            ALTIA_CHAR * funcName = AltiaAnimationIdToName(nameId);
            if (funcName)
                (*AltiaReportFuncPtr)(funcName, value);
        }

        if (nameId >= 0)
        {


            /* Check if Animation ID is a function and execute if ALTIA_TRUE */
            if (nameId < test_funcIndex_count)
            {
                int i;
                ALTIA_INDEX index = (ALTIA_INDEX)nameId;
                int count = test_funcIndex[index].count;
                int first = test_funcIndex[index].firstIndex;

                Altia_Extent_type tempe;


                AltiaEventType origValue = test_curValue[index];

                /* Save the new value for this animation */
                test_curValue[index] = value;

                /* Process all the functions associated with this animation */
                for (i = 0; i < count; i++)
                {
                    ALTIA_INDEX findex, cindex;

                    ALTIA_UINT32 id;

                    ALTIA_BOOLEAN foundState = ALTIA_FALSE;
                    ALTIA_BYTE funcType = (ALTIA_BYTE)test_funcFIndexs[first + i].funcType;

                    findex = (ALTIA_INDEX)test_funcFIndexs[first + i].index;

                    /* We can't use 'value' anymore since it might have been
                     * changed while processing functions.  Reload the curent value
                     * for this animation.
                     */
                    value = test_curValue[index];

                    if (funcType == AltiaConnectionFunc)
                    {
                        /* We have connection animation */

                        continue;
                    }

                    if (funcType == AltiaTimerStartFunc)
                    {
                        /* We have animation start timer */

                        if (ALTIA_TIMER_STOPPED(findex) == 0)
                            continue;

                        if (_altiaEnabled(ALTIA_TIMER_STARTCMD(findex),
                                      AltiaAnimationIdToName(nameId), ALTIA_TIMER_STARTVAL(findex)))
                        {
                            /* we have meet the start enable condition */
                            ALTIA_SET_TIMER_STOPPED(findex,0);
                            altiaStartTimer(findex, ALTIA_TIMER_MSECS(findex));
                        }

                        continue;
                    }

                    if (funcType == AltiaTimerStopFunc)
                    {
                        /* We have animation stop timer*/

                        if (ALTIA_TIMER_STOPPED(findex))
                            continue;

                        if (_altiaEnabled(ALTIA_TIMER_STOPCMD(findex),
                                       AltiaAnimationIdToName(nameId), ALTIA_TIMER_STOPVAL(findex)))
                        {
                            /* we have meet the start enable condition */
                            ALTIA_SET_TIMER_STOPPED(findex,1);
                            altiaStopTimer(findex);
                        }

                        continue;
                    }

                    if (funcType == AltiaControlFunc)
                    {
                        /* We have a WHEN statement to call */

                        foundState = _altiaExecWhen(findex, index, -1);
                        if (foundState)
                            animated = ALTIA_TRUE;

                        continue;
                    }

                    /* Animation type */
                    cindex = ALTIA_FNAME_FIRST(findex);
                    switch(ALTIA_FNAME_ANIMATETYPE(findex))
                    {

                        case AltiaNormalAnimation:
                        {
                            ALTIA_INDEX objI = ALTIA_FNAME_OBJECT(findex);
                            if (index != ALTIA_DOBJ_CURRENTFUNC(objI) ||
                                !ALTIA_EQUAL(ALTIA_DOBJ_CURRENTVALUE(objI), value))
                            {
                                ALTIA_SET_DOBJ_CURRENTFUNC(objI,index);
                                ALTIA_SET_DOBJ_CURRENTVALUE(objI,value);
                                foundState = _altiaSetState(findex, value);
                            }
                            break;
                        }


                        case AltiaCustomFAnimation:
                            foundState = _altiaCustomFState(findex, value, &id);
                            if (foundState)
                            {
                                ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);
                                _altiaGetExtent(AltiaDynamicObject, obj, &tempe);

                                /* Pass the extent to custom animation to give
                                 * custom animation an opportunity to adjust it.
                                 */
                                foundState = _altiaCustomAnimation(findex, value, id, cindex, &tempe);
                                if (foundState)
                                {
                                    if (!_altiaIsHidden(obj, AltiaDynamicObject))
                                    {

                                        _altiaAddMyExtent(AltiaDynamicObject, obj, &tempe);
                                    }
                                }
                            }
                            break;


                        case AltiaCustomSAnimation:
                            foundState = _altiaCustomSState(findex, value, &id);
                            if (foundState)
                            {
                                ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);
                                _altiaGetExtent(AltiaDynamicObject, obj, &tempe);

                                /* Pass the extent to custom animation to give
                                 * custom animation an opportunity to adjust it.
                                 */
                                foundState = _altiaCustomAnimation(findex, value, id, cindex, &tempe);
                                if (foundState)
                                {
                                    if (!_altiaIsHidden(obj, AltiaDynamicObject))
                                    {

                                        _altiaAddMyExtent(AltiaDynamicObject, obj, &tempe);
                                    }
                                }
                            }
                            break;


                        case AltiaCustomIAnimation:
                            foundState = _altiaCustomIState(findex, value, &id);
                            if (foundState)
                            {
                                ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);
                                _altiaGetExtent(AltiaDynamicObject, obj, &tempe);

                                /* Pass the extent to custom animation to give
                                 * custom animation an opportunity to adjust it.
                                 */
                                foundState = _altiaCustomAnimation(findex, value, id, cindex, &tempe);
                                if (foundState)
                                {
                                    if (!_altiaIsHidden(obj, AltiaDynamicObject))
                                    {

                                        _altiaAddMyExtent(AltiaDynamicObject, obj, &tempe);
                                    }
                                }
                            }
                            break;


                        case AltiaDeckAnimation:
                            /* Change card only if card state is different */
                            if (ALTIA_ROUND(ALTIA_E2D(value))
                                != ALTIA_ROUND(ALTIA_E2D(origValue)))
                                foundState = _altiaSwitchCard(findex, value);
                            break;

                        case AltiaNoAnimation:
                        default:
                            break;
                    }

                    if (foundState)
                        animated = ALTIA_TRUE;
                }
            }




            if (nameId >= test_funcIndex_count)
            {
                /* Decrement ID by function count to create builtin table index */
                nameId -= test_funcIndex_count;
            }
            else
            {
                /* It's possible that this function is a builtin AND a custom animation.
                ** For this case we need to find the builtin ID.
                */
                nameId = altiaFindBuiltinId(test_funcIndex[nameId].name);
            }


            /* Check if Animation ID is a builtin and execute if ALTIA_TRUE */
            if (altiaExecBuiltinId(nameId, value))
                animated = ALTIA_TRUE;
        }

        /* We now execute any delayed whens created by control.  We must
         * wait until last to behave like Altia.
         */
        if (altiaInWhen == ALTIA_FALSE)
        {
            if (_altiaExecDelayedWhen())
                animated = ALTIA_TRUE;
        }

    }

    return animated;
}

#ifdef Altiafp
ALTIA_CHAR * AltiaAnimationIdToName(int nameId)
#else
ALTIA_CHAR * AltiaAnimationIdToName(nameId)
int nameId;
#endif
{

    if ((nameId >= 0) && (nameId < test_funcIndex_count))
        return test_funcIndex[nameId].name;

    /* Adjust name ID for builtins */
    nameId -= test_funcIndex_count;

    /* Last option is a builtin animation */
    return altiaFindBuiltinName((ALTIA_INDEX)nameId);
}

#ifdef Altiafp
int AltiaAnimationNameToId(ALTIA_CHAR *funcName)
#else
int AltiaAnimationNameToId(funcName)
ALTIA_CHAR *funcName;
#endif
{
    ALTIA_INDEX index = -1;

    index = _altiaFindAnimationIdx(funcName);

    if (index < 0)
    {
        /* See if animation name is a builtin */
        index = altiaFindBuiltinId(funcName);

        if (index >= 0)
        {
            /* Offset the builtin index by the function table count so we can
            ** find it again later and properly identify it as a builtin.
            */
            index += (ALTIA_INDEX)test_funcIndex_count;
        }

    }
    return index;
}


/*----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN AltiaAnimateText(ALTIA_CHAR *funcName,
                               ALTIA_CHAR *value)
#else
ALTIA_BOOLEAN AltiaAnimateText(funcName, value)
ALTIA_CHAR *funcName;
ALTIA_CHAR *value;
#endif
{
    ALTIA_BOOLEAN res = ALTIA_FALSE;
    ALTIA_INT i;
    ALTIA_INT len = (ALTIA_INT)ALT_STRLEN(value);

    int nameId = AltiaAnimationNameToId(funcName);
    if (nameId < 0)
    {
        /* Animation does not exist -- report out the characters
        ** if there's a callback report function, or if the User
        ** has requested callbacks for invalid animations.
        */
        if (AltiaReportFuncPtr || AltiaInvalidAnimationNamePtr)
        {
            for (i = 0; i < len; i++)
            {
                /* Need to keep from sign extending char to support
                 * chars in other languages.
                 */
                AltiaEventType eventValue = (AltiaEventType)(value[i] & ALT_CHARMASK);

                /* Reports */
                if (AltiaReportFuncPtr != NULL)
                    (*AltiaReportFuncPtr)(funcName, eventValue);
                if (AltiaInvalidAnimationNamePtr != NULL)
                        (AltiaInvalidAnimationNamePtr)(funcName, eventValue);
            }

            /* Reports */
            if (AltiaReportFuncPtr != NULL)
                (*AltiaReportFuncPtr)(funcName, (AltiaEventType)0);
            if (AltiaInvalidAnimationNamePtr != NULL)
                    (AltiaInvalidAnimationNamePtr)(funcName, (AltiaEventType)0);
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            /* Need to keep from sign extending char to support
             * chars in other languages.
             */
            res |= AltiaAnimateId(nameId, (AltiaEventType)(value[i] & ALT_CHARMASK));
        }
        res |= AltiaAnimateId(nameId, 0);
    }

    return res;
}
