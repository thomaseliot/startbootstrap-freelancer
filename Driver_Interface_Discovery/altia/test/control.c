/* $Revision: 1.44 $    $Date: 2010-04-01 18:37:38 $
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

/*
 * This file contains the control code from the design converted to
 * C code.  If the design has no control code or if no control code
 * was selected for code generation, this file will be empty.
 */


#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaControl.h"
#include "altiaData.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef NO_VARARGS
#if defined(WIN32) || defined(QNX) || defined(VXWORKS) || !defined(OLD_VARARGS)
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#endif

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

#define format printf

#if defined(UNDER_CE) && !defined(hypot)
#define hypot _hypot
#elif defined(VXWORKS)

#endif


extern ALTIA_BOOLEAN _altiaIsString(ALTIA_INDEX idx);

extern ALTIA_BOOLEAN altiaInitializingDesign;

extern void _altiaCheckSystem(void);










static Altia_WhenDelay_type _whenDelay[ALTIA_CNTLQ_SIZE];


static ALTIA_INT _delayTail = 0;
static ALTIA_INT _delayHead = 0;



static ALTIA_CHAR buffer[96];

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_altia2Str(AltiaEventType value)
{


    ALT_SPRINTF(buffer, ALT_TEXT("%d"), value);

    return buffer;
}

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_altia2Strf(AltiaEventType value)
{
    ALTIA_CHAR *ptr = _altia2Str(value);
    if (ALT_STRCHR(ptr, '.') == 0)
    {
        ALT_STRCAT(ptr, ALT_TEXT(".0"));
    }
    return ptr;
}

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_altiaGetTextRef(void *ptr)
{
    ALTIA_INDEX index;

    if (ptr == nil)
        return 0;

    index = _altiaFindAnimationIdx((ALTIA_CHAR*)ptr);
    if (index >= 0 && _altiaIsString(index))
    {
        /* Reference is to an animation name */
        return _altiaFindCurString((ALTIA_CHAR*)ptr);
    }
    else
    {
        return (ALTIA_CHAR*)ptr;
    }
}

/*----------------------------------------------------------------------*/
static AltiaEventType _altiaGetRef(void *ptr)
{
    ALTIA_INDEX index;

    if (ptr == nil)
        return 0;

    index = _altiaFindAnimationIdx((ALTIA_CHAR*)ptr);
    if (index >= 0)
    {
        /* Reference is to an animation name */
        return _altiaFindCurVal((ALTIA_CHAR*)ptr);
    }
    else
    {
        return *((AltiaEventType*)ptr);
    }
}

/*----------------------------------------------------------------------*/
static void _altiaSetTextRef(void *ptr, ALTIA_CHAR *text)
{
    ALTIA_INDEX index;

    if (ptr == nil)
        return;

    index = _altiaFindAnimationIdx((ALTIA_CHAR*)ptr);
    if (index >= 0 && _altiaIsString(index))
    {
        /* Reference is to an animation name */
        AltiaAnimateText((ALTIA_CHAR*)ptr, text);
    }
    else
    {
        ALT_STRCPY((ALTIA_CHAR*)ptr, text);
    }
}

/*----------------------------------------------------------------------*/
static void _altiaSetRef(void *ptr, AltiaEventType val)
{
    ALTIA_INDEX index;

    if (ptr == nil)
        return;

    index = _altiaFindAnimationIdx((ALTIA_CHAR*)ptr);
    if (index >= 0)
    {
        /* Reference is to an animation name */
        AltiaAnimate((ALTIA_CHAR*)ptr, val);
    }
    else
    {
        *((AltiaEventType*)ptr) = val;
    }
}

/*----------------------------------------------------------------------
 * This func is for when a set statement has 2 at signs.  Since
 * at code gen time we can't determine the types of the animations
 * we have to let this function do it.
 *----------------------------------------------------------------------*/
static void _altiaIndirect(ALTIA_CHAR *dest, ALTIA_CHAR *src)
{
    ALTIA_BOOLEAN srcstr = false;
    ALTIA_BOOLEAN deststr = false;
    ALTIA_INDEX destIdx;
    ALTIA_INDEX srcIdx;
    ALTIA_CHAR *destName = _altiaFindCurString(dest);
    ALTIA_CHAR *srcName = _altiaFindCurString(src);
    srcIdx = _altiaFindAnimationIdx(srcName);
    destIdx = _altiaFindAnimationIdx(destName);
    if (destIdx == -1)
        return;
    if (srcIdx == -1)
        return;
    if (_altiaIsString(srcIdx))
        srcstr = true;
    if (_altiaIsString(destIdx))
        deststr = true;
    if (srcstr && deststr)
    {
        AltiaAnimateText(destName, _altiaFindCurString(srcName));
    }
    else if (srcstr)
    {


        AltiaAnimate(destName, ALT_STRTOL(_altiaFindCurString(srcName), NULL, 0));

    }
    else if (deststr)
    {
        AltiaAnimateText(destName, _altia2Str(_altiaFindCurVal(srcName)));
    }
    else
    {
        AltiaAnimate(destName, _altiaFindCurVal(srcName));
    }
}

/*----------------------------------------------------------------------
 * This func is for when a compare statement has 2 at signs.  Since
 * at code gen time we can't determine the types of the animations
 * we have to let this function do it.
 *----------------------------------------------------------------------*/
