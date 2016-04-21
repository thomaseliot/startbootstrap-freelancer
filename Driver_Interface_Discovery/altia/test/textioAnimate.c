/* $Revision: 1.65 $    $Date: 2010-12-13 00:38:47 $
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



/* 9/14/01:  To support integration with MFC applications which use
 * multi-byte characters, include tchar.h on WIN32 targets (except Linux
 * Microwindows) to resolve character conversion routines (e.g., isspace).
 * On all other targets, include cytpe.h.
 */
#if defined(WIN32) && !defined(MICROWIN)
#include <tchar.h>
#endif
#if !defined(WIN32) || !defined(MFCAPP)
#include <ctype.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaData.h"

#define CHARACTER_ID 1
#define TEXT_ID (1<<1)
#define JUSTIFY_ID (1<<2)
#define MAX_CHAR_ID (1<<3)
#define APPEND_ID (1<<4)
#define CLEAR_CHAR_ID (1<<5)
#define INT_BASE_ID (1<<6)
#define INTEGER_ID (1<<7)
#define CLIP_ID (1<<8)
#define SCROLL_ON_ID (1<<9)
#define SCROLL_ID (1<<10)
#define CURSOR_ID (1<<11)
#define CURSOR_POS_ID (1<<12)
#define FLOAT_ID (1<<13)
#define DECIMAL_ID (1<<14)
#define SHORTCUT_ON_ID (1<<15)
#define JUMP_ON_ID (1<<16)
#define SELECT_ON_ID (1<<17)
#define LENGTH_MODE_ID (1<<18)
#define LENGTH_ID (1<<19)
#define SELECT_NOW_ID (1<<20)
#define IO_HLCOLOR_ID (1<<21) 
#define MAX_PIXEL_ID  (1<<22)

#define textio_min(x1,x2) ((x1) > (x2))? (x2):(x1)
#define textio_max(x1,x2) ((x1) > (x2))? (x1):(x2)

enum TextJustifyType {
    LEFT_JUSTIFY, RIGHT_JUSTIFY, RIGHT_JUSTIFY_ZERO, CENTER_JUSTIFY,
    LEFT_SIB_JUSTIFY, RIGHT_SIB_JUSTIFY, CENTER_SIB_JUSTIFY,
    RIGHT_SIB_CLIP_LEFT_JUSTIFY
};
/* Set this define to the number of above enums -1
 */
#define MAX_JUSTIFY 7

#define ZIP_FIRST_CHAR -10000
#define ZIP_LAST_CHAR 10000

/* Set the maximum length for a highlight color string */
#define MAX_COLOR_LEN 25 


enum IntegerBaseType {
    DECIMAL_TYPE, HEX_TYPE, OCTAL_TYPE, FLOAT_TYPE, EFLOAT_TYPE
};

/* Set this define to the number of above enums -1
 */
#define MAX_BASE 4

enum CursorSet {
    NO_CURSOR, BUILTIN_LEFT, BUILTIN_POS, BUILTIN_RIGHT, USER_LEFT,
    USER_POS, USER_RIGHT, BUILTIN_HOME, BUILTIN_END
};

enum SelectType {
    NO_SELECT, SELECT_CHAR, SELECT_WORD, SELECT_HOME, SELECT_END,
    SELECT_ALL_HOME, SELECT_ALL_END, SELECT_MOUSE
};

enum SelectNowType {
    SEL_NOW_NONE, SEL_NOW_MOUSE, SEL_NOW_ALL_HOME, SEL_NOW_ALL_END
};

enum LengthType {
    NO_LENGTH, DISPLAYEDP, ACTUALP, DISPLAYEDC, ACTUALC
};

extern ALTIA_INT AltiaFloatExtent;
extern ALTIA_FLOAT AltiaExtentX0;
extern ALTIA_FLOAT AltiaExtentY0;
extern ALTIA_FLOAT AltiaExtentX1;
extern ALTIA_FLOAT AltiaExtentY1;

extern int altiaLibFontWidth(
#ifdef Altiafp
Altia_Font_type *font,
ALTIA_CHAR s
#endif
);


extern ALTIA_INDEX _altiaGetCustomIFuncIndex(
#ifdef Altiafp
ALTIA_UINT32 id,
ALTIA_INDEX first,
ALTIA_SHORT count
#endif
);



extern ALTIA_INDEX _altiaGetCustomFIndex(
#ifdef Altiafp
ALTIA_UINT32 id,
ALTIA_INDEX first,
ALTIA_SHORT count
#endif
);



extern ALTIA_INDEX _altiaGetCustomFFuncIndex(
#ifdef Altiafp
ALTIA_UINT32 id,
ALTIA_INDEX first,
ALTIA_SHORT count
#endif
);



extern ALTIA_INDEX _altiaGetCustomSIndex(
#ifdef Altiafp
ALTIA_UINT32 id,
ALTIA_INDEX first,
ALTIA_SHORT count
#endif
);

extern ALTIA_INDEX _altiaGetCustomSFuncIndex(
#ifdef Altiafp
ALTIA_UINT32 id,
ALTIA_INDEX first,
ALTIA_SHORT count
#endif
);



extern ALTIA_BOOLEAN altiaLibIsNumber(
#ifdef Altiafp
ALTIA_CHAR *,
AltiaEventType *
#endif
);

extern ALTIA_BOOLEAN altiaLibIsInteger(
#ifdef Altiafp
ALTIA_CHAR *,
int *
#endif
);

extern void _altiaLibRectDraw(
#ifdef Altiafp
ALTIA_WINDOW win,
ALTIA_UINT32 dc,
Altia_GraphicState_type *gs,
ALTIA_CONST Altia_Rect_type *rect,
ALTIA_BYTE filled,
Altia_Transform_type *total
#endif
);




/*----------------------------------------------------------------------*/
ALTIA_INDEX _altiaGetSibling(ALTIA_INDEX parent, ALTIA_INDEX dobj)
{
    /* We want the prev siblings extent to justify on instead of 
    ** the parent.
    */
    ALTIA_INDEX prev = -1;
    ALTIA_INDEX first;
    int i, cnt;

    /* Find sibling if parent is a group or deck */
    if (AltiaChildDraw == ALTIA_DOBJ_DRAWTYPE(parent))
    {
        /* Check if this parent is a deck */
        ALTIA_INDEX firstFunc = ALTIA_DOBJ_FIRSTFUNC(parent);
        int funcCnt = ALTIA_DOBJ_FUNCCNT(parent);
        for (i = 0; i < funcCnt; i++)
        {
            if (AltiaDeckAnimation == ALTIA_FNAME_ANIMATETYPE(firstFunc + i))
            {
                /* This is a deck */
                int j;
                ALTIA_INDEX firstName = ALTIA_FNAME_FIRST(firstFunc + i);
                int nameCnt = ALTIA_FNAME_COUNT(firstFunc + i);

                /* Process each card to find this textio */
                for (j = 0; j < nameCnt; j++)
                {
                    /* process all the children for this card */
                    int k;
                    cnt = test_decks[firstName + j].childCnt;
                    first = test_decks[firstName + j].firstChild;
                    for (k = 0; k < cnt; k++)
                    {
                        ALTIA_INDEX child = first + k;

                        /* Child index is -1 if child is popped to top. */
                        if ((ALTIA_INDEX)test_children[child].child == dobj &&
                            (ALTIA_SHORT)test_children[child].type  == AltiaDynamicObject &&
                            (ALTIA_INDEX)test_children[child].child != -1)
                        {
                            /* Found the card containing the textio */
                            if (k > 0)
                                return (child - 1);
                            else if ((k + 1) < cnt)
                                return (child + 1);

                            /* Single object card with only this textio */
                            return -1;
                        }
                    }
                }

                /* Done with the deck -- failed to find textio */
                return -1;
            }
        }
    }


    {
        /* Children of parent group */
        first = ALTIA_DOBJ_FIRSTCHILD(parent);
        cnt = ALTIA_DOBJ_CHILDCNT(parent);
    }

    /* Get group children */
    prev = first;
    for (i = 0; i < cnt; i++)
    {
        ALTIA_INDEX child = first + i;

        /* Child index is -1 if child is popped to top. */
        if ((ALTIA_INDEX)test_children[child].child == dobj &&
            (ALTIA_SHORT)test_children[child].type  == AltiaDynamicObject && 
            (ALTIA_INDEX)test_children[child].child != -1)
        {
            if (prev == child)
            {
                /* We have to look at the next child since our object is the first */
                if ((i + 1) < cnt)
                {
                    /* We have a next child */
                    prev = first + i + 1;
                }
                else
                    prev = -1;
            }
            break;
        }
        prev = child;
    }

    return prev;
}




#define TEXTBUFSIZE 64
static ALTIA_CHAR textbuf[TEXTBUFSIZE];



/*----------------------------------------------------------------------
 * Note that _checkSize actually checks to make sure there is space
 * for len + 1 so caller does not need to take into account the end of
 * string character in len. 
 *----------------------------------------------------------------------*/
#ifdef Altiafp
static ALTIA_BOOLEAN _checkSize(Altia_CustomSValue_type *custSPtr,
                                int len)
#else
static ALTIA_BOOLEAN _checkSize(custSPtr, len)
Altia_CustomSValue_type *custSPtr;
int len;
#endif
{

    if (len >= 64)
        return ALTIA_FALSE;
    else
        return ALTIA_TRUE;


}

/*----------------------------------------------------------------------*/
static void _lineAppend(ALTIA_INDEX textio,
                        Altia_CustomSValue_type *custSPtr,
                        int value)
{
    if (custSPtr->index == 0) {
        ALTIA_SET_TEXT_START(textio,0); /* starting over */
        ALTIA_SET_TEXT_SELECTSTART(textio,0);
        ALTIA_SET_TEXT_SELECTEND(textio,0);
    }
    /* Check if there is room for current length + 1 */
    if (!_checkSize(custSPtr, custSPtr->index + 1))
        return;
    custSPtr->value[custSPtr->index++] = value;
    custSPtr->value[custSPtr->index] = '\0';
}

