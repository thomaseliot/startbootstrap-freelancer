/* $Revision: 1.51 $    $Date: 2009-12-14 18:21:46 $
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
#include <stdio.h>

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include <math.h>

#include "altiaData.h"
#include "altiaLibDraw.h"

extern void TargetAltiaClose(
#ifdef Altiafp
void
#endif
);


extern void _altiaPopAdjTrans(
#ifdef Altiafp
ALTIA_SHORT,
ALTIA_INDEX,
Altia_Transform_type *
#endif
);
extern void _altiaUnpopAdjTrans(
#ifdef Altiafp
ALTIA_SHORT,
ALTIA_INDEX,
Altia_Transform_type *
#endif
);






extern ALTIA_INDEX altiaFindObj(
#ifdef Altiafp
AltiaEventType,
ALTIA_SHORT *
#endif
);

extern void _altiaErrorOutput(
#ifdef Altiafp
ALTIA_CONST ALTIA_CHAR *
#endif
);




extern void _altiaGetLabelExtent(
#ifdef Altiafp
Altia_Label_type * label
#endif
);



extern ALTIA_BOOLEAN _altiaCustomSState(
#ifdef Altiafp
ALTIA_INDEX findex,
AltiaEventType value,
ALTIA_UINT32 *id
#endif
);






/* Builtin function type */
typedef ALTIA_BOOLEAN (*AltiaBuiltinFuncType)(AltiaEventType value);

typedef struct
{
    ALTIA_CHAR           *name;
    AltiaBuiltinFuncType func;
} Altia_Builtin_type;

typedef struct
{
    ALTIA_INDEX    parent;
    ALTIA_INDEX    childIdx;
    ALTIA_INDEX    idx;
    ALTIA_INDEX    inputIdx;
    ALTIA_SHORT    inputCnt;
    ALTIA_SHORT    otype;

    ALTIA_INDEX    pattern;
    ALTIA_INDEX    brush;
    ALTIA_COLOR    foreground;
    ALTIA_COLOR    background;
    Altia_Transform_type ptrans;

} Altia_Pop_type;

typedef struct
{
    ALTIA_INDEX inputIdx;
    ALTIA_INDEX idx;
    ALTIA_SHORT otype;
} Altia_PopInput_type;

#ifdef WIN32
#undef y0
#undef y1
#endif

#define ALTIA_MAX_COLOR     64
#define ALTIA_MAX_FONT      128

/* Used with altiaSetObj builtin */
static AltiaEventType _altiaCurId = 0;

/* Used with altiaCacheOutput builtin */
extern ALTIA_BOOLEAN _altiaOverrideUpdate;

/* Used with altiaEnableOnlyInput and altiaDisableOnlyInput builtins */
ALTIA_BOOLEAN altiaEnableOnly = ALTIA_FALSE;