static ALTIA_BOOLEAN _altiaCompIndirect(ALTIA_CHAR *op1, ALTIA_CHAR *op2)
{
    ALTIA_BOOLEAN res = false;
    ALTIA_BOOLEAN op1str = false;
    ALTIA_BOOLEAN op2str = false;
    ALTIA_INDEX op1Idx;
    ALTIA_INDEX op2Idx;
    ALTIA_CHAR *op1Name = _altiaFindCurString(op1);
    ALTIA_CHAR *op2Name = _altiaFindCurString(op2);
    op1Idx = _altiaFindAnimationIdx(op1Name);
    op2Idx = _altiaFindAnimationIdx(op2Name);
    if (op1Idx == -1)
        return false;
    if (op2Idx == -1)
        return false;
    if (_altiaIsString(op1Idx))
        op1str = true;
    if (_altiaIsString(op2Idx))
        op2str = true;
    if (op1str && op2str)
    {
        if (ALT_STRCMP(_altiaFindCurString(op1Name), 
                       _altiaFindCurString(op2Name)) == 0)
        {
            res = true;
        }
    }
    else if (op1str)
    {
        if (ALT_STRCMP(op1Name, _altia2Str(_altiaFindCurVal(op2Name))) == 0)
        {
            res = true;
        }
    }
    else if (op2str)
    {
        if (ALT_STRCMP(op2Name, _altia2Str(_altiaFindCurVal(op1Name))) == 0)
        {
            res = true;
        }
    }
    else
    {
        if (_altiaFindCurVal(op1Name) == _altiaFindCurVal(op2Name))
        {
            res = true;
        }
    }
    return res;
}

/*----------------------------------------------------------------------
 * This func is for when a statement has an at sign on the left hand side.
 * and a value on the right. This function determines the type of the left
 * hand side and gives it the value.
 *----------------------------------------------------------------------*/
static void _altiaIndirectVal(ALTIA_CHAR *dest, AltiaEventType srcVal,
                              ALTIA_CHAR *srcText)
{
    ALTIA_INDEX destIdx;
    ALTIA_CHAR *destName = _altiaFindCurString(dest);
    destIdx = _altiaFindAnimationIdx(destName);
    if (destIdx == -1)
        return;
    if (srcText != NULL)
    {
        AltiaAnimateText(destName, srcText);
    }
    else if (_altiaIsString(destIdx))
    {
        AltiaAnimateText(destName, _altia2Str(srcVal));
    }
    else 
    {
        AltiaAnimate(destName, srcVal);
    }
}

/*----------------------------------------------------------------------
 * This func is for when a statement has a control variable with an at
 * sign on the left side and a value on the right. This function determines
 * the type of the left hand side and gives it the value.
 *----------------------------------------------------------------------*/
static void _altiaDirectVal(ALTIA_CHAR *dest, AltiaEventType srcVal,
                              ALTIA_CHAR *srcText)
{
    ALTIA_INDEX destIdx;

    destIdx = _altiaFindAnimationIdx(dest);
    if (destIdx == -1)
        return;

    if (srcText != NULL)
    {
        AltiaAnimateText(dest, srcText);
    }
    else if (_altiaIsString(destIdx))
    {
        AltiaAnimateText(dest, _altia2Str(srcVal));
    }
    else 
    {
        AltiaAnimate(dest, srcVal);
    }
}

/*----------------------------------------------------------------------
 * This func is needed since altia control code interpreter will
 * do integer or floating point division based upon the operands so
 * so we have to do the same.
 *----------------------------------------------------------------------*/
static AltiaEventType _altiaDivOp(AltiaEventType v1, AltiaEventType v2)
{
    int temp = (int)v1;
    int temp2 = (int)v2;
    AltiaEventType etemp = temp;
    if (ALTIA_EQUAL(etemp, v1))
    {
        /* v1 appears to be an int */
        etemp = temp2;
        if (ALTIA_EQUAL(etemp, v2))
        {
            return temp / temp2;
        }
    }
    return v1 / v2;
}

/*----------------------------------------------------------------------
 * This func is needed since altia control code interpreter will
 * do convert operands to ints and do the mod so we have to do the same.
 *----------------------------------------------------------------------*/
static AltiaEventType _altiaModOp(AltiaEventType v1, AltiaEventType v2)
{
    int temp = (int)v1;
    int temp2 = (int)v2;
    return temp % temp2;
}

/*----------------------------------------------------------------------*/
static int _altiaInt(AltiaEventType value)
{
    int res = (int) value;
    return res;
}

/*----------------------------------------------------------------------*/
static ALTIA_DOUBLE _altiaDouble(AltiaEventType value)
{
    ALTIA_DOUBLE res = value;
    return res;
}

/*----------------------------------------------------------------------*/
static ALTIA_CHAR *_altiaRemoveZeros(ALTIA_CHAR *str)
{
    /* Remove zeros trailing a decimal point */
    AltiaEventType val;
    register ALTIA_CHAR *p;
    ALTIA_BOOLEAN decimalPt = false;

    /* Look for a decimal point */
    for (p = str; *p != 0; p++)
    {
        if (*p == '.')
        {
            decimalPt = true;
        }
        else if (ALT_ISALPHA((unsigned)(*p)))
        {
            return str;
        }
        else
        {
            if (decimalPt && ALT_ISSPACE((unsigned)(*p)))
            {
                return str;
            }
        }
    }

    if (decimalPt)
    {
        if (altiaLibIsNumber(str, &val))
        {
            return _altia2Strf(val);
        }
    }

    return str;
}



/*----------------------------------------------------------------------*/
#ifndef NO_VARARGS
#if defined(WIN32)

static int scan(ALTIA_CHAR *string, ALTIA_CHAR *format, ... )

#elif defined(QNX) || defined(VXWORKS) || !defined(OLD_VARARGS)

static int scan(ALTIA_CHAR *string, ALTIA_CHAR *format, ... )

#else