/*----------------------------------------------------------------------*/
static void _lineAppendVal(ALTIA_INDEX textio,
                           Altia_CustomSValue_type *custSPtr,
                           AltiaEventType rval,
                           int btype,
                           int decPts)
{
    ALTIA_CHAR format[64];
    ALTIA_CHAR ch;
    int len;

    /* For fixed point, event value is an integer so no rounding.
     * ALTIA_ROUND(ALTIA_E2D(rval)) would only truncate the value
     * resulting in unexpected different behavior for fixed point.
     */
    int ival = (int) rval;



    if (custSPtr->index == 0)
    {
        ALTIA_SET_TEXT_START(textio,0); /* starting over */
        ALTIA_SET_TEXT_SELECTSTART(textio,0);
        ALTIA_SET_TEXT_SELECTEND(textio,0);
    }

    switch (btype)
    {
#ifndef ALTIA_NO_DOUBLE
    case EFLOAT_TYPE:
        ch = 'e';
        break;
    case FLOAT_TYPE:
        ch = 'f';
        break;
#endif /* ALTIA_NO_DOUBLE */
    case DECIMAL_TYPE:
        ch = 'd';
        break;
    case HEX_TYPE:
        ch = 'x';
        break;
    case OCTAL_TYPE:
        ch = 'o';
        break;
    default:
        ch = 'd';
        break;
    }

    /* To show a single zero in non float mode decPts has to be at least
    ** a 1.
    */
    if (btype != FLOAT_TYPE && btype != EFLOAT_TYPE && decPts == 0)
        decPts = 1;
    if (decPts >= 0)
        ALT_SPRINTF(format, ALT_TEXT("%%.%d%c"), decPts, ch);
    else
        ALT_SPRINTF(format, ALT_TEXT("%%%c"), ch);
#ifndef ALTIA_NO_DOUBLE
    if (btype == FLOAT_TYPE || btype == EFLOAT_TYPE)
        ALT_SPRINTF(textbuf,format,(double)rval);
    else
#endif /* ALTIA_NO_DOUBLE */
        ALT_SPRINTF(textbuf,format,ival);
    len = ALT_STRLEN(textbuf);

    /* Check if there is room for current length + additional length */
    if (!_checkSize(custSPtr, custSPtr->index + len))
        return;
    ALT_STRCPY(&custSPtr->value[custSPtr->index], textbuf);
    custSPtr->index += len;
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
static void _lineSetIndex(Altia_CustomSValue_type *custSPtr,
                          int index)
#else
static void _lineSetIndex(custSPtr, index)
Altia_CustomSValue_type *custSPtr;
int index;
#endif
{
    if (index < 0)
        index = 0;
    else
    {
        int len = ALT_STRLEN(custSPtr->value);
        if (index > len)
            index = len;
    }
    custSPtr->index = index;
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
static void _lineBackup(Altia_CustomSValue_type *custSPtr)
#else
static void _lineBackup(custSPtr)
Altia_CustomSValue_type *custSPtr;
#endif
{
    custSPtr->index--;
    if (custSPtr->index >= 0)
    {

        custSPtr->value[custSPtr->index] = '\0';
    }
    else
        custSPtr->index = 0;
}

/*----------------------------------------------------------------------*/
static int _lineSize(ALTIA_INDEX textio,
                     ALTIA_CHAR * text,
                     int maxcount)
{
    if (maxcount > 0)
        return maxcount;
    else
        return ALT_STRLEN(&text[ALTIA_TEXT_START(textio)]);
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
static int _lineTotalSize(Altia_CustomSValue_type *custSPtr)
#else
static int _lineTotalSize(custSPtr)
Altia_CustomSValue_type *custSPtr;
#endif
{
    return ALT_STRLEN(&custSPtr->value[0]);
}

/*----------------------------------------------------------------------*/
static void _lineSetStart(ALTIA_INDEX textio,
                          Altia_CustomSValue_type *custSPtr,
                          int val)
{
    int len = ALT_STRLEN(custSPtr->value);
    if (len == 0)
    {
        ALTIA_SET_TEXT_START(textio,0);
    }
    if (val <= 0)
    {
        ALTIA_SET_TEXT_START(textio,0);
    }
    else if (val > len - 1)
    {
        ALTIA_SET_TEXT_START(textio,len - 1);
    }
    else
    {
        ALTIA_SET_TEXT_START(textio,val);
    }
}

/*----------------------------------------------------------------------*/
static void _lineSetSelectStart(ALTIA_INDEX textio,
                                Altia_CustomSValue_type *custSPtr, int val)
{
    int len = ALT_STRLEN(custSPtr->value);
    if (len == 0)
    {
        ALTIA_SET_TEXT_SELECTSTART(textio,0);
    }
    else if (val < 0)
    {
        ALTIA_SET_TEXT_SELECTSTART(textio,0);
    }
    else if (val > len)
    {
        ALTIA_SET_TEXT_SELECTSTART(textio,len);
    }
    else
    {
        ALTIA_SET_TEXT_SELECTSTART(textio,val);
    }
}

/*----------------------------------------------------------------------*/
static void _lineSetSelectEnd(ALTIA_INDEX textio,
                              Altia_CustomSValue_type *custSPtr, int val)
{
    int len = ALT_STRLEN(custSPtr->value);
    if (len == 0)
    {
        ALTIA_SET_TEXT_SELECTEND(textio,0);
    }
    else if (val < 0)
    {
        ALTIA_SET_TEXT_SELECTEND(textio,0);
    }
    else if (val > len)
    {
        ALTIA_SET_TEXT_SELECTEND(textio,len);
    }
    else
    {
        ALTIA_SET_TEXT_SELECTEND(textio,val);
    }
}

/*----------------------------------------------------------------------*/
static void _lineDeleteSelection(ALTIA_INDEX textio,
                                 Altia_CustomSValue_type *custSPtr)
{
    int s1, s2, rem;
    s1 = textio_min(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio));
    s2 = textio_max(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio));
    if (s1 == s2)
        return;
    rem = ALT_STRLEN(&custSPtr->value[s2]);

    if (rem == 0)
        custSPtr->value[s1] = 0;
    else
        memmove(&custSPtr->value[s1], &custSPtr->value[s2],
            (rem+1)*sizeof(ALTIA_CHAR));
    ALTIA_SET_TEXT_SELECTEND(textio,s1);
    ALTIA_SET_TEXT_SELECTSTART(textio,s1);
    custSPtr->index = s1;

    /* If start is now out of range, fix it. */
    if (ALTIA_TEXT_START(textio) > (s1 + rem))
        ALTIA_SET_TEXT_START(textio,s1 + rem);
}

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_lineGetSelection(ALTIA_INDEX textio,
                                     Altia_CustomSValue_type *custSPtr)
{
    int s1, s2;
    s1 = textio_min(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio));
    s2 = textio_max(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio));
    if (s1 == s2) {
        return ALT_TEXT("");
    }

    if ((s2-s1) >= TEXTBUFSIZE)
    {
        ALT_STRNCPY(textbuf, &custSPtr->value[s1], TEXTBUFSIZE - 1);
        textbuf[TEXTBUFSIZE - 1] = '\0';
    }
    else
    {
        ALT_STRNCPY(textbuf, &custSPtr->value[s1], s2-s1);
        textbuf[s2-s1] = '\0';
    }
    return textbuf;
}

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_lineGetConstrainString(ALTIA_INDEX dobj,
                                           ALTIA_INDEX textio,
                                           ALTIA_CHAR *src,
                                           int max,
                                           ALTIA_BOOLEAN clip,
                                           int justify,
                                           int pwidth,
                                           int *len)
{
    int parentWidth;
    int width;
    int fullLen;
    Altia_Extent_type pextent;
    ALTIA_INDEX fontIndex = 0;
    ALTIA_BOOLEAN fixed = ALTIA_FALSE;
    ALTIA_BOOLEAN rotated = ALTIA_FALSE;
    ALTIA_BOOLEAN rotated90 = ALTIA_FALSE;
    ALTIA_BOOLEAN fullString = justify == CENTER_JUSTIFY ||
                               justify == RIGHT_JUSTIFY ||
                               justify == CENTER_SIB_JUSTIFY ||
                               justify == RIGHT_SIB_JUSTIFY ||
                               justify == LEFT_SIB_JUSTIFY ||
                               justify == RIGHT_SIB_CLIP_LEFT_JUSTIFY;
    ALTIA_BOOLEAN leftClip = (justify == RIGHT_SIB_CLIP_LEFT_JUSTIFY)
                             && ((pwidth > 0) || (!max && clip));

    fullLen = *len = ALT_STRLEN(&src[ALTIA_TEXT_START(textio)]);
    if (*len <= 0)
        return nil;

    /* There is a hierarchy in animation processing.  The order is:
    **
    ** 1. max_pixel_count > 0
    ** 2. max_char_count > 0
    ** 3. clip_on != 0
    **
    ** As soon as one of those checks is ALTIA_TRUE, the remaining checks
    ** are skipped.  So (max_pixel_count != 0) will override the 
    ** other two animations.
    **/
    if (pwidth > 0)
    {
        /* We are going to determine the number of characters that will
        ** fit into the specified pixel width.  We are using the font's 
        ** width functions since they are more accurate then the getExtent 
        ** call for a single char (ie getExtent adds a +1 fudge factor for 
        ** scaling).
        */
        int h, offset;
        ALTIA_CHAR * pstr = &src[ALTIA_TEXT_START(textio)];
        fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
        fixed = ALTIA_TRUE;

        /* Calculate full-string pixel width */
        altiaLibFontExtent(&test_fonts[fontIndex], 
                           &src[ALTIA_TEXT_START(textio)], 
                           *len, &width, &h, &offset);

        /* Trim characters to obtain constrained string */
        if (leftClip)
        {
            /* Remove text from left side until it fits, or
            ** until we have only one character left.
            */
            while(width >= pwidth)
            {
                if (1 == *len)
                    break;
                else
                {
                    pstr++;
                    *len = *len - 1;
                }
                /* We shaved off one character from left side. */
                /* We always calculate the total width of the
                ** whole string and check to see if this is longer
                ** than max_pixel_width.
                */
                altiaLibFontExtent(&test_fonts[fontIndex], 
                                   pstr, *len, &width, &h, &offset);
            }
        }
        else
        {
            /* Remove text from right side until it fits, or
            ** until we have only one character left.
            */
            while(width >= pwidth)
            {
                if (1 == *len)
                    break;
                else
                    *len = *len - 1;
                /* We shaved off one character from right side. */
                /* We always calculate the total width of the
                ** whole string and check to see if this is longer
                ** than max_pixel_width.
                */
                altiaLibFontExtent(&test_fonts[fontIndex], 
                                   pstr, *len, &width, &h, &offset);
            }
        }
    }
    else if (max > 0)
    {
        fixed = ALTIA_TRUE;
        if (*len > max)
            *len = max;
    }
    else if (clip == ALTIA_FALSE)
    {
        fixed = ALTIA_TRUE;
    }
    else
    {
        ALTIA_INDEX parent = ALTIA_DOBJ_PARENT(dobj);
        if ((parent == -1) || (ALTIA_DOBJ_DRAWTYPE(parent) == AltiaClipDraw)
                           || (ALTIA_DOBJ_DRAWTYPE(parent) == AltiaAlphaMaskDraw))
        {
            /* If parent is a clip or mask object, it will do the clipping
             * or if no parent then no clipping
             */
            fixed = ALTIA_TRUE;
        }
        else
        {
            int newlen;

            _altiaGetExtent(AltiaDynamicObject, parent, &pextent);
            fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
            rotated = ALTIA_ROTATED(ALTIA_DOBJ_TOTAL_ADDR(dobj));
            rotated90 = ALTIA_ROTATED90(ALTIA_DOBJ_TOTAL_ADDR(dobj));
            if (!rotated)
            {
                ALTIA_COORD xdist, ydist;
                /* We are going to determine the number of characters that will
                 * fit into the parent.  We are using the font's width functions
                 * since they are more accurate then the getExtent call for a
                 * single char(ie getExtent adds a +1 fudge factor for scaling).
                 * We also need to take into account how far our object is 
                 * offset in relation to the parents origin.
                 */
                if (altiaLibIsFixedWidthFont(&test_fonts[fontIndex]))
                {
                    fixed = ALTIA_TRUE;
                    /* Use library FontWidth function to get the width
                     * of a single character
                     */
                    width = altiaLibFontWidth(&test_fonts[fontIndex], 
                        (ALTIA_CHAR)'M');
                }else
                {
                    /* Use library FontWidth function to get the width
                     * of a single character
                     */
                    width = altiaLibFontWidth(&test_fonts[fontIndex], 
                        (ALTIA_CHAR)'i');
                }
                altiaLibTransformDist(ALTIA_DOBJ_TOTAL_ADDR(dobj), width, 0, 
                    &xdist, &ydist);
                width = xdist;

                if (width <= 0)
                    fixed = ALTIA_TRUE;
                else
                {
                    if (fullString)
                    {
                        /* Since we will be moving the text the len
                         * is the number we can fit into the parent
                         */
                        newlen = (int)((pextent.x1 - pextent.x0 -1) / width);
                    }
                    else
                    {
                        /* We need to know where the text starts,
                         * since that is all we use its ok to get the
                         * current extent without computing a new one.
                         */
                        Altia_Extent_type extent;
                        _altiaGetExtent(AltiaDynamicObject, dobj, &extent);

                        /* Then we subtract 1 so the cursor will fit as well */
                        newlen = (int)((pextent.x1 - extent.x0 - 1) / width);
                    }

                    /* newlen can be less then 0 only in the case of a deck
                     * that has not switched to the text io obj's card.
                     * In that case show whole string.
                     */
                    if (newlen < 0)
                    {
                        fixed = ALTIA_TRUE;
                    }
                    else if (newlen < *len)
                    {
                        *len = newlen;
                    }
                }
            }
        }
    }

    if (*len >= TEXTBUFSIZE)
        *len = TEXTBUFSIZE - 1;
    if (!leftClip)
        ALT_STRNCPY(textbuf, &src[ALTIA_TEXT_START(textio)], *len);
    else
        ALT_STRNCPY(textbuf, &src[ALTIA_TEXT_START(textio)] + (fullLen - *len), *len);
    textbuf[*len] = 0;
    if (fixed && !rotated)
    {
        return textbuf;
    }

    /* Reduce parents width by 1 so that the text width must always be
    ** at least 1 pixel shorter than the parent's width for it to be
    ** considered a fit.  Otherwise, highlight and/or cursor may not
    ** fit into the parent's extent when placed after the last visible
    ** character.
    */
    if (rotated90)
    {
        if (pextent.y1 > pextent.y0)
            pextent.y1--;
        parentWidth = pextent.y1 - pextent.y0 + 1;
    }
    else
    {
        if (pextent.x1 > pextent.x0)
            pextent.x1--;
        parentWidth = pextent.x1 - pextent.x0 + 1;
    }

    while (*len > 0)
    {
        /* Keep searching until we fit into the constraint */
        Altia_Extent_type temp;
        ALTIA_BOOLEAN within = ALTIA_FALSE;
        altiaLibStringExtent(&test_fonts[fontIndex], textbuf,
                             *len, ALTIA_DOBJ_TOTAL_ADDR(dobj), &temp);
        if (!rotated)
        {
            /* only worry about the x part of the extent */
            width = temp.x1 - temp.x0 + 1;
            within = (temp.x0 >= pextent.x0) && (temp.x1 <= pextent.x1);
        }
        else
        {
            /* rotated */
            if (rotated90)
                width = temp.y1 - temp.y0 + 1;
            else
                width = temp.x1 - temp.x0 + 1;
            within = (temp.x0 >= pextent.x0) && (temp.y0 >= pextent.y0)
                && (temp.x1 <= pextent.x1) && (temp.y1 <= pextent.y1);
        }
        if (within || (fullString && width < parentWidth))
        {
            break;
        }
        else
        {
            /* Remove a letter and try again.
            ** Don't let string go to 0 len so that there is always
            ** at least 1 character displayed.
            */
            if (*len == 1)
                break;
            (*len)--;
            if (leftClip)
                memmove(textbuf, textbuf + 1, *len * sizeof(ALTIA_CHAR));
            textbuf[*len] = 0;
        }
    }
    return textbuf;
}