/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN builtinSetFont(ALTIA_INDEX fontidx,
                             ALTIA_SHORT otype,
                             ALTIA_INDEX idx)
{
    ALTIA_BOOLEAN res = ALTIA_FALSE;
#ifdef ALTIA_SET_FONT_BASELINE_ADJUST
    ALTIA_INDEX oldFont = -1;
#endif /* ALTIA_SET_FONT_BASELINE_ADJUST */

    /* This function is only populated if the design contains labels */

    if (otype == AltiaDynamicObject)
    {

        /* Check if this is a group-type object.  Search children
        ** if this object has children.  If this object is a deck
        ** then search children of hidden cards.
        */
        if (AltiaChildDraw == ALTIA_DOBJ_DRAWTYPE(idx))
        {

            /* Parent object -- loop through and process children.
            ** if this is a deck, then check children for hidden
            ** cards.
            */
            int count = ALTIA_DOBJ_CHILDCNT(idx);
            ALTIA_INDEX first = ALTIA_DOBJ_FIRSTCHILD(idx);
            int i;
            ALTIA_SHORT ctype;
            ALTIA_INDEX child;
            ALTIA_BOOLEAN isDeck = ALTIA_FALSE;

            int j,k;
            int firstFunc = ALTIA_DOBJ_FIRSTFUNC(idx);
            int funcCnt = ALTIA_DOBJ_FUNCCNT(idx);

            /* Deck check */
            for (i = 0; i < funcCnt; i++)
            {
                if (AltiaDeckAnimation == ALTIA_FNAME_ANIMATETYPE(firstFunc + i))
                {
                    /* This is a deck */
                    int ffirst = ALTIA_FNAME_FIRST(firstFunc + i);
                    int fcnt = ALTIA_FNAME_COUNT(firstFunc + i);
                    isDeck = ALTIA_TRUE;

                    /* Process each card */
                    for (j = 0; j < fcnt; j++)
                    {
                        /* process all the children for this card */
                        int dfirst = (int)test_decks[ffirst+j].firstChild;
                        int dcnt = (int)test_decks[ffirst+j].childCnt;
                        for (k = 0; k < dcnt; k++)
                        {
                            child = (ALTIA_INDEX)test_children[dfirst+k].child;
                            ctype = (ALTIA_SHORT)test_children[dfirst+k].type;
                            if (child >= 0)
                            {
                                if (ALTIA_TRUE == builtinSetFont(fontidx, ctype, child))
                                    res = ALTIA_TRUE;
                            }
                        }
                    }

                    /* Done with the deck -- break out of the animation loop */
                    break;
                }
            }

            if (ALTIA_FALSE == isDeck)
            {
                for (i = 0; i < count; i++)
                {
                    ctype = (ALTIA_SHORT)test_children[first + i].type;
                    child = (ALTIA_INDEX)test_children[first + i].child;
                    if (child >= 0)
                    {
                        if (ALTIA_TRUE == builtinSetFont(fontidx, ctype, child))
                            res = ALTIA_TRUE;
                    }
                }
            }

            /* Add old extent */
            if (ALTIA_TRUE == res)
                _altiaAddExtent(otype, idx);

        }
        else
        {
            ALTIA_INDEX drawidx = ALTIA_DOBJ_DRAWINDEX(idx);
            ALTIA_INDEX labelidx;

            /* Process label data if it is applicable to this object.
            ** Applicable objects are:
            **
            ** AltiaLabelDraw
            ** AltiaTextioDraw
            ** AltiaMLineTextDraw
            ** AltiaTickDraw (contains font index)
            ** AltiaPieDraw (contains font index)
            */
            switch (ALTIA_DOBJ_DRAWTYPE(idx))
            {
                case AltiaLabelDraw:
                    /* Add old extent */
                    _altiaAddExtent(otype, idx);

                    /* Change font */
                    labelidx = drawidx;
#ifdef ALTIA_SET_FONT_BASELINE_ADJUST
                    oldFont = test_labels[labelidx].font;
#endif
                    test_labels[labelidx].font = fontidx;

                    /* Calculate new extent */
                    _altiaGetLabelExtent(&test_labels[labelidx]);
                    break;

                case AltiaTextioDraw:
                    /* Add old extent */
                    _altiaAddExtent(otype, idx);

                    /* Change font */
                    labelidx = ALTIA_TEXT_LABELINDEX(drawidx);
#ifdef ALTIA_SET_FONT_BASELINE_ADJUST
                    oldFont = test_labels[labelidx].font;
#endif
                    test_labels[labelidx].font = fontidx;

                    /* Calculate new extent */
                    _altiaGetLabelExtent(&test_labels[labelidx]);
                    break;




                default:
                    return ALTIA_FALSE;
            }

            /* We've updated this object */
            res = ALTIA_TRUE;
        }

    }
    else
    {


        /* No child search for static objects -- just process labels */
        if (AltiaLabelDraw == ALTIA_SOBJ_DRAWTYPE(idx))
        {
            ALTIA_INDEX labelidx;

            /* Add old extent */
            _altiaAddExtent(otype, idx);

            /* change the font for this label */
            labelidx = ALTIA_SOBJ_DRAWINDEX(idx);
#ifdef ALTIA_SET_FONT_BASELINE_ADJUST
            oldFont = test_labels[labelidx].font;
#endif
            test_labels[labelidx].font = fontidx;

            /* Calculate new extent */
            _altiaGetLabelExtent(&test_labels[labelidx]);

            /* We've updated this object */
            res = ALTIA_TRUE;
        }


    }

    /* Process extents if this object was updated*/
    if (ALTIA_TRUE == res)
    {
        Altia_Extent_type extent;

#ifdef ALTIA_SET_FONT_BASELINE_ADJUST
        /* Shift the text so the baseline remains the same */
        if (oldFont >= 0)
        {
            ALTIA_DOUBLE dy = FSUB(ALTIA_I2F(test_fonts[oldFont].descent),
                                   ALTIA_I2F(test_fonts[fontidx].descent));
            if (dy != 0)
            {
                /* Move Object */
                if (otype == AltiaDynamicObject)
                {

                    altiaLibTranslate(ALTIA_DOBJ_TRANS_ADDR(idx), (ALTIA_DOUBLE)0, dy);

                    /* lastTrans is now a last non-behavior transform */
                    altiaLibTranslate(ALTIA_DOBJ_LASTTRANS_ADDR(idx), (ALTIA_DOUBLE)0, dy);

                }


                else
                {
                    Altia_Transform_type strans;
                    _altiaBldTrans(ALTIA_SOBJ_BASETRANS(idx), -1, &strans);

                    /* We have to copy the otrans since we don't have an offset
                     * since we can change it here so it can't be shared
                     */
                    strans.a20     = ALTIA_SOBJ_OTRANS(idx).a20;
                    strans.a21     = ALTIA_SOBJ_OTRANS(idx).a21;

                    strans.xoffset = ALTIA_SOBJ_OTRANS(idx).xoffset;
                    strans.yoffset = ALTIA_SOBJ_OTRANS(idx).yoffset;


                    altiaLibTranslate(&strans, (ALTIA_DOUBLE)0, dy);

                    ALTIA_SOBJ_OTRANS(idx).a20     = strans.a20;
                    ALTIA_SOBJ_OTRANS(idx).a21     = strans.a21;

                    ALTIA_SOBJ_OTRANS(idx).xoffset = strans.xoffset;
                    ALTIA_SOBJ_OTRANS(idx).yoffset = strans.yoffset;

                }


            }
        }
#endif /* ALTIA_SET_FONT_BASELINE_ADJUST */

        /* Calculate new extent */
        _altiaTotalTrans(otype, idx);

        _altiaPushTrans(otype, idx);

        _altiaFindExtent(otype, idx, &extent);

        /* Update parent extent */

        _altiaUpdateParentExtent(otype, idx);

        /* Add new extent */
        _altiaAddExtent(otype, idx);
    }

    return res;
}