static int scan(string, format, va_alist)
ALTIA_CHAR *string;
ALTIA_CHAR *format;
va_dcl

#endif
{
    ALTIA_CHAR *str[6];
    ALTIA_CHAR str1[64];
    ALTIA_CHAR str2[64];
    ALTIA_CHAR str3[64];
    ALTIA_CHAR str4[64];
    ALTIA_CHAR str5[64];
    ALTIA_CHAR str6[64];
    int ivar[12];
    ALTIA_FLOAT dvar[12];
    VArg_t vptrs[12];
    int scnt = 0;
    int icnt = 0;
    int dcnt = 0;
    int pcnt = 0;
    int res;
    va_list marker;
    ALTIA_CHAR *ptr;
    ALTIA_BOOLEAN nextpercent = false;
    int i;

    str[0] = str1;
    str[1] = str2;
    str[2] = str3;
    str[3] = str4;
    str[4] = str5;
    str[5] = str6;

    ptr = ALT_STRCHR(format, '%');
    while (ptr != NULL && *ptr != 0)
    {
        if (nextpercent)
        {
            ptr++;
            while (*ptr != '%' && *ptr != 0)
                ptr++;
            if (*ptr == '%')
                ptr++;
            nextpercent = false;
        }else
            ptr++;
        switch (*ptr)
        {
        case 0:
            continue;
        case '%':
            nextpercent = true;
            break;
        case 'd':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
        case 'n':
        case 'c':
            if (pcnt > 11 || icnt > 11)
                return 0;
            ivar[icnt] = 0;
            vptrs[pcnt].p = &ivar[icnt++];
            vptrs[pcnt++].vtype = 0;
            nextpercent = true;
            break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
            if (pcnt > 11 || dcnt > 11)
                return 0;
            dvar[dcnt] = (ALTIA_FLOAT) 0;
            vptrs[pcnt].p = &dvar[dcnt++];
            vptrs[pcnt++].vtype = 1;
            nextpercent = true;
            break;
        case 's':
            if (pcnt > 11 || scnt > 5)
                return 0;
            *(str[scnt]) = 0;
            vptrs[pcnt].p = str[scnt++];
            vptrs[pcnt++].vtype = 2;
            nextpercent = true;
            break;
        default:
            break;
        }
    }
    if (pcnt == 0)
        return 0;
    switch (pcnt)
    {
    case 1:
        res = ALT_SSCANF(string, format, vptrs[0].p);
        break;
    case 2:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p);
        break;
    case 3:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p);
        break;
    case 4:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p);
        break;
    case 5:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p);
        break;
    case 6:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p);
        break;
    case 7:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p);
        break;
    case 8:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p, vptrs[7].p);
        break;
    case 9:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p, vptrs[7].p, vptrs[8].p);
        break;
    case 10:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p, vptrs[7].p, vptrs[8].p, vptrs[9].p);
        break;
    case 11:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p, vptrs[7].p, vptrs[8].p, vptrs[9].p,
            vptrs[10].p);
        break;
    case 12:
        res = ALT_SSCANF(string, format, vptrs[0].p, vptrs[1].p, 
            vptrs[2].p, vptrs[3].p, vptrs[4].p, vptrs[5].p, 
            vptrs[6].p, vptrs[7].p, vptrs[8].p, vptrs[9].p,
            vptrs[10].p, vptrs[11].p);
        break;
    default:
        return 0;
    }

#if defined(WIN32) || defined(QNX) || defined(VXWORKS) || !defined(OLD_VARARGS)
    va_start(marker, format);
#else
    va_start(marker);
#endif
    for (i = 0; i < pcnt; i++)
    {
        AltiaEventType *vptr;
        ALTIA_CHAR *resptr = va_arg(marker, ALTIA_CHAR *);
        int globalFlag = va_arg(marker, int);
        if (resptr != NULL)
        {
            switch (vptrs[i].vtype)
            {
            case 0:
                if (globalFlag)
                {
                    vptr = (AltiaEventType*)resptr;
                    *vptr = *((int*)vptrs[i].p);
                }else
                    AltiaAnimate(resptr, 
                        (AltiaEventType)*((int*)vptrs[i].p));
                break;
            case 1:
                if (globalFlag)
                {
                    vptr = (AltiaEventType*)resptr;
                    *vptr = *((float*)vptrs[i].p);
                }else
                    AltiaAnimate(resptr, 
                        (AltiaEventType)*((float*)vptrs[i].p));
                break;
            case 2:
                if (globalFlag)
                {
                    ALT_STRCPY(resptr, (ALTIA_CHAR*)vptrs[i].p);
                }else
                    AltiaAnimateText(resptr, (ALTIA_CHAR *)vptrs[i].p);
                break;
            }
        }
    }
    va_end(marker);
    return res;
}
#endif /* NO_VARARGS */

/*----------------------------------------------------------------------
 * Next function is here to test all of returns and parameters
 * of the static functions defined above.  This function is
 * never called, but it provides a test of the calling syntax
 * for these static functions during compile time.  A side-effect
 * is that it also satisfies stringent compilers that the
 * static functions are used.
 *----------------------------------------------------------------------*/