/*----------------------------------------------------------------------*/
static int _lineCopy(ALTIA_INDEX dobj,
                     ALTIA_INDEX textio,
                     ALTIA_CHAR *src,
                     ALTIA_CHAR *dest,
                     int ftype,
                     int max,
                     int pwidth,
                     ALTIA_BOOLEAN clip)
{
    int len;
    int pad;
    if ((pwidth > 0) || ((max == 0) && clip))
    {
        /* Max pixel width or clipped max character count */
        ALTIA_CHAR *teststring = _lineGetConstrainString(dobj, textio, 
                                                         src, max, 
                                                         clip, ftype, 
                                                         pwidth, &len);
        if (len > 0)
            ALT_STRCPY(dest, teststring);
        else
            dest[0] = 0;
        return len;
    }
    else if (max == 0)
    {
        /* No clip or max size of any kind */
        ALT_STRCPY(dest, &src[ALTIA_TEXT_START(textio)]);
        len = ALT_STRLEN(&src[ALTIA_TEXT_START(textio)]);
        return len;
    }else
    {
        /* Max character count case */
        ALTIA_BOOLEAN isIntFlag = ALTIA_FALSE;
        int value;
        len = ALT_STRLEN(&src[ALTIA_TEXT_START(textio)]);
        if (ALTIA_TEXT_START(textio) == 0
            && (((isIntFlag = altiaLibIsInteger(src,  &value)) == ALTIA_TRUE
            && ftype == RIGHT_JUSTIFY_ZERO) || len < max))
        { /* We need to check textformat */
            ALTIA_CHAR format[64];
            if (isIntFlag)
            { /* We have an integer string */
                if (ftype == RIGHT_JUSTIFY_ZERO)
                {
                    /* Move past any leading white space */
                    ALTIA_CHAR *tempstr = src;
                    while (*tempstr != 0 && ALT_ISSPACE(*tempstr))
                    {
                        tempstr++;
                        len--;
                    }
                    /* And ignore trailing white space */
                    while (tempstr[len - 1] != 0
                        && ALT_ISSPACE(tempstr[len - 1]))
                    {
                        len--;
                    }
                    /* If our string is still too big, chop it off */
                    if (len >= max)
                    {
                        ALT_STRNCPY(dest, tempstr, max);
                        dest[max] = 0;
                        return max;
                    }
                    /* Take into account a negative number */
                    if (tempstr[0] != '-')
                    {
                        ALT_SPRINTF(format,ALT_TEXT("%%0%dd"),max - len);
                        ALT_SPRINTF(dest,format,0);
                        ALT_STRCAT(dest,tempstr);
                    }
                    else
                    {
                        ALT_SPRINTF(format,ALT_TEXT("-%%0%dd"),max - len);
                        ALT_SPRINTF(dest,format,0);
                        ALT_STRCAT(dest,tempstr + sizeof(ALTIA_CHAR));
                    }
                    return max;
                }
            }
            /* handle the other justifies */
            pad = 0;
            if (ftype == RIGHT_JUSTIFY)
                pad = max - len;
            else if (ftype == CENTER_JUSTIFY)
            {
                pad = (max - len + 1) / 2;
            }
            if (pad > 0)
            {
                ALT_SPRINTF(format,ALT_TEXT("%%%dc"),pad);
                ALT_SPRINTF(dest,format,' ');
                ALT_STRCAT(dest,src);
                return ALT_STRLEN(dest);
            }else
            { /* No padding */
                ALT_STRCPY(dest, src);
                return len;
            }
        }else if (len < max)
        { /* We have a smaller then max string but we are not
           * starting at the begining.
           */
            ALT_STRCPY(dest, &src[ALTIA_TEXT_START(textio)]);
            return len;
        }else
        { /* Our string is too big so chop it off */
            ALT_STRNCPY(dest, &src[ALTIA_TEXT_START(textio)], max);
            dest[max] = '\0';
            return max;
        }
    }
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
static void _lineInsert(Altia_CustomSValue_type *custSPtr,
                        int value)
#else
static void _lineInsert(custSPtr, value)
Altia_CustomSValue_type *custSPtr;
int value;
#endif
{
    ALTIA_CHAR newval[2];
    newval[0] = value;
    newval[1] = 0;
    if (custSPtr->index == -1)
    {
        /* Check if there is room for current length + 1 */
        if (!_checkSize(custSPtr, ALT_STRLEN(custSPtr->value) + 1))
            return;
        ALT_STRCAT(custSPtr->value, newval);
    }
    else
    {
        /* We are inserting into the middle of the string */
        int rem;
        int index = custSPtr->index;
        rem = ALT_STRLEN(&custSPtr->value[index]);
        /* Check if there is room for initial length + 1 + remaining length */
        if (!_checkSize(custSPtr, index + 1 + rem))
            return;
        if (rem == 0)
            ALT_STRCAT(custSPtr->value, newval);
        else
        {
            /* a real insert */
            memmove(&custSPtr->value[index+1], &custSPtr->value[index],
                (rem+1)*sizeof(ALTIA_CHAR));
            custSPtr->value[index] = newval[0];
        }
    }
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
static void _lineDelete(Altia_CustomSValue_type *custSPtr)
#else
static void _lineDelete(custSPtr)
Altia_CustomSValue_type *custSPtr;
#endif
{
    int len;
    int index = custSPtr->index;
    if (index > 0)
    {
        len = ALT_STRLEN(&custSPtr->value[index]); 

        if (len > 0)
        {
            memmove(&custSPtr->value[index -1], &custSPtr->value[index],
                (len + 1) * sizeof(ALTIA_CHAR));
        }
        else
            custSPtr->value[index-1] = 0;
        custSPtr->index--;
    }
}

/*----------------------------------------------------------------------*/
static void _lineSet(ALTIA_INDEX textio,
                     Altia_CustomSValue_type *custSPtr,
                     ALTIA_CHAR *s,
                     int len)
{
    /* Check if there is room for requested length.
     * Note that _checkSize() will make sure the available space
     * is at least len+1.
     */
    if (!_checkSize(custSPtr, len))
        return;
    ALT_STRNCPY(custSPtr->value, s, len);
    custSPtr->index = len;
    ALTIA_SET_TEXT_SELECTSTART(textio,len);
    ALTIA_SET_TEXT_SELECTEND(textio,len);
    custSPtr->value[len] = 0;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _getDisplayInfo(ALTIA_INDEX dobj,
                                     ALTIA_INDEX textio,
                                     Altia_CustomSValue_type *custSPtr,
                                     int *start,
                                     int *last,
                                     int *len)
{
    ALTIA_BOOLEAN clipped = ALTIA_FALSE;
    ALTIA_CHAR *fullstr = custSPtr->value;
    int flen = ALT_STRLEN(fullstr);

    *start = ALTIA_TEXT_START(textio);
    if (ALTIA_TEXT_MAX_COUNT(textio) > 0)
    {
        *len = ALTIA_TEXT_MAX_COUNT(textio);
    }
    else if (ALTIA_TEXT_CLIP(textio))
    {
        /* Since we want the current string don't send the current justify
         * just set it to zero.  If we sent a center justify it would
         * assume we are going to move the string to fit.
         */
        _lineGetConstrainString(dobj, textio, custSPtr->value, ALTIA_TEXT_MAX_COUNT(textio),
                                ALTIA_TEXT_CLIP(textio), 0, ALTIA_TEXT_MAX_PIXEL_COUNT(textio), len);
    }else
        *len = flen;
    if (flen > *len)
        clipped = ALTIA_TRUE;
    else
        *len = flen;
    *last = *start + *len -1;
    return clipped;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _doScroll(ALTIA_INDEX dobj,
                               ALTIA_INDEX textio,
                               Altia_CustomSValue_type *custSPtr,
                               int num)
{
    int start, last, len;
    ALTIA_CHAR *fullstr = custSPtr->value;
    int flen = ALT_STRLEN(fullstr);
    ALTIA_BOOLEAN clipped = _getDisplayInfo(dobj, textio, custSPtr,
                                            &start, &last, &len);
    if (clipped)
    {
        if (num == ZIP_FIRST_CHAR)
            _lineSetStart(textio, custSPtr, 0);
        else if (num == ZIP_LAST_CHAR)
            _lineSetStart(textio, custSPtr, flen - len);
        else
        {
            int finish;
            start = ALTIA_TEXT_START(textio);
            _lineSetStart(textio, custSPtr, start+num);
            finish = last + num;
            if (num > 0 && finish < flen && ALTIA_TEXT_CLIP(textio))
            {
                /* When scrolling to the right we need to ensure that
                ** we actually changed the last char by the correct amount.
                ** With proportional fonts scrolling one char off the left
                ** might not scroll one onto the right.
                */
                int newstart, newlast, newlen;
                _getDisplayInfo(dobj, textio, custSPtr, &newstart, &newlast, &newlen);
                while (newlast < finish)
                {
                    start = ALTIA_TEXT_START(textio);
                    _lineSetStart(textio, custSPtr, start+1);
                    _getDisplayInfo(dobj, textio, custSPtr, &newstart, &newlast, &newlen);
                    if (start == newstart)
                    {
                        /* We have gone to the end of the string */
                        break;
                    }
                }
            }
        }
        return ALTIA_TRUE;
    }
    return ALTIA_FALSE;
}

/*----------------------------------------------------------------------*/
static void _sendCursorPos(ALTIA_INDEX dobj,
                           ALTIA_INDEX textio,
                           Altia_CustomSValue_type *custSPtr,
                           ALTIA_INDEX first,
                           ALTIA_SHORT count)
{
    int value;
    int index;
    int start;
    ALTIA_INDEX fIndex;
    /* If no user cursor then just return */
    if (ALTIA_TEXT_CURSOR(textio) != USER_LEFT && ALTIA_TEXT_CURSOR(textio) != USER_POS 
        && ALTIA_TEXT_CURSOR(textio) != USER_RIGHT)
        return;

    index = custSPtr->index;
    start = ALTIA_TEXT_START(textio);
    if (index <= start)
        value = 0;
    else
    {
        int len;
        ALTIA_CHAR *ptr = _lineGetConstrainString(dobj, textio, custSPtr->value, 
            ALTIA_TEXT_MAX_COUNT(textio), ALTIA_TEXT_CLIP(textio), 
            ALTIA_TEXT_JUSTIFY(textio), ALTIA_TEXT_MAX_PIXEL_COUNT(textio), 
            &len);
        if (ptr != nil)
        {
            int width, height, offset;
            ALTIA_INDEX fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
            ptr[index - start] = '\0';

            altiaLibFontExtent(&test_fonts[fontIndex], ptr,
                ALT_STRLEN(ptr), &width, &height, &offset);
            value = width + offset;
        }
        else
            value = 0;
    }
    fIndex = _altiaGetCustomIFuncIndex(CURSOR_POS_ID, first, count);
    if (fIndex >= 0)
    {
        /* We route the object relative value so if the object gets scaled
         * the cursor will still work correctly.
         */
        AltiaAnimate(test_funcIndex[fIndex].name, value);
    }
}



/*----------------------------------------------------------------------*/
static void _adjustCursor(ALTIA_INDEX dobj,
                          ALTIA_INDEX textio,
                          Altia_CustomSValue_type *custSPtr,
                          ALTIA_INDEX first,
                          ALTIA_SHORT count,
                          int num)
{
    int start, last, len;
    int index = custSPtr->index;

    index += num;
    _getDisplayInfo(dobj, textio, custSPtr, &start, &last, &len);
    if (index < start)
        _doScroll(dobj, textio, custSPtr, index - start);
    else if (index > last + 1)
    {
        if (index <= (int)ALT_STRLEN(custSPtr->value))
            _doScroll(dobj, textio, custSPtr, index - last - 1);
    }
    _lineSetIndex(custSPtr, index);
    _sendCursorPos(dobj, textio, custSPtr, first, count);
}

/*----------------------------------------------------------------------*/
static void _sendLength(ALTIA_INDEX dobj,
                        ALTIA_INDEX textio,
                        Altia_CustomSValue_type *custSPtr,
                        ALTIA_INDEX first,
                        ALTIA_SHORT count)
{
    int len;
    int value;
    int width, height, offset;
    ALTIA_INDEX fIndex;
    ALTIA_INDEX fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
    ALTIA_CHAR *ptr;

    /* If no length mode set then just return */
    if (ALTIA_TEXT_LENGTHMODE(textio) == NO_LENGTH)
        return;

    value = -1;
    switch (ALTIA_TEXT_LENGTHMODE(textio)){
    case DISPLAYEDP:
        ptr = _lineGetConstrainString(dobj, textio, custSPtr->value, ALTIA_TEXT_MAX_COUNT(textio),
                                      ALTIA_TEXT_CLIP(textio), ALTIA_TEXT_JUSTIFY(textio), 
                                      ALTIA_TEXT_MAX_PIXEL_COUNT(textio), &len);
        /* Need a non empty string.  Do not test an empty string because
         * target graphics library might return a non-zero offset even
         * when the string is empty.
         */
        if (ptr != nil && *ptr != '\0')
        {
            altiaLibFontExtent(&test_fonts[fontIndex], ptr,
                               len, &width, &height, &offset);
            value = width+offset;
        }
        break;
    case ACTUALP:
        /* Need a non empty string.  Do not test an empty string because
         * target graphics library might return a non-zero offset even
         * when the string is empty.
         */
        if (*(custSPtr->value) != '\0')
        {
            altiaLibFontExtent(&test_fonts[fontIndex], custSPtr->value,
                ALT_STRLEN(custSPtr->value), &width, &height, &offset);
            value = width+offset;
        }
        break;
    case DISPLAYEDC:
        /* Just get displayed character count */
        ptr = _lineGetConstrainString(dobj, textio, custSPtr->value, ALTIA_TEXT_MAX_COUNT(textio),
                                      ALTIA_TEXT_CLIP(textio), ALTIA_TEXT_JUSTIFY(textio), 
                                      ALTIA_TEXT_MAX_PIXEL_COUNT(textio), &len);
        if (ptr != nil)
            value = ALT_STRLEN(ptr);
        break;
    case ACTUALC:
        value = ALT_STRLEN(custSPtr->value);
        break;
    default:
        break;
    }

    fIndex = _altiaGetCustomIFuncIndex(LENGTH_ID, first, count);
    if (fIndex >= 0)
    {
        if (value >= 0)
            AltiaAnimate(test_funcIndex[fIndex].name, value);
        else
            AltiaAnimate(test_funcIndex[fIndex].name, 0);
    }
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _overwriteSelection(ALTIA_INDEX dobj,
                                         ALTIA_INDEX textio,
                                         Altia_CustomSValue_type *custSPtr,
                                         ALTIA_INDEX first,
                                         ALTIA_SHORT count,
                                         ALTIA_CHAR *newText,
                                         int base,
                                         int decpts)
{
    int nlen, newidx;
    int i;

    /* if nothing is currently selected, then returns ALTIA_FALSE, else
     * returns ALTIA_TRUE after deleting selection and scrolling (as necessary)
     */
    if (ALTIA_TEXT_SELECTEND(textio) == ALTIA_TEXT_SELECTSTART(textio)) {
        _lineSetSelectStart(textio, custSPtr, custSPtr->index);
        _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
        return ALTIA_FALSE;
    }

    nlen = ALT_STRLEN(newText);

    _lineDeleteSelection(textio, custSPtr);

    if (base != 0 || decpts != 0) {
        _lineAppendVal(textio, custSPtr, newText[0], base, decpts);
        _lineSetSelectStart(textio, custSPtr, custSPtr->index);
        _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
        return ALTIA_TRUE;
    }

    /* implicitly scroll to the right */

    newidx = custSPtr->index + nlen;
    if (ALTIA_TEXT_CURSOR(textio) != 0)
    {
        for (i=nlen-1; i >= 0; --i)
            _lineInsert(custSPtr, (int)newText[i]);
        _doScroll(dobj, textio, custSPtr, nlen);
        _lineSetIndex(custSPtr, newidx);
        _sendCursorPos(dobj, textio, custSPtr, first, count);
    }
    else
    {
        if (ALTIA_TEXT_SCROLL_ON(textio))
        {
            for (i=nlen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)newText[i]);

            _doScroll(dobj, textio, custSPtr, nlen);
        }
        else if (ALTIA_TEXT_CLIP(textio) || ALTIA_TEXT_MAX_COUNT(textio) > 0)
        {
            int start, last, len;
            for (i=nlen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)newText[i]);

            if (_getDisplayInfo(dobj, textio, custSPtr, &start, &last, &len))
            {
                /* If we went over the end, remove last char added */
                _lineBackup(custSPtr);
            }
        }
        else
        {
            for (i=nlen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)newText[i]);
        }
        _lineSetIndex(custSPtr, newidx);
    }
    _lineSetSelectStart(textio, custSPtr, custSPtr->index);
    _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
 
    return ALTIA_TRUE;
}

/*----------------------------------------------------------------------*/
static void _copy(ALTIA_INDEX textio,
                  Altia_CustomSValue_type *custSPtr)
{
#if !defined(ALTIAGL) || defined(ALTIAGL_HAS_CLIPBOARD)
    /* Only copy a non-empty string to clipboard. */
    ALTIA_CHAR *ptr = _lineGetSelection(textio, custSPtr);
    if (ptr != nil && *ptr != 0)
        altiaLibCopyTextToClipBoard(ptr);
#endif
}

/*----------------------------------------------------------------------*/
static void _paste(ALTIA_INDEX dobj,
                   ALTIA_INDEX textio,
                   Altia_CustomSValue_type *custSPtr,
                   ALTIA_INDEX first,
                   ALTIA_SHORT count)
{
#if defined(ALTIAGL) && !defined(ALTIAGL_HAS_CLIPBOARD)
    return;
#else
    int clen, newidx, i;
    ALTIA_CHAR *cliptext = (ALTIA_CHAR *)altiaLibGetTextFromClipBoard();

    if (cliptext == nil)
        return;
    clen = ALT_STRLEN(cliptext);

    /* If cursor is on AND scroll is off AND max char count is on AND
     * (current text + new text - any selected text) exceeds max char
     * count, change the amount of new text to meet the limit defined
     * by max char count.  To always allow adding of chars when cursor
     * is on and max char count is on, scrolling must be on as well.
     */
    if (ALTIA_TEXT_CURSOR(textio) != 0 && ALTIA_TEXT_SCROLL_ON(textio) == 0 && ALTIA_TEXT_MAX_COUNT(textio) > 0)
    {
        int slen = _lineTotalSize(custSPtr)
                   - textio_max(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio))
                   + textio_min(ALTIA_TEXT_SELECTSTART(textio), ALTIA_TEXT_SELECTEND(textio));

        if ((slen + clen) > ALTIA_TEXT_MAX_COUNT(textio))
        {
            if ((clen = ALTIA_TEXT_MAX_COUNT(textio) - slen) < 0)
                clen = 0;
            cliptext[clen] = '\0';
        }
    }

    if (clen == 0)
        return;

    if (_overwriteSelection(dobj, textio, custSPtr, first,
                            count, cliptext, 0, 0))
        return;

    newidx = custSPtr->index + clen;
    if (ALTIA_TEXT_CURSOR(textio) != 0) 
    {
        for (i=clen-1; i >= 0; --i) 
            _lineInsert(custSPtr, (int)cliptext[i]);
        _doScroll(dobj, textio, custSPtr, clen);
        _lineSetIndex(custSPtr, newidx);
        _sendCursorPos(dobj, textio, custSPtr, first, count);
    }
    else
    {
        if (ALTIA_TEXT_SCROLL_ON(textio)) 
        {
            for (i=clen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)cliptext[i]);
            _doScroll(dobj, textio, custSPtr, clen);
        }
        else if (ALTIA_TEXT_CLIP(textio) || ALTIA_TEXT_MAX_COUNT(textio) > 0)
        {
            int start, last, len;
            for (i=clen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)cliptext[i]);
            if (_getDisplayInfo(dobj, textio, custSPtr, &start, &last, &len))
            {
                /* If we went over the end, remove last char added */
                _lineBackup(custSPtr);
            }
        }
        else 
        {
            for (i=clen-1; i >= 0; --i)
                _lineInsert(custSPtr, (int)cliptext[i]);
        }
        _lineSetIndex(custSPtr, newidx);
    }