/*----------------------------------------------------------------------*/
static void altiaCheckLastEnableInput(void)
{

}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaCacheOutputBuiltin(AltiaEventType value)
{
    if (value == 0)
    {
       /* Turn caching off which should also force an update.  Note
        * that we don't have to do an explicit update. Since we are in
        * the middle of an animate, it will get done for us if we return
        * ALTIA_TRUE and updating is on.  This allows application code to
        * continue to have updating off through the rest of the event
        * routing if routing was initiated by application code and it
        * has its own caching enabled.
        */
        _altiaOverrideUpdate = ALTIA_FALSE;
        return ALTIA_TRUE;
    }

    _altiaOverrideUpdate = ALTIA_TRUE;
    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaErrorOutput(AltiaEventType value)
{
    static int textbufindex = 0;
    
    static int textbufsize = 64;
    static ALTIA_CHAR textbuf[64];
    
    

    if (textbufindex >= textbufsize)
    {
        
        textbuf[textbufsize - 1] = '\0';
        
        
    }
    else
        textbuf[textbufindex++] = (ALTIA_CHAR) value;
    

    if (value == 0 && textbuf != nil)
    {
        _altiaErrorOutput(textbuf);
        textbufindex = 0;
        
    }

    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaDisableOnlyInput(AltiaEventType value)
{

    return ALTIA_FALSE; /* no animation to draw */
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaEnableOnlyInput(AltiaEventType value)
{

    return ALTIA_FALSE;  
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaGetObjXY(AltiaEventType value)
{
    ALTIA_INDEX idx;
    ALTIA_SHORT otype;
    AltiaEventType x = (AltiaEventType) 0;
    AltiaEventType y = (AltiaEventType) 0;
    ALTIA_BOOLEAN res;
    Altia_Extent_type * eptr;

    if (value == 0)
    {
        return ALTIA_FALSE;
    }

    idx = altiaFindObj(value, &otype);
    if (idx == -1)
    {
        return ALTIA_FALSE;
    }

    eptr = altiaGetCurrentExtent(idx, otype);
    if (NULL != eptr)
    {
        x = eptr->x0;
        y = eptr->y0;
    }

    /* convert the offset  to altia world coordinates */
    x += test_window_xoffset;
    y += test_window_yoffset;

    res = AltiaAnimate(ALT_TEXT("altiaObjX"), x);
    res |= AltiaAnimate(ALT_TEXT("altiaObjY"), y);

    return res;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaGetObjWH(AltiaEventType value)
{
    ALTIA_INDEX idx;
    ALTIA_SHORT otype;
    AltiaEventType width = (AltiaEventType) 0;
    AltiaEventType height = (AltiaEventType) 0;
    ALTIA_BOOLEAN res;
    Altia_Extent_type * eptr;

    if (value == 0)
    {
        return ALTIA_FALSE;
    }

    idx = altiaFindObj(value, &otype);
    if (idx == -1)
    {
        return ALTIA_FALSE;
    }

    eptr = altiaGetCurrentExtent(idx, otype);
    if (NULL != eptr)
    {
        /* Note: do not add 1 to extent to get dimensions.  This makes it
         * consistent with Altia not adding 1.  Reported size for images
         * will be exact, vector objects will be reported 1 pixel too small.
         */
        width  = (AltiaEventType)(eptr->x1 - eptr->x0);
        height = (AltiaEventType)(eptr->y1 - eptr->y0);
    }

    res = AltiaAnimate(ALT_TEXT("altiaObjW"), width);
    res |= AltiaAnimate(ALT_TEXT("altiaObjH"), height);

    return res;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaQuit(AltiaEventType value)
{
    TargetAltiaClose();
    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaImageObj(AltiaEventType value)
{
    ALTIA_BOOLEAN res = ALTIA_FALSE;

    /* This function is only populated if the design contains image objects */

    return res;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN altiaSetObj(AltiaEventType value)
{
    _altiaCurId = value;
    return ALTIA_FALSE;
}


/* Default Builtin Table - These builtins are available even
 * if builtins is not checked in the Codegen Option Dialog.
 *
 * IMPORTANT:
 * The builtins list must be sorted by animation name and
 * the count must be correct!
 */
ALTIA_CONST ALTIA_INT Altia_builtins_count = 9;
ALTIA_CONST Altia_Builtin_type Altia_builtins[] =
{
    { ALT_TEXT("altiaCacheOutput"),      altiaCacheOutputBuiltin },
    { ALT_TEXT("altiaDisableOnlyInput"), altiaDisableOnlyInput   },
    { ALT_TEXT("altiaEnableOnlyInput"),  altiaEnableOnlyInput    },
    { ALT_TEXT("altiaErrorOutput"),      altiaErrorOutput        },
    { ALT_TEXT("altiaGetObjWH"),         altiaGetObjWH           },
    { ALT_TEXT("altiaGetObjXY"),         altiaGetObjXY           },
    { ALT_TEXT("altiaQuit"),             altiaQuit               },
    { ALT_TEXT("altiaSetImage"),         altiaImageObj           },
    { ALT_TEXT("altiaSetObj"),           altiaSetObj             } 
};

/*----------------------------------------------------------------------*/
ALTIA_INDEX altiaFindBuiltinId(ALTIA_CHAR *funcName)
{
    ALTIA_BOOLEAN done = ALTIA_FALSE;
    ALTIA_INDEX low = 0;
    ALTIA_INDEX high = Altia_builtins_count - 1;
    ALTIA_INDEX index = high / 2;
    int res;

    while (done == ALTIA_FALSE)
    {
        res = ALT_STRCMP(funcName, Altia_builtins[index].name);
        if (res == 0)
        {
            break;
        }else if (res < 0)
            high = index - 1;
        else
            low = index + 1;
        if (low > high)
            done = ALTIA_TRUE;
        else
            index = (high + low)/2;
    }

    if (done == ALTIA_TRUE)
        return -1;

    return index;
}

/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaCheckBuiltins(ALTIA_CHAR *funcName,
                                 AltiaEventType value)
{
    ALTIA_BOOLEAN result = ALTIA_FALSE;
    AltiaBuiltinFuncType fptr;
    ALTIA_INDEX idx;

    idx = altiaFindBuiltinId(funcName);
    if (idx != -1)
    {
        fptr = Altia_builtins[idx].func;
        if (fptr != NULL)
            result = (*fptr)(value);
    }

    return result;
}

ALTIA_BOOLEAN altiaExecBuiltinId(int nameId, AltiaEventType value)
{
    ALTIA_BOOLEAN result = ALTIA_FALSE;
    AltiaBuiltinFuncType fptr;

    if ((nameId >= 0) && (nameId < Altia_builtins_count))
    {
        fptr = Altia_builtins[nameId].func;
        if (fptr != NULL)
            result = (*fptr)(value);
    }

    return result;
}

/*----------------------------------------------------------------------*/
ALTIA_CHAR * altiaFindBuiltinName(ALTIA_INDEX nameId)
{
    if ((nameId >= 0) && (nameId < Altia_builtins_count))
        return Altia_builtins[nameId].name;

    return NULL;
}