void _altiaControlTestStaticFuncs(void)
{
    ALTIA_CHAR *charRet;
    AltiaEventType eventRet;
    ALTIA_BOOLEAN  boolRet;
    int intRet;
    ALTIA_DOUBLE doubleRet;


    charRet   = _altia2Str((AltiaEventType) 0);
    charRet   = _altia2Strf((AltiaEventType) 0);
    charRet   = _altiaGetTextRef(NULL);
    eventRet  = _altiaGetRef(NULL);
                _altiaSetTextRef(NULL, NULL);
                _altiaSetRef(NULL, (AltiaEventType) 0);
                _altiaIndirect(NULL, NULL);
    boolRet   = _altiaCompIndirect(NULL, NULL);
                _altiaIndirectVal(NULL, (AltiaEventType) 0, NULL);
                _altiaDirectVal(NULL, (AltiaEventType) 0, NULL);
    eventRet  = _altiaDivOp((AltiaEventType) 0, (AltiaEventType) 0);
    eventRet  = _altiaModOp((AltiaEventType) 0, (AltiaEventType) 0);
    intRet    = _altiaInt((AltiaEventType) 0);
    doubleRet = _altiaDouble((AltiaEventType) 0);
    charRet   = _altiaRemoveZeros(NULL);



#ifndef NO_VARARGS
    intRet    = scan(NULL, NULL);
#endif
}

/*****************************************************************************
 * Externs
 ****************************************************************************/
 


/*****************************************************************************
 * Forward Declarations
 ****************************************************************************/
AltiaEventType  _status_bar_decrement_event(AltiaEventType  _status_bar_dec_card);

AltiaEventType  _status_bar_increment_event(AltiaEventType  _status_bar_inc_card);

AltiaEventType  _status_bar_change_card(AltiaEventType  _change_val);

AltiaEventType  _status_bar_toggle_timer(void);

AltiaEventType  _dash_home_decrement_event(AltiaEventType  _dash_home_dec_card);

AltiaEventType  _dash_home_increment_event(AltiaEventType  _dash_home_inc_card);

AltiaEventType  _dash_home_change_card(AltiaEventType  _change_val);

AltiaEventType  _dash_home_toggle_timer(void);




/*****************************************************************************
 * Routines
 ****************************************************************************/
AltiaEventType  _status_bar_decrement_event(AltiaEventType  _status_bar_dec_card)
{

    // "decrement handler that manages bounds checking for user input values"
    /* EXPR status_bar_dec_change_val = -1*status_bar_dec_card */
    AltiaAnimate(ALT_TEXT("status_bar_dec_change_val"),- (AltiaEventType) 1* _status_bar_dec_card);
    /* EXPR status_bar_dec_bounds_test = status_bar_card + status_bar_dec_change_val */
    AltiaAnimate(ALT_TEXT("status_bar_dec_bounds_test"), _altiaFindCurVal(ALT_TEXT("status_bar_card"))+  _altiaFindCurVal(ALT_TEXT("status_bar_dec_change_val")));
    if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) > _altiaFindCurVal(ALT_TEXT("status_bar_min_card")))
    {
        // "if not yet at minimum"
        if (_altiaFindCurVal(ALT_TEXT("status_bar_dec_bounds_test")) > _altiaFindCurVal(ALT_TEXT("status_bar_min_card")))
        {
            // "if this change will not put us past the minimum"
            _status_bar_change_card( _altiaFindCurVal(ALT_TEXT("status_bar_dec_change_val")));
        }
        else
        {
            // "if it would have gone past simply set it to minimum"
            /* SET status_bar_card = status_bar_min_card */
            AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_min_card")));
        }
    }
    else
    {
        // "wrap"
        /* SET status_bar_card = status_bar_max_card */
        AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_max_card")));
    }
    return (AltiaEventType )0;
}
AltiaEventType  _status_bar_increment_event(AltiaEventType  _status_bar_inc_card)
{

    // "increment handler that manages bounds checking for user input values"
    /* EXPR status_bar_inc_change_val = status_bar_inc_card */
    AltiaAnimate(ALT_TEXT("status_bar_inc_change_val"), _status_bar_inc_card);
    if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) < _altiaFindCurVal(ALT_TEXT("status_bar_max_card")))
    {
        // "if not yet maximum"
        /* EXPR status_bar_inc_bounds_test = status_bar_card + status_bar_inc_change_val */
        AltiaAnimate(ALT_TEXT("status_bar_inc_bounds_test"), _altiaFindCurVal(ALT_TEXT("status_bar_card"))+  _altiaFindCurVal(ALT_TEXT("status_bar_inc_change_val")));
        if (_altiaFindCurVal(ALT_TEXT("status_bar_inc_bounds_test")) < _altiaFindCurVal(ALT_TEXT("status_bar_max_card")))
        {
            // "if this change will not put us past the maximum"
            _status_bar_change_card( _altiaFindCurVal(ALT_TEXT("status_bar_inc_change_val")));
        }
        else
        {
            // "if it would have gone past simply set it to maximum"
            /* SET status_bar_card = status_bar_max_card */
            AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_max_card")));
        }
    }
    else
    {
        // "wrap"
        /* SET status_bar_card = status_bar_min_card */
        AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_min_card")));
    }
    return (AltiaEventType )0;
}

AltiaEventType  _status_bar_change_card(AltiaEventType  _change_val)
{

    // "quickly handles inc or dec of card animation"
    /* EXPR status_bar_card = status_bar_card + change_val */
    AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_card"))+  _change_val);
    return (AltiaEventType )0;
}

AltiaEventType  _status_bar_toggle_timer(void)
{

    if (_altiaFindCurVal(ALT_TEXT("status_bar_startTimer")) != 1)
    {
        /* SET status_bar_startTimer = 1 */
        AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)1);
    }
    else
    {
        /* SET status_bar_startTimer = 0 */
        AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
    }
    return (AltiaEventType )0;
}