#endif
}

/*----------------------------------------------------------------------*/
static void _deleteNextChar(ALTIA_INDEX textio,
                            Altia_CustomSValue_type *custSPtr)
{
    int len;
    int index = custSPtr->index;

    len = ALT_STRLEN(&custSPtr->value[index]);
    if (len == 0)
    {
        /* nothing to delete */
        return;
    }

    memmove(&custSPtr->value[index], &custSPtr->value[index+1],
            len * sizeof(ALTIA_CHAR));
}

/*----------------------------------------------------------------------*/
static void _doSelect(ALTIA_INDEX dobj,
                      ALTIA_INDEX textio,
                      Altia_CustomSValue_type *custSPtr,
                      ALTIA_INDEX first,
                      ALTIA_SHORT count,
                      int value, ALTIA_SHORT selectMode)
{
    int oidx, nidx, flen;
    ALTIA_CHAR *ptr;
    ALTIA_CHAR ochar=nil;
    switch (selectMode) {
    case SELECT_CHAR:
        /* no modification to index necessary */
        _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
        break;
    case SELECT_WORD:
        /* have to move index to next whitespace or punctuation */
        if (custSPtr->index == 0) {
            _lineSetSelectEnd(textio, custSPtr, 0);
            break;
        }
        oidx = custSPtr->index-value;
        if (value < 0) {
            oidx -= 1;
        }
        ptr = custSPtr->value;
        flen = ALT_STRLEN(ptr);
        if (!(oidx >= flen)) {
            ochar = ptr[oidx];
        }
        if (custSPtr->index == flen) {
            _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
        }
        if (ochar) {
            if (ALT_ISSPACE(ochar) || ALT_ISPUNCT(ochar)) {
                /* do not search for any more whitespace */
                _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
                break;
            }
        }
        if (custSPtr->index == ALTIA_TEXT_SELECTSTART(textio)) {
            _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
            break;
        }
        if (value < 0) {
            nidx = custSPtr->index-1;
        } else {
            nidx = custSPtr->index+1;
        }
        while ((nidx > 0) && (nidx <= flen)) {
            if (nidx == ALTIA_TEXT_SELECTSTART(textio)) {
                break;
            }
            if (ALT_ISSPACE(ptr[nidx]) || ALT_ISPUNCT(ptr[nidx])){
                if (nidx < custSPtr->index) {
                    /* going to the left
                     * don't include the whitespace
                     */
                    ++nidx;
                }
                break;
            } else {
                if (value < 0) {
                    --nidx;
                } else {
                    ++nidx;
                }
            }
        }
        _lineSetSelectEnd(textio, custSPtr, nidx);
        _lineSetIndex(custSPtr, nidx);
        break;
    case SELECT_HOME:
        _lineSetSelectEnd(textio, custSPtr, 0);
        _lineSetIndex(custSPtr, 0);
        break;
    case SELECT_END:
        _lineSetSelectEnd(textio, custSPtr, _lineTotalSize(custSPtr));
        _lineSetIndex(custSPtr, _lineTotalSize(custSPtr));
        break;
    case SELECT_ALL_HOME:
        _lineSetSelectStart(textio, custSPtr, 0);
        _lineSetSelectEnd(textio, custSPtr, _lineTotalSize(custSPtr));
        _lineSetIndex(custSPtr, 0);
        break;
    case SELECT_ALL_END:
        _lineSetSelectStart(textio, custSPtr, 0);
        _lineSetSelectEnd(textio, custSPtr, _lineTotalSize(custSPtr));
        _lineSetIndex(custSPtr, _lineTotalSize(custSPtr));
        break;
    case SELECT_MOUSE:

        break;
    default:
        break;
    }
}

/*----------------------------------------------------------------------
 * To implement less stimulus intensive select/shortcut/jump operations,
 * shortcut keys are fixed as C/c/X/x/V/v and character value of -1 is
 * delete.  This function processes them based on the setting
 * of cursor and  shortcutOn.  The processing must include taking
 * care of length reporting for the caller.  Return ALTIA_TRUE if a key was
 * processed as a shortcut.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _processShortcut(ALTIA_INDEX dobj,
                                      ALTIA_INDEX textio,
                                      Altia_CustomSValue_type *custSPtr,
                                      ALTIA_INDEX first,
                                      ALTIA_SHORT count,
                                      int value)
{
    ALTIA_BOOLEAN ok = ALTIA_FALSE;

    if (ALTIA_TEXT_CURSOR(textio) == NO_CURSOR)
        return ALTIA_FALSE;

    if (value == -1)
    {
        /* Cursor must be on and value must be -1 to be a Delete request. */
        if (!_overwriteSelection(dobj, textio, custSPtr, first, count, ALT_TEXT(""), 0, 0))
            _deleteNextChar(textio, custSPtr);

        _sendLength(dobj, textio, custSPtr, first, count);
        ok = ALTIA_TRUE;
    }
    else if (ALTIA_TEXT_SHORTCUTON(textio))
    {
        /* Cursor must be on and shortcut
         * must be on to process copy/cut/paste.
         */
        switch (value)
        {
            /* case COPY: */
            case 'C':
            case 'c':
                _copy(textio, custSPtr);
                ok = ALTIA_TRUE;
                break;

            /* case CUT: */
            case 'X':
            case 'x':
                _copy(textio, custSPtr);
                _overwriteSelection(dobj, textio, custSPtr, first, count, ALT_TEXT(""), 0, 0);
                _sendLength(dobj, textio, custSPtr, first, count);
                ok = ALTIA_TRUE;
                break;

            /* case PASTE: */
            case 'V':
            case 'v':
                _paste(dobj, textio, custSPtr, first, count);
                _sendLength(dobj, textio, custSPtr, first, count);
                ok = ALTIA_TRUE;
                break;

            default:
                /* No action on other character values. */
                break;
        }
    }

    return ok;
}