AltiaEventType  _dash_home_decrement_event(AltiaEventType  _dash_home_dec_card)
{

    // "decrement handler that manages bounds checking for user input values"
    /* EXPR dash_home_dec_change_val = -1*dash_home_dec_card */
    AltiaAnimate(ALT_TEXT("dash_home_dec_change_val"),- (AltiaEventType) 1* _dash_home_dec_card);
    /* EXPR dash_home_dec_bounds_test = dash_home_card + dash_home_dec_change_val */
    AltiaAnimate(ALT_TEXT("dash_home_dec_bounds_test"), _altiaFindCurVal(ALT_TEXT("dash_home_card"))+  _altiaFindCurVal(ALT_TEXT("dash_home_dec_change_val")));
    if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) > _altiaFindCurVal(ALT_TEXT("dash_home_min_card")))
    {
        // "if not yet at minimum"
        if (_altiaFindCurVal(ALT_TEXT("dash_home_dec_bounds_test")) > _altiaFindCurVal(ALT_TEXT("dash_home_min_card")))
        {
            // "if this change will not put us past the minimum"
            _dash_home_change_card( _altiaFindCurVal(ALT_TEXT("dash_home_dec_change_val")));
        }
        else
        {
            // "if it would have gone past simply set it to minimum"
            /* SET dash_home_card = dash_home_min_card */
            AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_min_card")));
        }
    }
    else
    {
        // "wrap"
        /* SET dash_home_card = dash_home_max_card */
        AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_max_card")));
    }
    return (AltiaEventType )0;
}

AltiaEventType  _dash_home_increment_event(AltiaEventType  _dash_home_inc_card)
{

    // "increment handler that manages bounds checking for user input values"
    /* EXPR dash_home_inc_change_val = dash_home_inc_card */
    AltiaAnimate(ALT_TEXT("dash_home_inc_change_val"), _dash_home_inc_card);
    if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) < _altiaFindCurVal(ALT_TEXT("dash_home_max_card")))
    {
        // "if not yet maximum"
        /* EXPR dash_home_inc_bounds_test = dash_home_card + dash_home_inc_change_val */
        AltiaAnimate(ALT_TEXT("dash_home_inc_bounds_test"), _altiaFindCurVal(ALT_TEXT("dash_home_card"))+  _altiaFindCurVal(ALT_TEXT("dash_home_inc_change_val")));
        if (_altiaFindCurVal(ALT_TEXT("dash_home_inc_bounds_test")) < _altiaFindCurVal(ALT_TEXT("dash_home_max_card")))
        {
            // "if this change will not put us past the maximum"
            _dash_home_change_card( _altiaFindCurVal(ALT_TEXT("dash_home_inc_change_val")));
        }
        else
        {
            // "if it would have gone past simply set it to maximum"
            /* SET dash_home_card = dash_home_max_card */
            AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_max_card")));
        }
    }
    else
    {
        // "wrap"
        /* SET dash_home_card = dash_home_min_card */
        AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_min_card")));
    }
    return (AltiaEventType )0;
}

AltiaEventType  _dash_home_change_card(AltiaEventType  _change_val)
{

    // "quickly handles inc or dec of card animation"
    /* EXPR dash_home_card = dash_home_card + change_val */
    AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_card"))+  _change_val);
    return (AltiaEventType )0;
}

AltiaEventType  _dash_home_toggle_timer(void)
{

    if (_altiaFindCurVal(ALT_TEXT("dash_home_startTimer")) != 1)
    {
        /* SET dash_home_startTimer = 1 */
        AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)1);
    }
    else
    {
        /* SET dash_home_startTimer = 0 */
        AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
    }
    return (AltiaEventType )0;
}



/*****************************************************************************
 * When Statements
 ****************************************************************************/
ALTIA_BOOLEAN when_GUI_ctrl_block_label_set_style_0(AltiaEventType __AltiaValue__)
{
    /* SET GUI_ctrl_block_label_float = GUI_ctrl_block_label_float */
    AltiaAnimate(ALT_TEXT("GUI_ctrl_block_label_float"), _altiaFindCurVal(ALT_TEXT("GUI_ctrl_block_label_float")));
    return ALTIA_TRUE;
}

// " "
// "==status_bar==Deck====Begin Control code "

// "==========================================================================="

// "PhotoProto button inputs"
ALTIA_BOOLEAN when_status_bar_deckButtonInput_1(AltiaEventType __AltiaValue__)
{
    // "if timer toggle"
    if ((_altiaFindCurVal(ALT_TEXT("status_bar_timerToggler")) == 1) && (_altiaFindCurVal(ALT_TEXT("status_bar_slider_or_knob_flag")) != 1))
    {
        _status_bar_toggle_timer();
    }
    else
    {
        // "if some flavor of regular non timer toggle button press"
        if (_altiaFindCurVal(ALT_TEXT("status_bar_deckButtonInput")) == 3)
        {
            // "if jump to card press"
            /* SET status_bar_jump_to_card = status_bar_pointingToLayer */
            AltiaAnimate(ALT_TEXT("status_bar_jump_to_card"), _altiaFindCurVal(ALT_TEXT("status_bar_pointingToLayer")));
        }
        else
        {
            /* SET status_bar_timer_event = 1 */
            AltiaAnimate(ALT_TEXT("status_bar_timer_event"), (AltiaEventType)1);
        }
    }
    return ALTIA_TRUE;
}

// "{}"
// "deck handler routines"
// "{}"

// "FlowProto and common deck entry points"

ALTIA_BOOLEAN when_status_bar_jump_to_card_2(AltiaEventType __AltiaValue__)
{
    /* SET status_bar_card = status_bar_jump_to_card */
    AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_jump_to_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_dec_card_3(AltiaEventType __AltiaValue__)
{
    _status_bar_decrement_event( _altiaFindCurVal(ALT_TEXT("status_bar_dec_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_inc_card_4(AltiaEventType __AltiaValue__)
{
    _status_bar_increment_event( _altiaFindCurVal(ALT_TEXT("status_bar_inc_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_loop_5(AltiaEventType __AltiaValue__)
{
    // "begin looping"
    /* SET status_bar_looptype = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_looptype"), (AltiaEventType)0);
    /* SET status_bar_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
    /* SET status_bar_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_once_6(AltiaEventType __AltiaValue__)
{
    // "begin play once"
    /* SET status_bar_looptype = 1 */
    AltiaAnimate(ALT_TEXT("status_bar_looptype"), (AltiaEventType)1);
    /* SET status_bar_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
    /* SET status_bar_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_pingpong_7(AltiaEventType __AltiaValue__)
{
    // "begin play once"
    /* SET status_bar_looptype = 2 */
    AltiaAnimate(ALT_TEXT("status_bar_looptype"), (AltiaEventType)2);
    /* SET status_bar_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
    /* SET status_bar_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_status_bar_stop_animation_8(AltiaEventType __AltiaValue__)
{
    // "stop timer"
    /* SET status_bar_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
    return ALTIA_TRUE;
}

// "{}"
// "timer event"
ALTIA_BOOLEAN when_status_bar_timer_event_9(AltiaEventType __AltiaValue__)
{
    /* SET status_bar_slider_or_knob_flag = 0 */
    AltiaAnimate(ALT_TEXT("status_bar_slider_or_knob_flag"), (AltiaEventType)0);
    if (_altiaFindCurVal(ALT_TEXT("status_bar_looptype")) == 0)
    {
        // "loop animation event"
        if ((_altiaFindCurVal(ALT_TEXT("status_bar_pointerLoop")) == 2) || (_altiaFindCurVal(ALT_TEXT("status_bar_deckButtonInput")) == 2))
        {
            /* SET status_bar_dec_card = 1 */
            AltiaAnimate(ALT_TEXT("status_bar_dec_card"), (AltiaEventType)1);
        }
        else
        {
            /* SET status_bar_inc_card = 1 */
            AltiaAnimate(ALT_TEXT("status_bar_inc_card"), (AltiaEventType)1);
        }
    }
    else if (_altiaFindCurVal(ALT_TEXT("status_bar_looptype")) == 1)
    {
        // "once animation event"
        if ((_altiaFindCurVal(ALT_TEXT("status_bar_pointerLoop")) == 2) || (_altiaFindCurVal(ALT_TEXT("status_bar_deckButtonInput")) == 2))
        {
            // "decrement"
            if ((_altiaFindCurVal(ALT_TEXT("status_bar_once_animation_done")) == 1) && (_altiaFindCurVal(ALT_TEXT("status_bar_startTimer")) == 1))
            {
                // "reset animation for timer click event"
                /* SET status_bar_card = status_bar_max_card */
                AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_max_card")));
                /* SET status_bar_once_animation_done = 0 */
                AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)0);
            }
            else
            {
                if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) > _altiaFindCurVal(ALT_TEXT("status_bar_min_card")))
                {
                    /* SET status_bar_once_animation_done = 0 */
                    AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)0);
                    _status_bar_change_card( (AltiaEventType) -1);
                    if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) == _altiaFindCurVal(ALT_TEXT("status_bar_min_card")))
                    {
                        /* SET status_bar_startTimer = 0 */
                        AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
                        /* SET status_bar_once_animation_done = 1 */
                        AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)1);
                    }
                }
            }
        }
        else
        {
            // "increment"
            if ((_altiaFindCurVal(ALT_TEXT("status_bar_once_animation_done")) == 1) && (_altiaFindCurVal(ALT_TEXT("status_bar_startTimer")) == 1))
            {
                // "reset animation for timer click event"
                /* SET status_bar_card = status_bar_min_card */
                AltiaAnimate(ALT_TEXT("status_bar_card"), _altiaFindCurVal(ALT_TEXT("status_bar_min_card")));
                /* SET status_bar_once_animation_done = 0 */
                AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)0);
            }
            else
            {
                if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) < _altiaFindCurVal(ALT_TEXT("status_bar_max_card")))
                {
                    /* SET status_bar_once_animation_done = 0 */
                    AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)0);
                    _status_bar_change_card( (AltiaEventType) 1);
                    // "check to see if that is the end"
                    if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) == _altiaFindCurVal(ALT_TEXT("status_bar_max_card")))
                    {
                        /* SET status_bar_startTimer = 0 */
                        AltiaAnimate(ALT_TEXT("status_bar_startTimer"), (AltiaEventType)0);
                        /* SET status_bar_once_animation_done = 1 */
                        AltiaAnimate(ALT_TEXT("status_bar_once_animation_done"), (AltiaEventType)1);
                    }
                }
            }
        }
    }
    else if (_altiaFindCurVal(ALT_TEXT("status_bar_looptype")) == 2)
    {
        // "pingpong animation event"
        if (_altiaFindCurVal(ALT_TEXT("status_bar_pongDirection")) == 1)
        {
            if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) < _altiaFindCurVal(ALT_TEXT("status_bar_max_card")))
            {
                _status_bar_change_card( (AltiaEventType) 1);
            }
            else
            {
                /* SET status_bar_pongDirection = -1 */
                AltiaAnimate(ALT_TEXT("status_bar_pongDirection"), (AltiaEventType)-1);
                _status_bar_change_card( (AltiaEventType) -1);
            }
        }
        else if (_altiaFindCurVal(ALT_TEXT("status_bar_pongDirection")) == -1)
        {
            if (_altiaFindCurVal(ALT_TEXT("status_bar_card")) > _altiaFindCurVal(ALT_TEXT("status_bar_min_card")))
            {
                _status_bar_change_card( (AltiaEventType) -1);
            }
            else
            {
                /* SET status_bar_pongDirection = 1 */
                AltiaAnimate(ALT_TEXT("status_bar_pongDirection"), (AltiaEventType)1);
                _status_bar_change_card( (AltiaEventType) 1);
            }
        }
    }
    return ALTIA_TRUE;
}