/*----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _doCharInput(ALTIA_INDEX dobj,
                                  ALTIA_INDEX textio,
                                  Altia_CustomSValue_type *custSPtr,
                                  ALTIA_INDEX first,
                                  ALTIA_SHORT count,
                                  int value,
                                  ALTIA_BOOLEAN buffered)
{
    ALTIA_BOOLEAN res = ALTIA_FALSE;
    if (value == 0)
    {
        AltiaEventType newval;

        if (altiaLibIsNumber(custSPtr->value, &newval))
        { /* We have a complete integer so route it via INTEGER_NAME */
            ALTIA_INDEX fIndex = -1;
            ALTIA_INDEX iIndex = -1;

            fIndex = _altiaGetCustomFFuncIndex(FLOAT_ID, first, count);


            iIndex = _altiaGetCustomIFuncIndex(INTEGER_ID, first, count);


            if (fIndex >= 0)
            {
                /* We want to ignore the route ourselfs */
                ALTIA_SET_TEXT_ROUTING(textio,ALTIA_TRUE);
                AltiaAnimate(test_funcIndex[fIndex].name, newval);
                ALTIA_SET_TEXT_ROUTING(textio,ALTIA_FALSE);
            }
            if (iIndex >= 0)
            {
                /* We want to ignore the route ourselfs */
                ALTIA_SET_TEXT_ROUTING(textio,ALTIA_TRUE);

                /* Event values are already integer so no rounding required */
                AltiaAnimate(test_funcIndex[iIndex].name, newval);


                ALTIA_SET_TEXT_ROUTING(textio,ALTIA_FALSE);
            }
        }
        /* If not buffered then
        * no action required 
        * and because of that we have to handle the next_count issue
        * ourselfs
        */
        if (buffered)
        {
            /* only return 1 if the string changed */
            if (ALT_STRCMP(test_labels[ALTIA_TEXT_LABELINDEX(textio)].name, 
                           custSPtr->value) != 0)
                res = 1;
        }
        if (ALTIA_TEXT_APPEND(textio) == ALTIA_FALSE)
        {
            custSPtr->index = 0;
            ALTIA_SET_TEXT_SELECTSTART(textio,0);
            ALTIA_SET_TEXT_SELECTEND(textio,0);
            ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_TRUE);
        }
    }
    else if (_processShortcut(dobj, textio, custSPtr, first, count, value))
    {
        /* To implement less stimulus intensive shortcut operations,
        * shortcut keys now fixed as C/c/X/x/V/v and -1 for delete.
        * If one was processed, we are done!
        * Note: the processing takes care of length reporting for us.
        */
        return 1;
    }
    else if (value == '\b')
    {
        /* If we get a backspace, delete a character from our next
        * string that we are preparing to display.  Note that a
        * backspace revalidates the _next_count parameter.  That is,
        * maybe the caller just sent us a '\0' to print the string.
        * Normally, the next character sent is put in _next_string[0].
        * If the next character is a backspace, we revalidate the
        * previous string and just remove the last character from it
        * if we can.
        */
        if (_overwriteSelection(dobj, textio, custSPtr, first, count, ALT_TEXT(""), 0, 0)) 
        {
            _sendLength(dobj, textio, custSPtr, first, count);
            return 1;
        }
        if (ALTIA_TEXT_CURSOR(textio) != 0)
        {
            int index;
            _lineDelete(custSPtr);
            /* We want to make sure that the character in front of
             * the removed character is always shown.  We do this by
             * scrolling and if we can't see that char then we scroll back.
             */
            index = custSPtr->index -1;
            if (ALTIA_TEXT_CLIP(textio) && index > 0)
            {
                /* We need to check to see that we did not scroll through
                * more characters then just one (This happens with
                * proportional spaced fonts and clip on.
                */
                if (_doScroll(dobj, textio, custSPtr, -1))
                {
                    /* We did a actual scroll */
                    int nstart, nlast, nlen;
                    _getDisplayInfo(dobj, textio, custSPtr, &nstart, &nlast, &nlen);
                    if (index > nlast)
                    {
                        /* undo the scroll since it went too far */
                        _doScroll(dobj, textio, custSPtr, 1);
                    }
                }
            }
            else
            {
                _doScroll(dobj, textio, custSPtr, -1);
            }
            _sendCursorPos(dobj, textio, custSPtr, first, count);
        }
        else
        {
            _lineBackup(custSPtr);
            if (ALTIA_TEXT_SCROLL_ON(textio))
                _doScroll(dobj, textio, custSPtr, -1);
        }

        if (!buffered)
        {
            res = 1;
        }
    }
    else if ((value != 9
        && value <= ALT_CHARMASK && value >= 0 && ALT_ISPRINT(value))
        || (value >= 0x80 && value <= ALT_CHARMASK))
    {
        /* only accept printable values , Kanji tabs (9) are printable
        ** but we don't want to show them.
        ** NOTE:   We test if 0 <= value <= ALT_CHARMASK before passing it
        **         to ALT_ISPRINT().  On Windows at least, ALT_ISPRINT()
        **         might use an array to look up the printable status of the
        **         value so it better not be out of range of the array size.
        */

#ifdef ALTIA_FONT_PRELOAD
        /* Preload the font character.  Useful with runtime font engines */
        if (ALTIA_TEXT_LABELINDEX(textio) >= 0)
            egl_FontPreload(test_labels[ALTIA_TEXT_LABELINDEX(textio)].font, (EGL_CHAR)value);
#endif /* ALTIA_FONT_PRELOAD */

        /* If we are to clear the string on next key, do it now */
        if (ALTIA_TEXT_CLEARNEXT(textio))
        {
            ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_FALSE);

            if (custSPtr->index == 0)
                custSPtr->value[0] = 0;
        }

        /* Process new character into this object */
        textbuf[0] = (ALTIA_CHAR) value;
        textbuf[1] = '\0';
        if (!buffered && _overwriteSelection(dobj, textio, custSPtr, first, count, textbuf, 0, 0))
        {
            _sendLength(dobj, textio, custSPtr, first, count);
            return 1;
        }
        if (!buffered)
        {
            if (ALTIA_TEXT_CURSOR(textio) != 0)
            {
                /* Scroll must be on OR max char count must be off OR total
                * length of line must be less than max char count before
                * attempting to add another char.  To always allow adding of
                * chars when cursor is on and max char count is on, scrolling
                * must be on as well.
                */
                if (ALTIA_TEXT_SCROLL_ON(textio)
                    || ALTIA_TEXT_MAX_COUNT(textio) <= 0
                    || _lineTotalSize(custSPtr) < ALTIA_TEXT_MAX_COUNT(textio))
                {
                    _lineInsert(custSPtr, value);
                    _adjustCursor(dobj, textio, custSPtr, first, count, 1);
                }
            }
            else
            {
                if (ALTIA_TEXT_SCROLL_ON(textio))
                {
                    _lineAppend(textio, custSPtr, value);
                    _doScroll(dobj, textio, custSPtr, ZIP_LAST_CHAR);
                }
                else if (ALTIA_TEXT_CLIP(textio) || ALTIA_TEXT_MAX_COUNT(textio) > 0)
                {
                    /* We want to stop adding chars if we get to the end */
                    int start, last, len;
                    _lineAppend(textio, custSPtr, value);
                    if (_getDisplayInfo(dobj, textio, custSPtr, &start, &last, &len))
                    {
                        /* If we went over the end then remove last char we added */
                        _lineBackup(custSPtr);
                    }
                }else
                    _lineAppend(textio, custSPtr, value);

            }
            /* Note that we want to redraw string because we are doing
             * non-buffered I/O.
             */
            res = 1;
        }else
            _lineAppend(textio, custSPtr, value);
    }

    /* Only output the length if res is non-zero (i.e., a visual change
    * is needed) or if value is '\0' (i.e., completion of string).  Avoid
    * outputting the length if this is just an add/remove of a character
    * during buffered input.
    */
    if (res || value == '\0')
        _sendLength(dobj, textio, custSPtr, first, count);

    return res;
}
/*----------------------------------------------------------------------*/
static void _doJustify(ALTIA_INDEX dobj, ALTIA_INDEX textio)
{
    /* We don't do LEFT_JUSTIFY since the textio object does that
    ** automaticly and RIGHT_JUSTIFY_ZERO since that is done with max_count
    */
    if (ALTIA_TEXT_MAX_COUNT(textio) == 0 &&
       (ALTIA_TEXT_JUSTIFY(textio) == RIGHT_JUSTIFY || ALTIA_TEXT_JUSTIFY(textio) >= CENTER_JUSTIFY))
    {
        /* adjust the transform so the text is justified to the parent or 
        ** sibling
        */
        Altia_Extent_type extent;
        Altia_Extent_type myextent;
        ALTIA_DOUBLE offset;
        ALTIA_DOUBLE offset2 = 0;
        ALTIA_FLOAT x0, y0;
        ALTIA_FLOAT x1, y1;
        ALTIA_INDEX fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
        ALTIA_BOOLEAN missingSibling = ALTIA_FALSE;

        ALTIA_INDEX parent = ALTIA_DOBJ_PARENT(dobj);
        if (parent >= 0)
        {
            if (ALTIA_TEXT_JUSTIFY(textio) == CENTER_JUSTIFY || ALTIA_TEXT_JUSTIFY(textio) == RIGHT_JUSTIFY)
            {
                _altiaGetExtent(AltiaDynamicObject, parent, &extent);

            }
            else
            {

                /* We want the prev siblings extent to justify on instead of 
                ** the parent.
                */
                ALTIA_INDEX prev = _altiaGetSibling(parent, dobj);

                /* Child index is -1 if child is popped to top. */
                if (prev < 0 || (ALTIA_INDEX)test_children[prev].child == -1)
                {

                    _altiaGetExtent(AltiaDynamicObject, parent, &extent);

                    /* For later, it is important to know there are no
                     * siblings if the justify mode is sibling mode.
                     */
                    missingSibling = ALTIA_TRUE;


                }else
                {
                    _altiaGetExtent((ALTIA_SHORT)test_children[prev].type, 
                                    test_children[prev].child, 
                                    &extent);

                }

            }

            /* We need to figure out a new extent since the string might
            ** have changed and can't use obj->extent.
            */
            {
                Altia_Extent_type temp;
                int w, h, offset;

                /* Keep Text I/O from shifting on center or right sibling
                ** justify by only applying negative offsets during the extent
                ** calc.  BUT, just do this if the Text I/O really has a
                ** sibling otherwise the text drifts (yes, it is ALTIA_TRUE that
                ** using a sibling mode without a sibling is invalid, but
                ** it happens and we want to keep the behavior the same).
                ** For left/non sibling justify, still apply any offset
                ** (positive or negative) to keep historical text position.
                ** This is a good compromise.  It keeps numbers with positive
                ** offsets from wiggling for center or right sibling justify
                ** while characters with negative offsets (such as in script
                ** fonts) stay better aligned (don't go too far left, off
                ** center, or not far enough right).  Left sibling justify
                ** numbers with positive offsets will still wiggle, but not
                ** applying the offset will change the text position for many
                ** many existing designs which can be difficult to repair.
                ** The pure left justify mode (0) will not wiggle so that is
                ** a workaround for the left sibling justify mode wiggle.
                ** Another option would be to only respect negative offsets
                ** for left sibling justify, but then center and right sibling
                ** justify can be inaccurate for script fonts.  If we did
                ** not have to be careful with the offset, we could simply
                ** call altiaLibStringExtent().
                **
                ** When clip mode is enabled, text for right sibling justify
                ** modes might go too far left/right if positive offset not
                ** applied.  Investigation shows center sibling justify mode
                ** with clip mode enabled looks better if positive offset
                ** is not applied.
                */
                AltiaFloatExtent = 1;
                altiaLibFontExtent(
                    &test_fonts[fontIndex], ALTIA_TEXT_DISPSTR(textio),
                    ALT_STRLEN(ALTIA_TEXT_DISPSTR(textio)), &w, &h, &offset);

                /* Per above comment, ignore the offset (use 0) if:
                ** 1. The offset is positive.
                ** 2. The object actually has a sibling to justify with.
                ** 3. The justify mode is right or center sibling.
                ** 4. But NOT clip mode with one of the right sibling modes.
                **/
                if (0 < offset
                    && !missingSibling
                    && LEFT_SIB_JUSTIFY < ALTIA_TEXT_JUSTIFY(textio)
                    && !(ALTIA_TEXT_CLIP(textio)
                         && (RIGHT_SIB_JUSTIFY == ALTIA_TEXT_JUSTIFY(textio)
                             || RIGHT_SIB_CLIP_LEFT_JUSTIFY == ALTIA_TEXT_JUSTIFY(textio))))
                {
                    offset = 0;
                }

                temp.x0 = offset;
                temp.x1 = w + offset;
                temp.y0 = 0;
                temp.y1 = h;
                altiaLibTransformExtent(ALTIA_DOBJ_TOTAL_ADDR(dobj), &temp, &myextent);
                AltiaFloatExtent = 0;
            }
            /* To determine which way should be left or right we will
            ** transform 2 pts and see if they increase or decrease.
            **/
            altiaLibTransformF(ALTIA_DOBJ_TOTAL_ADDR(dobj), 0, 0, &x0, &y0);
            altiaLibTransformF(ALTIA_DOBJ_TOTAL_ADDR(dobj), ALTIA_C2F(1), 0, &x1, &y1);

            {
                /* RTC-1757:  This work item supercedes 1498 and 1519.  For this 
                **            change we use the floating point extent values without
                **            any rounding.  The floating point values have no padding
                **            which fixes the issue from RTC-1498.  The floating
                **            point values are not rounded which fixes the issue 
                **            from RTC-1519.
                **
                **            This work item is required because rounding the floating
                **            point extent is the same as introducing pad.  This
                **            change eliminates rounding altogether.  Whereas 1519
                **            eliminated the rounding only for missing sibling cases.
                **
                ** HISTORY (superceded items):
                **
                ** RTC-1519:  The precise extent cannot be used if we're missing
                **            a sibling because in that case we're using the
                **            parent extent.  If the parent is effected by this
                **            text object then we'll have a mismatch between
                **            the text and itself.  Need to use same extent as
                **            calculated originally.
                **
                ** RTC-1498:  Use rounded floating point extent values (not padded).
                **            This prevents error from the normal transformed
                **            extent which will pad dimensions by +1 pixel when
                **            the extent is fractionally larger than an integer
                **            pixel value.
                */
                if (ALTIA_ROTATED90(ALTIA_DOBJ_TOTAL_ADDR(dobj)))
                {
                    /* text has been rotated so center on y instead of x */
                    if (ALTIA_TEXT_JUSTIFY(textio) == CENTER_JUSTIFY ||
                        ALTIA_TEXT_JUSTIFY(textio) == CENTER_SIB_JUSTIFY)
                    {
                        offset = (ALTIA_FLOAT)
                                 FSUB(FMULT(ALTIA_I2D(extent.y0 + extent.y1), ALTIA_C2D(0.5f)),
                                      FMULT(FADD(AltiaExtentY0, AltiaExtentY1), ALTIA_C2D(0.5f)));
                    }
                    else if (ALTIA_TEXT_JUSTIFY(textio) == RIGHT_JUSTIFY ||
                             ALTIA_TEXT_JUSTIFY(textio) == RIGHT_SIB_JUSTIFY ||
                             ALTIA_TEXT_JUSTIFY(textio) == RIGHT_SIB_CLIP_LEFT_JUSTIFY)
                    {
                        if (y0 <= y1)
                            offset = FSUB(ALTIA_I2F(extent.y1), AltiaExtentY1);
                        else
                            offset = FSUB(ALTIA_I2F(extent.y0), AltiaExtentY0);
                    }
                    else if (ALTIA_TEXT_JUSTIFY(textio) == LEFT_SIB_JUSTIFY)
                    {
                        if (y0 <= y1)
                            offset = FSUB(ALTIA_I2F(extent.y0), AltiaExtentY0);
                        else
                            offset = FSUB(ALTIA_I2F(extent.y1), AltiaExtentY1);
                    }
                    else
                        offset = 0;

                    if (!ALTIA_EQUAL(offset, 0))
                    {
                        ALTIA_FLOAT tx, ty;
                        /* inverse transform the distance so when the
                        ** parents transforms get applied we move the dist.
                        */
                        altiaLibInvTransformDistF(ALTIA_DOBJ_TOTAL_ADDR(parent),
                                                  offset2, offset, &tx, &ty);
                        altiaLibTranslate(ALTIA_DOBJ_TRANS_ADDR(dobj), tx, ty);

                        /* For a move not in a defined animation, fix lastTrans */
                        altiaLibTranslate(ALTIA_DOBJ_LASTTRANS_ADDR(dobj), tx, ty);
                        _altiaTotalTrans(AltiaDynamicObject, dobj);
                    }
                }
                else
                {
                    /* center in the x direction */
                    if (ALTIA_TEXT_JUSTIFY(textio) == CENTER_JUSTIFY ||
                        ALTIA_TEXT_JUSTIFY(textio) == CENTER_SIB_JUSTIFY)
                    {
                        offset = (ALTIA_FLOAT)
                                 FSUB(FMULT(ALTIA_I2D(extent.x0 + extent.x1), ALTIA_C2D(0.5f)),
                                      FMULT(FADD(AltiaExtentX0, AltiaExtentX1), ALTIA_C2D(0.5f)));
                    }
                    else if (ALTIA_TEXT_JUSTIFY(textio) == RIGHT_JUSTIFY ||
                             ALTIA_TEXT_JUSTIFY(textio) == RIGHT_SIB_JUSTIFY ||
                             ALTIA_TEXT_JUSTIFY(textio) == RIGHT_SIB_CLIP_LEFT_JUSTIFY)
                    {
                        if (x0 <= x1)
                            offset = FSUB(ALTIA_I2F(extent.x1), AltiaExtentX1);
                        else
                            offset = FSUB(ALTIA_I2F(extent.x0), AltiaExtentX0);
                    }
                    else if (ALTIA_TEXT_JUSTIFY(textio) == LEFT_SIB_JUSTIFY)
                    {
                        if (x0 <= x1)
                            offset = FSUB(ALTIA_I2F(extent.x0), AltiaExtentX0);
                        else
                            offset = FSUB(ALTIA_I2F(extent.x1), AltiaExtentX1);
                    }
                    else
                        offset = 0;
                    if (!ALTIA_EQUAL(offset,0))
                    {
                        ALTIA_FLOAT tx, ty;
                        /* inverse transform the distance so when the
                        ** parents transforms get applied we move the dist.
                        */
                        altiaLibInvTransformDistF(ALTIA_DOBJ_TOTAL_ADDR(parent),
                                                  offset, offset2, &tx, &ty);
                        altiaLibTranslate(ALTIA_DOBJ_TRANS_ADDR(dobj), tx, ty);

                        /* For a move not in a defined animation, fix lastTrans */
                        altiaLibTranslate(ALTIA_DOBJ_LASTTRANS_ADDR(dobj), tx, ty);
                        _altiaTotalTrans(AltiaDynamicObject, dobj);
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
 * To implement less stimulus intensive select/shortcut/jump operations,
 * scroll processing is in its own function.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _processScroll(ALTIA_INDEX dobj,
                                    ALTIA_INDEX textio,
                                    Altia_CustomSValue_type *custSPtr,
                                    ALTIA_INDEX first,
                                    ALTIA_SHORT count,
                                    int num, ALTIA_BOOLEAN jump,
                                    ALTIA_SHORT selectMode)
{
    ALTIA_BOOLEAN ok = ALTIA_FALSE;

    int nidx, flen;
    ALTIA_CHAR *ptr;

    /* If this is a selection request and the selection start is the
     * same as the selection end, they may not be at the current cursor.
     * Get them positioned the sam as the current cursor position.
     */
    if (selectMode != NO_SELECT
        && (ALTIA_TEXT_SELECTSTART(textio) == ALTIA_TEXT_SELECTEND(textio)))
    {
        _lineSetSelectStart(textio, custSPtr, custSPtr->index);
        _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
    }

    if (jump)
    {
        /* have to move index to next whitespace or punctuation */
        ptr = custSPtr->value;
        flen = ALT_STRLEN(ptr);
        if (num < 0)
        {
            nidx = custSPtr->index-1;
        }
        else
        {
            if (!ALT_ISSPACE(ptr[custSPtr->index])
                && !ALT_ISPUNCT(ptr[custSPtr->index]))
                nidx = custSPtr->index+1;
            else
                nidx = custSPtr->index;
        }
        while ((nidx > 0) && (nidx <= flen))
        {
            if (ALT_ISSPACE(ptr[nidx]) || ALT_ISPUNCT(ptr[nidx]))
            {
                if (nidx < custSPtr->index)
                {
                    /* going to the left
                     * don't include the whitespace
                     */
                    ++nidx;
                }
                break;
            }
            else 
            {
                if (num < 0)
                    --nidx;
                else
                    ++nidx;
            }
        }
        if (!(nidx == custSPtr->index))
            num = nidx - custSPtr->index;
    }

    if (num != 0)
    {
        ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_FALSE);

        if (ALTIA_TEXT_CURSOR(textio) != 0)
        {
            ALTIA_SHORT currIndex; /* save for later compare */
            /* Move cursor differently for a select to
             * home or end request.
             */
            if (selectMode < SELECT_HOME || selectMode > SELECT_ALL_END)
                _adjustCursor(dobj, textio, custSPtr, first, count, num);
            else if (selectMode == SELECT_HOME || selectMode == SELECT_ALL_HOME)
                _adjustCursor(dobj, textio, custSPtr, first, count, -custSPtr->index);
            else
                _adjustCursor(dobj, textio, custSPtr, first, count, _lineTotalSize(custSPtr) - custSPtr->index);

            currIndex = custSPtr->index;
            if (selectMode != NO_SELECT)
                _doSelect(dobj, textio, custSPtr, first, count, num, selectMode);
            if (currIndex != custSPtr->index)
            {
                /* need to move the cursor */
                _doScroll(dobj, textio, custSPtr, custSPtr->index - currIndex);
            }
            ok = ALTIA_TRUE;
        }
        else if (ALTIA_TEXT_SCROLL_ON(textio) && _doScroll(dobj, textio, custSPtr, num))
        {
            ALTIA_SHORT currIndex = custSPtr->index;;
            if (selectMode != NO_SELECT)
                _doSelect(dobj, textio, custSPtr, first, count, num, selectMode);
            if (currIndex != custSPtr->index) 
            {
                /* need to move the cursor */
                if (currIndex < custSPtr->index)
                    _adjustCursor(dobj, textio, custSPtr, first, count, custSPtr->index - currIndex);
                else
                    _adjustCursor(dobj, textio, custSPtr, first, count, currIndex - custSPtr->index);
            }
            ok = ALTIA_TRUE;
        }
        else if (selectMode == SELECT_ALL_HOME
                 || selectMode == SELECT_ALL_END)
        {
            /* Allow select all requests even if scrolling and
             * the cursor are not on.
             */
            ALTIA_SHORT currIndex = custSPtr->index;
            _doSelect(dobj, textio, custSPtr, first, count, num, selectMode);
            if (currIndex != custSPtr->index)
                _doScroll(dobj, textio, custSPtr, custSPtr->index - currIndex);
            ok = ALTIA_TRUE;
        }
        if (selectMode == NO_SELECT) 
        {
            _lineSetSelectStart(textio, custSPtr, custSPtr->index);
            _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
        }
    }

    return ok;
}

/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaLibTextioProcess(ALTIA_INDEX textio,
                                    ALTIA_INDEX dobj,
                                    ALTIA_BOOLEAN pushExtent)
{
    Altia_CustomSValue_type * custSPtr = ALTIA_TEXT_CUSTS(textio);
    if (NULL != custSPtr)
    {
        ALTIA_BOOLEAN doUpdate = ALTIA_FALSE;

        /* Only perform processing if the text is visible and doesn't have 
        ** dependencies on animations (such as the length animation)
        */
        if (((ALTIA_SHORT)0 != ALTIA_TEXT_LENGTHMODE(textio)) ||
            (ALTIA_FALSE == _altiaIsHidden(dobj, AltiaDynamicObject)))
        {
            doUpdate = ALTIA_TRUE;
        }

        if (ALTIA_FALSE != doUpdate)
        {
            /*
             * We keep in the extent field the object relative extent.
             * When we compute the extent we can just use that.  This
             * should be the only place where we need to recompute that
             * extent since only animation can change that.
             */
            Altia_Extent_type extent;
            Altia_Transform_type ident;
            ALTIA_INDEX fontIndex = test_labels[ALTIA_TEXT_LABELINDEX(textio)].font;
            ALTIA_CHAR * text = custSPtr->value;
            int len;

            /* Clear dirty flag now -- otherwise we'll get recursion with _altiaFindExtent */
            ALTIA_SET_TEXT_CUSTS(textio, 0);




            ident.a00 = ident.a11 = ALT_I2FX(1);
            ident.a01 = ident.a10 = 0;
            ident.a20 = 0;
            ident.a21 = 0;

            ident.angle = 0;
            ident.xoffset = 0;
            ident.yoffset = 0;



            _lineCopy(dobj, textio, text, ALTIA_TEXT_DISPSTR(textio), ALTIA_TEXT_JUSTIFY(textio), 
                      ALTIA_TEXT_MAX_COUNT(textio), ALTIA_TEXT_MAX_PIXEL_COUNT(textio), ALTIA_TEXT_CLIP(textio));

            /* adjust the label string to ours */
            test_labels[ALTIA_TEXT_LABELINDEX(textio)].name = ALTIA_TEXT_DISPSTR(textio);
            _doJustify(dobj, textio);
            len = ALT_STRLEN(ALTIA_TEXT_DISPSTR(textio));
            if (len == 0)
            {
                altiaLibStringExtent(&test_fonts[fontIndex], ALT_TEXT("i"),
                                     1, &ident, 
                                     &test_labels[ALTIA_TEXT_LABELINDEX(textio)].extent);
            }
            else
            {
                altiaLibStringExtent(&test_fonts[fontIndex], ALTIA_TEXT_DISPSTR(textio),
                                     len, &ident, 
                                     &test_labels[ALTIA_TEXT_LABELINDEX(textio)].extent);
            }

            /* Finalize extent */
            _altiaFindExtent(AltiaDynamicObject, dobj, &extent);
            if (ALTIA_FALSE != pushExtent)
            {
                _altiaAddExtent(AltiaDynamicObject, dobj);
            }

            _altiaUpdateParentExtent(AltiaDynamicObject, dobj);

            /* Updated */
            return ALTIA_TRUE;
        }
    }

    return ALTIA_FALSE;
}


/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaTextioAnimation(ALTIA_INDEX dobj,
                                    ALTIA_INDEX first,
                                    ALTIA_SHORT count,
                                    ALTIA_UINT32 id,
                                    ALTIA_INDEX cindex,
                                    AltiaEventType value)
{

    /* For fixed point, event value is an integer so no rounding.
     * ALTIA_ROUND(ALTIA_E2D(value)) would only truncate the value
     * resulting in unexpected different behavior for fixed point.
     */
    int ivalue = (int) value;


    ALTIA_BOOLEAN ok = ALTIA_FALSE;
    ALTIA_INDEX textio = ALTIA_DOBJ_DRAWINDEX(dobj);
    ALTIA_BOOLEAN routing = ALTIA_TEXT_ROUTING(textio);
    Altia_CustomSValue_type *custSPtr;
    /* The TEXT_ID animation is where the string is stored so get it */
    int idx = _altiaGetCustomSIndex(TEXT_ID, first, count);
    if (idx < 0)
    {
        return ALTIA_FALSE;
    }
    custSPtr = &test_custS[idx];

    if (id == TEXT_ID && routing == ALTIA_FALSE)
    {
        if (ivalue != -1)
        {
            ok = _doCharInput(dobj, textio, custSPtr, first, count, ivalue, ALTIA_TRUE);
        }
        else if (ivalue == -1)
        {
            /* We need to route our string */
            /* We want the whole string */
            int str_count;
            int i;

            ALTIA_CHAR string[64 + 1];


            ALTIA_INDEX nIndex;
            int savestart = ALTIA_TEXT_START(textio);

            _lineSetStart(textio, custSPtr, 0);

            /* Let's get the formated string  */
            str_count = _lineCopy(dobj, textio, custSPtr->value, string, 
                                  ALTIA_TEXT_JUSTIFY(textio), 0, ALTIA_TEXT_MAX_PIXEL_COUNT(textio), 
                                  ALTIA_FALSE);

            /* Don't want to get into feedback loop so set flag indicating that
            * we are in the process of routing commands.
            */
            ALTIA_SET_TEXT_ROUTING(textio,ALTIA_TRUE);
            nIndex = _altiaGetCustomSFuncIndex(TEXT_ID, first, count);

            if (nIndex >= 0)
            {
                for(i = 0; i < str_count; i++)
                {
                    AltiaAnimate(test_funcIndex[nIndex].name, 
                        string[i] & ALT_CHARMASK);
                }
                ok = AltiaAnimate(test_funcIndex[nIndex].name, 0);
            }

            /* Done routing. */
            ALTIA_SET_TEXT_ROUTING(textio,ALTIA_FALSE);
            _lineSetStart(textio, custSPtr, savestart);

        }
    }
    if ((id == INTEGER_ID || id == FLOAT_ID) && routing == ALTIA_FALSE)
    {/* We want to output the value sent as a string */
        int index;
        if (!ALTIA_TEXT_APPEND(textio))
        {
            _lineSetIndex(custSPtr, 0);
            _lineSetSelectStart(textio, custSPtr, 0);
            _lineSetSelectEnd(textio, custSPtr, 0);
            _lineAppendVal(textio, custSPtr, value, ALTIA_TEXT_BASE(textio), ALTIA_TEXT_DECPTS(textio));
            _doScroll(dobj, textio, custSPtr, ZIP_LAST_CHAR);
        }else
        {
            index = custSPtr->index;
            textbuf[0] = (ALTIA_CHAR) value;
            textbuf[1] = '\0';
            if (!_overwriteSelection(dobj, textio, custSPtr, first, count, textbuf, ALTIA_TEXT_BASE(textio), ALTIA_TEXT_DECPTS(textio)))
                _lineAppendVal(textio, custSPtr, value, ALTIA_TEXT_BASE(textio), ALTIA_TEXT_DECPTS(textio));
            _doScroll(dobj, textio, custSPtr, custSPtr->index - index);
        }

        ok = ALTIA_TRUE;
        if (!ALTIA_TEXT_APPEND(textio))
        {
            _lineSetIndex(custSPtr, 0);
            ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_TRUE);
        }
        _sendLength(dobj, textio, custSPtr, first, count);
    }
    if (id == CHARACTER_ID && routing == ALTIA_FALSE)
    {
        ok = _doCharInput(dobj, textio, custSPtr, first, count, ivalue, ALTIA_FALSE);
    }
    if (id == CLEAR_CHAR_ID)
    {
        /* Do the clear unless all indications are that the text is
        * already cleared.  This is an optimization compared to
        * the behavior in Altia Design/Runtime.
        */
        if (ALTIA_TEXT_DISPSTR(textio) == nil 
            || ALTIA_TEXT_DISPSTR(textio)[0] != '\0'
            || custSPtr->value == nil || custSPtr->value[0] != '\0'
            || custSPtr->index != 0 || ALTIA_TEXT_START(textio) != 0
            || ALTIA_TEXT_SELECTSTART(textio) != 0 
            || ALTIA_TEXT_SELECTEND(textio) != 0)
        {
            _lineSet(textio, custSPtr, ALT_TEXT(""), 1);
            _lineSetIndex(custSPtr, 0);
            _lineSetStart(textio, custSPtr, 0);
            _lineSetSelectStart(textio, custSPtr, 0);
            _lineSetSelectEnd(textio, custSPtr, 0);

            /* Report length if length reporting is enabled. */
            _sendLength(dobj, textio, custSPtr, first, count);
            ok = ALTIA_TRUE;
        }
    }
    if (id == JUSTIFY_ID && routing == ALTIA_FALSE)
    {
        int jval = ALTIA_IVAL_VALUE(cindex);
        if (ALTIA_TEXT_JUSTIFY(textio) != jval)
        {
            ALTIA_SET_TEXT_JUSTIFY(textio,jval);

            /* Since we can't have justify on at the same time
            * as cursor and scroll_on.  We have to turn those off.
            */
            if (ALTIA_TEXT_JUSTIFY(textio) != 0)
            {
                ALTIA_SET_TEXT_CURSOR(textio,0);
                ALTIA_SET_TEXT_SCROLL_ON(textio,ALTIA_FALSE);
            }
            else
            {
                /* If justify is going to 0 and scroll was previously on,
                 * restore the scrolling to on.
                 */
                ALTIA_SET_TEXT_SCROLL_ON(textio,ALTIA_TEXT_ACTUAL_SCROLL_ON(textio));
            }
            ok = ALTIA_TRUE;
        }
    }
    if (id == MAX_CHAR_ID)
    {
        if (ivalue <= 0)
        {
            if (ALTIA_TEXT_MAX_COUNT(textio) > 0)
            {
                ALTIA_SET_TEXT_MAX_COUNT(textio,0);
                ok = ALTIA_TRUE;
            }
        }
        else
        {
            ALTIA_SET_TEXT_MAX_COUNT(textio,ivalue);
            ok = ALTIA_TRUE;
        }
    }
    if (id == INT_BASE_ID)
    {
        ALTIA_SET_TEXT_BASE(textio,ALTIA_IVAL_VALUE(cindex));
    }
    if (id == CLIP_ID)
    {
        if (ivalue == 0)
        {
            if (ALTIA_TEXT_CLIP(textio) == ALTIA_TRUE)
            {
                ALTIA_SET_TEXT_CLIP(textio,ALTIA_FALSE);
                ok = ALTIA_TRUE;
            }
        }
        else
        {
            if (ALTIA_TEXT_CLIP(textio) == ALTIA_FALSE)
            {
                ALTIA_SET_TEXT_CLIP(textio,ALTIA_TRUE);
                ok = ALTIA_TRUE;
            }
        }
    }
    if (id == APPEND_ID)
    {
        if (ivalue == 0)
        {
            if (ALTIA_TEXT_APPEND(textio))
                _lineSetIndex(custSPtr, 0);
            ALTIA_SET_TEXT_APPEND(textio,ALTIA_FALSE);
        }
        else
        {
            if (ALTIA_TEXT_APPEND(textio) == ALTIA_FALSE)
            { /* move the index to the end of the string */
                int len = _lineSize(textio, custSPtr->value, 0);
                _lineSetIndex(custSPtr, len);
            }
            ALTIA_SET_TEXT_APPEND(textio,ALTIA_TRUE);
            ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_FALSE);
        }
    }
    if (id == SCROLL_ON_ID)
    {
        if (ALTIA_TEXT_JUSTIFY(textio) == 0)
        {
            if (ivalue == 0)
            {
                ALTIA_SET_TEXT_SCROLL_ON(textio,ALTIA_FALSE);
            }
            else
            {
                ALTIA_SET_TEXT_SCROLL_ON(textio,ALTIA_TRUE);
            }

            /* Track newest scroll on state in case it gets temporarily
            * reset by justify mode.
            */
            ALTIA_SET_TEXT_ACTUAL_SCROLL_ON(textio,ALTIA_TEXT_SCROLL_ON(textio));
        }
    }
    if (id == SCROLL_ID)
    {
        /* If cursor is on and select is on, there is selecting to do. */
        if (ALTIA_TEXT_CURSOR(textio) != NO_CURSOR && ALTIA_TEXT_SELECTON(textio))
        {
            ALTIA_SHORT selectMode;
            if (ivalue == -10000)
                selectMode = SELECT_HOME;
            else if (ivalue == 10000)
                selectMode = SELECT_END;
            else if (ALTIA_TEXT_JUMPON(textio))
                selectMode = SELECT_WORD;
            else
                selectMode = SELECT_CHAR;
            if (_processScroll(dobj, textio, custSPtr, first, count, ivalue, ALTIA_FALSE, selectMode))
                ok = ALTIA_TRUE;
        }

        /* Else, if value is -10000 or 10000, go to end or beginning. */
        else if (ivalue == -10000 || ivalue == 10000)
        {
            if (_processScroll(dobj, textio, custSPtr, first, count, ivalue, ALTIA_FALSE, NO_SELECT))
                ok = ALTIA_TRUE;

            /* Work around for defect when going to end of text.  Must
            * tickle it a bit to get text to scroll if necessary.  Could
            * use cursor mode BUILTIN_END (8) for this, but it does
            * not work for user cursor.  Should fix this at some point.
            */
            if (ivalue == 10000)
            {
                if (_processScroll(dobj, textio, custSPtr, first, count, -1, ALTIA_FALSE, NO_SELECT))
                    ok = ALTIA_TRUE;
                if (_processScroll(dobj, textio, custSPtr, first, count, 1, ALTIA_FALSE, NO_SELECT))
                    ok = ALTIA_TRUE;
            }
        }

        /* Else, if cursor is on and jump is on, jump desired number
        * of words left or right.
        */
        else if (ALTIA_TEXT_CURSOR(textio) != NO_CURSOR && ALTIA_TEXT_JUMPON(textio))
        {
            if (_processScroll(dobj, textio, custSPtr, first, count, ivalue, ALTIA_TRUE, NO_SELECT))
                ok = ALTIA_TRUE;
        }

        /* Else, this is just an old fashioned scroll. */
        else if (_processScroll(dobj, textio, custSPtr, first, count, ivalue, ALTIA_FALSE, NO_SELECT))
            ok = ALTIA_TRUE;
    }
    if (id == CURSOR_ID)
    {
        if (ALTIA_TEXT_JUSTIFY(textio) == 0)
        {
            int start, last, len;
            ALTIA_SET_TEXT_CURSOR(textio,ivalue);
            if (ivalue == BUILTIN_LEFT || ivalue == USER_LEFT)
            {
                _lineSetIndex(custSPtr, ALTIA_TEXT_START(textio));
                if (ALTIA_TEXT_SELECTSTART(textio) == ALTIA_TEXT_SELECTEND(textio)) {
                    _lineSetSelectStart(textio, custSPtr, ALTIA_TEXT_START(textio));
                    _lineSetSelectEnd(textio, custSPtr, ALTIA_TEXT_START(textio));
                }
            }
            else if (ivalue == BUILTIN_RIGHT || ivalue == USER_RIGHT)
            {
                /* display cursor at end of line */
                _getDisplayInfo(dobj, textio, custSPtr, &start, &last, &len);
                _lineSetIndex(custSPtr, last + 1);
                if (ALTIA_TEXT_SELECTSTART(textio) == ALTIA_TEXT_SELECTEND(textio)) {
                    _lineSetSelectStart(textio, custSPtr, last + 1);
                    _lineSetSelectEnd(textio, custSPtr, last + 1);
                }
            }
            else if (ivalue == BUILTIN_POS || ivalue == USER_POS)
            {

            }
            else if (ivalue == BUILTIN_HOME)
            {
                /* Put cursor at start of the entire text.
                * This may require scrolling the text.
                */
                if (custSPtr->index != 0)
                    _adjustCursor(dobj, textio, custSPtr, first, count, -custSPtr->index);
                _lineSetSelectStart(textio, custSPtr, custSPtr->index);
                _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
            }
            else if (ivalue == BUILTIN_END)
            {
                /* Put cursor at end of the entire text.
                * This may require scrolling the text.
                */
                int totalSize = _lineTotalSize(custSPtr);
                if (custSPtr->index != totalSize)
                    _adjustCursor(dobj, textio, custSPtr, first, count, totalSize - custSPtr->index);
                _lineSetSelectStart(textio, custSPtr, custSPtr->index);
                _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
            }
            else if (ivalue == NO_CURSOR && ALTIA_TEXT_SELECTEND(textio) != ALTIA_TEXT_SELECTSTART(textio))
            {
                /* Turn off any highlighted text. */
                _lineSetSelectStart(textio, custSPtr, custSPtr->index);
                _lineSetSelectEnd(textio, custSPtr, custSPtr->index);
            }
            _sendCursorPos(dobj, textio, custSPtr, first, count);
            ok = ALTIA_TRUE;
            ALTIA_SET_TEXT_CLEARNEXT(textio,ALTIA_FALSE);
        }
    }
    if (id == DECIMAL_ID)
    {
        ALTIA_SET_TEXT_DECPTS(textio,ALTIA_IVAL_VALUE(cindex));
    }
    if (id == SHORTCUT_ON_ID)
    {
        /* Always just set shortcut on/off and jump on/off if necessary. */
        switch (ivalue)
        {
        case -1:
            ALTIA_SET_TEXT_JUMPON(textio,ALTIA_FALSE);
        case  0:
            ALTIA_SET_TEXT_SHORTCUTON(textio,ALTIA_FALSE);
            break;

        case  2:
            ALTIA_SET_TEXT_JUMPON(textio,ALTIA_TRUE);
        case  1:
            ALTIA_SET_TEXT_SHORTCUTON(textio,ALTIA_TRUE);
            break;

        default:
            break;
        }
    }
    if (id == JUMP_ON_ID)
    {
        /* Always just set jump state on/off. */
        if (ivalue == 0)
        {
            ALTIA_SET_TEXT_JUMPON(textio,ALTIA_FALSE);
        }
        else
        {
            ALTIA_SET_TEXT_JUMPON(textio,ALTIA_TRUE);
        }
    }
    if (id == SELECT_ON_ID)
    {
        /* Always just set select state on/off. */
        if (ivalue == 0)
        {
            ALTIA_SET_TEXT_SELECTON(textio,ALTIA_FALSE);
        }
        else
        {
            ALTIA_SET_TEXT_SELECTON(textio,ALTIA_TRUE);
        }
    }
    if (id == LENGTH_MODE_ID)
    {
        ALTIA_SET_TEXT_LENGTHMODE(textio,ivalue);

        /* update the current length output */
        _sendLength(dobj, textio, custSPtr, first, count);
    }
    if (id == SELECT_NOW_ID)
    {
        /* If the cursor is on, now process SEL_NOW_MOUSE, SEL_NOW_ALL_MOUSE,
        * or SEL_NOW_ALL_END
        */
        ALTIA_SHORT selectMode = SELECT_ALL_END; /* By default,select to end. */
        switch (ivalue)
        {


        /* Select all and place cursor at the front or at the end. */
        case SEL_NOW_ALL_HOME:
            selectMode = SELECT_ALL_HOME;
        case SEL_NOW_ALL_END:
            if (_processScroll(dobj, textio, custSPtr, first, count, 1, ALTIA_FALSE, selectMode))
                ok = ALTIA_TRUE;
            break;

        default:
            break;
        }
    }
    if (id == IO_HLCOLOR_ID)
    {
        ALTIA_CHAR * hlcolor = ALTIA_TEXT_HLCOLOR(textio);
        if (hlcolor)
        {
            if (ivalue != '\0')
            {
                if (ALT_STRLEN(hlcolor) < MAX_COLOR_LEN-1)
                {
                    ALTIA_CHAR tmpChar[2];
                    tmpChar[0] = (ALTIA_CHAR) ivalue;
                    tmpChar[1] = '\0';
                    ALT_STRCAT(hlcolor, tmpChar);
                }
            }
            else if (hlcolor[0] != '\0')
            {
                int r, g, b;
                unsigned int hexrgb;

                if (ALT_SSCANF(hlcolor, ALT_TEXT("#%x"), &hexrgb) == 1)
                {
                    ALTIA_SET_TEXT_COLOR(textio,hexrgb);

                    /* we store colors by b, g, r */
                    r = (ALTIA_TEXT_COLOR(textio) >> 16) & 0xff;
                    g = (ALTIA_TEXT_COLOR(textio) >> 8) & 0xff;
                    b = (ALTIA_TEXT_COLOR(textio)) & 0xff;
                }
                else
                {
                    if (ALT_SSCANF(hlcolor, ALT_TEXT("%d %d %d"), &r, &g, &b) != 3)
                    {
                        /* Could set default color here. 
                        */
                        r = -1;
                        g = -1;
                        b = -1;
                    }
                }
                if (r >= 0 && g >= 0 && b >= 0)
                    ALTIA_SET_TEXT_COLOR(textio,(b << 16 | g << 8 | r));

                hlcolor[0] = '\0';

                ok = ALTIA_TRUE;
            }
        }
    }
    if (id == MAX_PIXEL_ID)
    {
        if (ivalue <= 0)
        {
            if (ALTIA_TEXT_MAX_PIXEL_COUNT(textio) > 0)
            {
                ALTIA_SET_TEXT_MAX_PIXEL_COUNT(textio,0);
                ok = ALTIA_TRUE;
            }
        }
        else
        {
            ALTIA_SET_TEXT_MAX_PIXEL_COUNT(textio,ivalue);
            ok = ALTIA_TRUE;
        }
    }

    if (ok)
    {
        /* Save the custS pointer for use in post-processing */
        ALTIA_SET_TEXT_CUSTS(textio, custSPtr);

        /* Post-process */
        ok = altiaLibTextioProcess(textio, dobj, ALTIA_TRUE);
    }

    return ok;
}

/*----------------------------------------------------------------------*/
void altiaLibTextioDraw(ALTIA_WINDOW win,
                        ALTIA_UINT32 dc,
                        Altia_GraphicState_type *gs,
                        ALTIA_INDEX textio,
                        ALTIA_INDEX ddata,
                        Altia_Transform_type *total)
{
    int idx = _altiaGetCustomSIndex(TEXT_ID, ALTIA_DOBJ_FIRSTFUNC(ddata), ALTIA_DOBJ_FUNCCNT(ddata));
    Altia_CustomSValue_type *custSPtr;
    int count = -1;

    if (ALTIA_TEXT_DISPSTR(textio) != nil)
        count = ALT_STRLEN(ALTIA_TEXT_DISPSTR(textio));

    if (idx < 0)
        return;

    custSPtr = &test_custS[idx];

    /* To be safe, no highlight if character count <= 0 */
    if (count > 0 && (ALTIA_TEXT_SELECTSTART(textio) != ALTIA_TEXT_SELECTEND(textio)))
    {
        ALTIA_COLOR saveColor = gs->foreground;
        ALTIA_INDEX fontIndex = gs->font;

        /* highlight by drawing rectangle behind text
         * in highlight color.
         */
        int width, height, offset;
        Altia_Rect_type srect;
        int x1 = ALTIA_TEXT_SELECTSTART(textio) - ALTIA_TEXT_START(textio);
        int x2 = ALTIA_TEXT_SELECTEND(textio) - ALTIA_TEXT_START(textio);

        gs->foreground = ALTIA_TEXT_COLOR(textio);
        if ((!((x1 <= 0) && (x2 <= 0)))
            && (!((x1 > count) && (x2 > count)))) 
        {
            if ((x1 <= 0) || (x2 <= 0)) 
            {
                if (x1 <= 0) 
                {
                    /* Get offset for exactly the first character,
                     * not just "i" because "i" may not have the
                     * same offset as the actual first character.
                     */
                    altiaLibFontExtent(&test_fonts[fontIndex],
                                       ALTIA_TEXT_DISPSTR(textio),
                                       1, &width, &height, &offset);
                    x1 = offset;
                    if (x2 > count) x2 = count;
                    altiaLibFontExtent(&test_fonts[fontIndex],
                                       ALTIA_TEXT_DISPSTR(textio),
                                       x2, &width, &height, &offset);
                    x2 = width;
                }
                else
                {
                    /* Get offset for exactly the first character,
                     * not just "i" because "i" may not have the
                     * same offset as the actual first character.
                     */
                    altiaLibFontExtent(&test_fonts[fontIndex],
                                       ALTIA_TEXT_DISPSTR(textio),
                                       1, &width, &height, &offset);
                    x2 = offset;
                    if (x1 > count) x1 = count;
                    altiaLibFontExtent(&test_fonts[fontIndex],
                                       ALTIA_TEXT_DISPSTR(textio),
                                       x1, &width, &height, &offset);
                    x1 = width;
                }
            }
            else
            {
                int minx;

                if (x1 > count)
                    x1 = count;
                else if (x2 > count)
                    x2 = count;
                minx = textio_min(x1, x2);
                altiaLibFontExtent(&test_fonts[fontIndex],
                                   ALTIA_TEXT_DISPSTR(textio),
                                   x1, &width, &height, &offset);
                /* A positive offset is only an issue when drawing
                 * at the 1st character.  When drawing after the first
                 * character, which is the situation here, a positive
                 * offset puts the highlight too far to the right.
                 * If offset is negative, apply it to x1 and x2.
                 */
                if (offset >= 0)
                    x1 = width;
                else
                    x1 = width + offset;
                altiaLibFontExtent(&test_fonts[fontIndex],
                                   ALTIA_TEXT_DISPSTR(textio),
                                   x2, &width, &height, &offset);
                if (offset >= 0)
                    x2 = width;
                else
                    x2 = width + offset;
            }
            srect.x0 = x1;
            srect.x1 = x2;
            srect.y0 = 0;
            srect.y1 = height;
            srect.filled = 1;
            _altiaLibRectDraw(win, dc, gs, &srect, 1, total);
        }
        gs->foreground = saveColor;
    }

    altiaLibLabelDraw(win, dc, gs, &test_labels[ALTIA_TEXT_LABELINDEX(textio)], total);

    /* Don't do any cursor manipulation if dispStr has not been
     * allocated yet (really only an issue when memory allocation is enabled).
     */
    if (ALTIA_TEXT_DISPSTR(textio) != nil
        && (ALTIA_TEXT_CURSOR(textio) == BUILTIN_LEFT 
            || ALTIA_TEXT_CURSOR(textio) == BUILTIN_POS
            || ALTIA_TEXT_CURSOR(textio) == BUILTIN_RIGHT
            || ALTIA_TEXT_CURSOR(textio) == BUILTIN_HOME
            || ALTIA_TEXT_CURSOR(textio) == BUILTIN_END))
    {
        ALTIA_COLOR saveColor = gs->foreground;
        ALTIA_INDEX fontIndex = gs->font;
        int index;
        int width, height, offset;
        Altia_Coord_type pts[2];

        /* Altia_CustomSValue_type *custSPtr; */
        /* We want the cursor to be the background color of the
         * textio object.
         */
        gs->foreground = ALTIA_DOBJ_BACKGROUND(ddata);
        index = custSPtr->index - ALTIA_TEXT_START(textio);
        if (index <= 0 || count == 0)
        { 
            /* If character count is > 0, get offset for exactly the first
             * character instead of "i" because "i" may not have the same
             * offset as the actual first character.
             */
            if (count == 0)
                altiaLibFontExtent(&test_fonts[fontIndex], ALT_TEXT("i"),
                                   1, &width, &height, &offset);
            else
                altiaLibFontExtent(&test_fonts[fontIndex], ALTIA_TEXT_DISPSTR(textio),
                                   1, &width, &height, &offset);
            pts[0].x = pts[1].x = offset;
            pts[0].y = 0;
            pts[1].y = height;
            altiaLibLineDraw(win, dc, gs, pts, 2, total);
        }else
        {
            if (count != -1 && index > count)
                index = count;
            altiaLibFontExtent(&test_fonts[fontIndex], ALTIA_TEXT_DISPSTR(textio),
                               index, &width, &height, &offset);
            /* A positive offset is only an issue when drawing at the 1st
             * character.  When drawing after the first character, which is
             * the situation here, a positive offset can put the cursor too
             * far to the right.  If the offset is negative, apply it so the
             * cursor is not too far to the right.
             */
            if (offset > 0)
                offset = 0;
            pts[0].x = pts[1].x = offset + width;
            pts[0].y = 0;
            pts[1].y = height;
            altiaLibLineDraw(win, dc, gs, pts, 2, total);
        }
        gs->foreground = saveColor;
    }
}