// " "
// "==dash_home==Deck====Begin Control code "

// "==========================================================================="

// "PhotoProto button inputs"
ALTIA_BOOLEAN when_dash_home_deckButtonInput_10(AltiaEventType __AltiaValue__)
{
    // "if timer toggle"
    if ((_altiaFindCurVal(ALT_TEXT("dash_home_timerToggler")) == 1) && (_altiaFindCurVal(ALT_TEXT("dash_home_slider_or_knob_flag")) != 1))
    {
        _dash_home_toggle_timer();
    }
    else
    {
        // "if some flavor of regular non timer toggle button press"
        if (_altiaFindCurVal(ALT_TEXT("dash_home_deckButtonInput")) == 3)
        {
            // "if jump to card press"
            /* SET dash_home_jump_to_card = dash_home_pointingToLayer */
            AltiaAnimate(ALT_TEXT("dash_home_jump_to_card"), _altiaFindCurVal(ALT_TEXT("dash_home_pointingToLayer")));
        }
        else
        {
            /* SET dash_home_timer_event = 1 */
            AltiaAnimate(ALT_TEXT("dash_home_timer_event"), (AltiaEventType)1);
        }
    }
    return ALTIA_TRUE;
}

// "{}"
// "deck handler routines"
// "{}"

// "FlowProto and common deck entry points"

ALTIA_BOOLEAN when_dash_home_jump_to_card_11(AltiaEventType __AltiaValue__)
{
    /* SET dash_home_card = dash_home_jump_to_card */
    AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_jump_to_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_dec_card_12(AltiaEventType __AltiaValue__)
{
    _dash_home_decrement_event( _altiaFindCurVal(ALT_TEXT("dash_home_dec_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_inc_card_13(AltiaEventType __AltiaValue__)
{
    _dash_home_increment_event( _altiaFindCurVal(ALT_TEXT("dash_home_inc_card")));
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_loop_14(AltiaEventType __AltiaValue__)
{
    // "begin looping"
    /* SET dash_home_looptype = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_looptype"), (AltiaEventType)0);
    /* SET dash_home_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
    /* SET dash_home_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_once_15(AltiaEventType __AltiaValue__)
{
    // "begin play once"
    /* SET dash_home_looptype = 1 */
    AltiaAnimate(ALT_TEXT("dash_home_looptype"), (AltiaEventType)1);
    /* SET dash_home_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
    /* SET dash_home_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_pingpong_16(AltiaEventType __AltiaValue__)
{
    // "begin play once"
    /* SET dash_home_looptype = 2 */
    AltiaAnimate(ALT_TEXT("dash_home_looptype"), (AltiaEventType)2);
    /* SET dash_home_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
    /* SET dash_home_startTimer = 1 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)1);
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN when_dash_home_stop_animation_17(AltiaEventType __AltiaValue__)
{
    // "stop timer"
    /* SET dash_home_startTimer = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
    return ALTIA_TRUE;
}

// "{}"
// "timer event"
ALTIA_BOOLEAN when_dash_home_timer_event_18(AltiaEventType __AltiaValue__)
{
    /* SET dash_home_slider_or_knob_flag = 0 */
    AltiaAnimate(ALT_TEXT("dash_home_slider_or_knob_flag"), (AltiaEventType)0);
    if (_altiaFindCurVal(ALT_TEXT("dash_home_looptype")) == 0)
    {
        // "loop animation event"
        if ((_altiaFindCurVal(ALT_TEXT("dash_home_pointerLoop")) == 2) || (_altiaFindCurVal(ALT_TEXT("dash_home_deckButtonInput")) == 2))
        {
            /* SET dash_home_dec_card = 1 */
            AltiaAnimate(ALT_TEXT("dash_home_dec_card"), (AltiaEventType)1);
        }
        else
        {
            /* SET dash_home_inc_card = 1 */
            AltiaAnimate(ALT_TEXT("dash_home_inc_card"), (AltiaEventType)1);
        }
    }
    else if (_altiaFindCurVal(ALT_TEXT("dash_home_looptype")) == 1)
    {
        // "once animation event"
        if ((_altiaFindCurVal(ALT_TEXT("dash_home_pointerLoop")) == 2) || (_altiaFindCurVal(ALT_TEXT("dash_home_deckButtonInput")) == 2))
        {
            // "decrement"
            if ((_altiaFindCurVal(ALT_TEXT("dash_home_once_animation_done")) == 1) && (_altiaFindCurVal(ALT_TEXT("dash_home_startTimer")) == 1))
            {
                // "reset animation for timer click event"
                /* SET dash_home_card = dash_home_max_card */
                AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_max_card")));
                /* SET dash_home_once_animation_done = 0 */
                AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)0);
            }
            else
            {
                if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) > _altiaFindCurVal(ALT_TEXT("dash_home_min_card")))
                {
                    /* SET dash_home_once_animation_done = 0 */
                    AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)0);
                    _dash_home_change_card( (AltiaEventType) -1);
                    if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) == _altiaFindCurVal(ALT_TEXT("dash_home_min_card")))
                    {
                        /* SET dash_home_startTimer = 0 */
                        AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
                        /* SET dash_home_once_animation_done = 1 */
                        AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)1);
                    }
                }
            }
        }
        else
        {
            // "increment"
            if ((_altiaFindCurVal(ALT_TEXT("dash_home_once_animation_done")) == 1) && (_altiaFindCurVal(ALT_TEXT("dash_home_startTimer")) == 1))
            {
                // "reset animation for timer click event"
                /* SET dash_home_card = dash_home_min_card */
                AltiaAnimate(ALT_TEXT("dash_home_card"), _altiaFindCurVal(ALT_TEXT("dash_home_min_card")));
                /* SET dash_home_once_animation_done = 0 */
                AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)0);
            }
            else
            {
                if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) < _altiaFindCurVal(ALT_TEXT("dash_home_max_card")))
                {
                    /* SET dash_home_once_animation_done = 0 */
                    AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)0);
                    _dash_home_change_card( (AltiaEventType) 1);
                    // "check to see if that is the end"
                    if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) == _altiaFindCurVal(ALT_TEXT("dash_home_max_card")))
                    {
                        /* SET dash_home_startTimer = 0 */
                        AltiaAnimate(ALT_TEXT("dash_home_startTimer"), (AltiaEventType)0);
                        /* SET dash_home_once_animation_done = 1 */
                        AltiaAnimate(ALT_TEXT("dash_home_once_animation_done"), (AltiaEventType)1);
                    }
                }
            }
        }
    }
    else if (_altiaFindCurVal(ALT_TEXT("dash_home_looptype")) == 2)
    {
        // "pingpong animation event"
        if (_altiaFindCurVal(ALT_TEXT("dash_home_pongDirection")) == 1)
        {
            if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) < _altiaFindCurVal(ALT_TEXT("dash_home_max_card")))
            {
                _dash_home_change_card( (AltiaEventType) 1);
            }
            else
            {
                /* SET dash_home_pongDirection = -1 */
                AltiaAnimate(ALT_TEXT("dash_home_pongDirection"), (AltiaEventType)-1);
                _dash_home_change_card( (AltiaEventType) -1);
            }
        }
        else if (_altiaFindCurVal(ALT_TEXT("dash_home_pongDirection")) == -1)
        {
            if (_altiaFindCurVal(ALT_TEXT("dash_home_card")) > _altiaFindCurVal(ALT_TEXT("dash_home_min_card")))
            {
                _dash_home_change_card( (AltiaEventType) -1);
            }
            else
            {
                /* SET dash_home_pongDirection = 1 */
                AltiaAnimate(ALT_TEXT("dash_home_pongDirection"), (AltiaEventType)1);
                _dash_home_change_card( (AltiaEventType) 1);
            }
        }
    }
    return ALTIA_TRUE;
}



/*****************************************************************************
 * When Function Table
 ****************************************************************************/
ALTIA_INT test_whenFuncs_count = 19;
Altia_WhenFunc_type test_whenFuncs[] = 
{
when_GUI_ctrl_block_label_set_style_0,when_status_bar_deckButtonInput_1,
when_status_bar_jump_to_card_2,when_status_bar_dec_card_3,
when_status_bar_inc_card_4,when_status_bar_loop_5,
when_status_bar_once_6,when_status_bar_pingpong_7,
when_status_bar_stop_animation_8,when_status_bar_timer_event_9,
when_dash_home_deckButtonInput_10,when_dash_home_jump_to_card_11,
when_dash_home_dec_card_12,when_dash_home_inc_card_13,
when_dash_home_loop_14,when_dash_home_once_15,
when_dash_home_pingpong_16,when_dash_home_stop_animation_17,
when_dash_home_timer_event_18
};

ALTIA_BOOLEAN altiaInWhen = false;
/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaExecWhen(ALTIA_INDEX index, ALTIA_INDEX nameIndex,
                             ALTIA_INT cloneNum)
{
    ALTIA_BOOLEAN res;
    AltiaEventType value;

    if (index < 0)
    {
        return false;
    }

    if (altiaInWhen)
    {
        /* We are current executing a when so we have to wait */
        ALTIA_INT inc = _delayTail + 1;

        if (inc >= ALTIA_CNTLQ_SIZE)
        {
            inc = 0;
        }
        if (inc == _delayHead)
        {
            _altiaErrorMessage(
                ALT_TEXT("control queue overflow, try dynamic memory"));
            return false; /* we are full and can't add any more to cir queue */
        }


        /* add to queue, don't check for duplicates since when's will 
         * get called less often if they are already queued and Altia
         * does not check for duplicates.
         */
        _whenDelay[_delayTail].index = index;
        _whenDelay[_delayTail].nameIndex = nameIndex;
        _delayTail = inc;
        /* only do event checking if we are NOT initializing the design */
        if (!altiaInitializingDesign)
        {
            _altiaCheckSystem(); 
        }
        return false;
    }
    else
    {
        altiaInWhen = true;
        value = test_curValue[nameIndex];
        res = (* test_whenFuncs[index])(value);
        altiaInWhen = false;
        return res;
    }
}

/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN _altiaExecDelayedWhen()
{
    ALTIA_BOOLEAN res = false;
    AltiaEventType value;

    while (_delayHead != _delayTail)
    {
        /* call any delayed whens */
        Altia_WhenDelay_type *ptr = &_whenDelay[_delayHead++];

        /* If at end of queue, reset to beginning */


        if (_delayHead >= ALTIA_CNTLQ_SIZE)
        {
            _delayHead = 0;
        }


        /* get the current value and use that */
        value = test_curValue[ptr->nameIndex];
        altiaInWhen = true;
        res |= (* test_whenFuncs[ptr->index]) (value);

        /* only do event checking if we are NOT initializing the design */
        if (!altiaInitializingDesign)
        {
            _altiaCheckSystem(); 
        }
        altiaInWhen = false;
    }

    return res;
}


