/* $Revision: 1.46 $    $Date: 2010-09-08 20:57:09 $
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

#if !defined(ALTIABASE_H)
#define ALTIABASE_H

/* The following macro is used for alignment of some variables
 * in Altia DeepScreen generated code.  Used when devices
 * require alignment of image data or when accessing data
 * that typecast to a structure (but declared as a non struct).
 */
#ifndef ALTIA_ALIGN
	#if defined(__GNUC__) || defined(GREEN_HILLS)
		#define ALTIA_ALIGN	__attribute__((aligned))
	#else
		#define ALTIA_ALIGN
	#endif
#endif /* ALTIA_ALIGN */

#ifdef ALTIAGL
/* AltiaGL currently requires malloc reguardless if dynamic memory has
 * been specified or not since it needs to create Graphic device
 * contexts, bitmaps, etc.
 */
#include <stdlib.h>

#ifndef ALTIA_MALLOC
    extern void *xalloc(size_t size);
    #define ALTIA_MALLOC(s)     xalloc((size_t)(s))
#endif

#ifndef ALTIA_FREE
    extern void xfree(void *ptr);
    #define ALTIA_FREE(a)       xfree(a)
#endif

#ifndef ALTIA_REALLOC
    extern void *xrealloc(void *ptr, size_t size);
    #define ALTIA_REALLOC(a,s)  xrealloc((a),(size_t)(s))
#endif

/* For enabling defines like ALTIA_USE_INLINE_FX_MATH and
 * ALTIA_USE_DRIVER_FX_MATH, use compile line options.
 * Including target specific headers here is not advised.
 */

#else


#ifndef API_NOMALLOC
#include <stdlib.h>

#ifndef ALTIA_MALLOC
    #define ALTIA_MALLOC(s)     malloc((size_t)(s))
#endif

#ifndef ALTIA_FREE
    #define ALTIA_FREE(a)       free(a)
#endif

#ifndef ALTIA_REALLOC
    #define ALTIA_REALLOC(a,s)  realloc((a),(size_t)(s))
#endif
#endif /* API_NOMALLOC */

#endif /* ALTIAGL */

/* Optionally force dynamic memory allocation of DIB or not by
 * defining ALTIA_DIB_MALLOC as 1 or 0, respectively.  If ALTIA_DIB_MALLOC
 * is not already defined, it is defined based on the code generation option
 * to use dynamic memory.
 */

/* Dynamic memory disabled. Dynamic allocation of DIB data NEVER allowed */
#ifdef ALTIA_DIB_MALLOC
#undef ALTIA_DIB_MALLOC
#endif
#define ALTIA_DIB_MALLOC 0



#define ALTIA_CONST const
#define LONG_MINUS_ONE 0xffffffffU

/* Altia's definition of white is red = 0xff, blue = 0xff,
 * and green = 0xff packed into 24-bits (the low order 24-bits
 * if it is being assigned to a 32-bit or larger data type).
 * Altia's definition of black is red = 0x00, blue = 0x00,
 * green = 0x00 also packed into 24-bits.
 */
#ifndef ALTIA_WHITE
#define ALTIA_WHITE 0x0ffffffU
#endif
#ifndef ALTIA_BLACK
#define ALTIA_BLACK 0x0U
#endif
#define ALTIA_RED 0x000000ffU
#define ALTIA_GREEN 0x0000ff00U
#define ALTIA_BLUE 0x00ff0000U

/*
 * For use when a monochrome display is selected.
 */
#define ALTIA_PIXEL_ON 1
#define ALTIA_PIXEL_OFF 0
#define ALTIA_NO_COLOR_SPECIFIED 3



/* 
 * If your processor can do 64 bit math then set the define below.
 * Note that while fxmultiply is slower then in-line 64 bit math
 * it does do range checking on the result and displays a error
 * for the first out of range value is computes.
 */
/* #define FXMATH64 */

#define ALTIAFIXPOINT

#define VARFX_ONE 1
typedef long ALTIA_FIXPT;
typedef long ALTIA_FIXPT64[2];
typedef long ALTIA_VARFX;

extern ALTIA_FIXPT convF2FX(double a);
extern float convFX2F(ALTIA_FIXPT a);
extern ALTIA_FIXPT fxsin(ALTIA_FIXPT angle);
extern ALTIA_FIXPT fxcos(ALTIA_FIXPT angle);
extern ALTIA_FIXPT fxatan2(ALTIA_FIXPT y, ALTIA_FIXPT x);
extern ALTIA_FIXPT fxsqrt(ALTIA_FIXPT value);
extern ALTIA_FIXPT fxhypot(ALTIA_FIXPT fx, ALTIA_FIXPT fy);
extern int fxsignificant(ALTIA_FIXPT value);

/* a ALT_FSHIFT of 14 will allow numbers to +-131072 with an 
 * accuracy of .00006.
 */
#define ALT_FSHIFT 14
#define TWICE_SHIFT (ALT_FSHIFT * 2)



#define ALT_FFACTOR ((double)((long)1 << ALT_FSHIFT))
/* Integer to fixed */
#define ALT_I2FX(x) ((long)(x) << ALT_FSHIFT)		
#define ALT_I2F(x) ((long)(x) << ALT_FSHIFT)		
/* Constant to fixed.
 * Since constants are comuted at compile time we can keep the
 * floating point multiply instead of calling conv.
 */
#define ALT_C2FX(x) ((ALTIA_FIXPT)((double)(x) * ALT_FFACTOR))
/* Float to fixed */
/* #define ALT_F2FX(x) convF2FX(x) */
#define ALT_F2FX(x) (x)
/* Double to fixed */
/* #define ALT_D2FX(x) convF2FX(x) */
#define ALT_D2FX(x) (x)
/* Round a fixed to an int.
 * Because of 2's complement shifting a negative will be 1 smaller,
 * So, because of this adding a .5 no matter if pos or negative will
 * do the correct roundoff. 
 */
#define ALT_ROUND_FX2I(x) ((long)(((x)+((long)1 << (ALT_FSHIFT-1))) >> ALT_FSHIFT))
/* Truncate a fixed to an int without rounding
 * Because of 2's complement shifting a negative will be 1 smaller
 */
#define ALT_FX2I(x) ((long)((x)>=0 ? (x) >> ALT_FSHIFT : ((x) >> ALT_FSHIFT)+1))

/* Fixed to a float */
/* #define ALT_FX2F(x) convFX2F(x) */
#define ALT_FX2F(x) (x)

#if ALTIA_USE_DRIVER_FX_MATH
/* Use target specific fx math routines.
** The driver_fx_math.h header must define ALT_MULFX(x,y) 
** and ALT_DIVFX(x,y) macros.  The driver_fx_math.h header 
** can utilize the FX macros in altiaBase.h above.
*/
#include "driver_fx_math.h"

/* All fx math is coming from target specific fx math routines so
** disable defining fxmultiply() and fxdivide() in altiaLibFxmath.c
** if this header is in the process of getting included by
** altiaLibFxmath.c.
*/
#ifdef ALTIA_LIB_FX_MATH
#define ALTIA_INLINE_MULTIPLY
#endif



#elif defined(FXMATH64)     /* NOT ALTIA_USE_DRIVER_FX_MATH */

/* Target does not have target specific fx math routines,
** but it can do 64-bit math.
*/

#ifdef WIN32
#ifndef ALTLONGLONG
#define ALTLONGLONG __int64
#endif /* ALTLONGLONG */
#else
#ifndef ALTLONGLONG
#define ALTLONGLONG long long
#endif /* ALTLONGLONG */
#endif /* WIN32 */

#define ALT_MULFX(x, y) (ALTIA_FIXPT)(((ALTLONGLONG)((ALTLONGLONG)(x) * (ALTLONGLONG)(y))) >> ALT_FSHIFT)
#define ALT_DIVFX(x, y) (ALTIA_FIXPT)((ALTLONGLONG)((((ALTLONGLONG)(x)) << ALT_FSHIFT) / (ALTLONGLONG)(y)))



#else    /* NOT ALTIA_USE_DRIVER_FX_MATH AND NOT FXMATH64 */

/* Target does not have target specific fx math routines
** nor can it do 64-bit math.
*/

#define ALT_TWICE_FSHIFT        (ALT_FSHIFT * 2)
#define ALT_FSHIFT_MINUS_ONE    (ALT_FSHIFT - 1)
#define ALT_FX_MAX_VALUE        ((ALTIA_FIXPT)1 << (ALTIA_FIXPT)(31 - ALT_FSHIFT))
#define ALT_FX_ONE              ((ALTIA_FIXPT)1 << (ALTIA_FIXPT)ALT_FSHIFT)
#define ALT_FX_DIV_ONE          ((ALTIA_FIXPT)1 << (ALTIA_FIXPT)ALT_TWICE_FSHIFT)
#define ALT_FX_FRACTION_MASK    (ALT_FX_ONE - 1)

/* If using function inlining, defined inline type */
#ifndef ALT_FX_INLINE
    #if ALTIA_USE_INLINE_FX_MATH
        #define ALT_FX_INLINE  static inline
    #else
        #define ALT_FX_INLINE
    #endif
#endif /* ALT_FX_INLINE */

/* Use overflow detection if not using function inlining */
#if !ALTIA_USE_INLINE_FX_MATH
extern void fxoverflow(void);
#endif

/* FX math functions.  If using inline option (ALTIA_USE_INLINE_FX_MATH)
** these will appear in all source that includes altiaBase.h.
** Otherwise, these functions will only appear in altiaLibFxMath.c
** and be declared extern for all source.  If ALTIA_LIB_FX_MATH is
** defined, it means this header is in the process of getting
** included by altiaLibFxmath.c and we want to get these functions
** defined into altiaLibFxMath.c for sure even when not doing inline.
*/
#if defined(ALTIA_LIB_FX_MATH) || ALTIA_USE_INLINE_FX_MATH

#define ALTIA_INLINE_MULTIPLY

ALT_FX_INLINE ALTIA_FIXPT fxmultiply(ALTIA_FIXPT x, ALTIA_FIXPT y)
{
    register long ix, iy, fx, fy, ires, fres, temp;
    register ALTIA_FIXPT res;
    register int negx = 0;
    register int negy = 0;

    /* Perform 64-bit multiply in four 32-bit steps using
    ** the rule of distribution:
    **
    ** (ix + fx) * (iy + fy) = ixiy + ixfy + fxiy + fxfy
    **
    ** Where x and y are fixed point numbers, ix and iy are
    ** the integer parts of x and y, fx and fy are the
    ** fractional parts of x and y.
    */
    if (x < 0)
    {
        negx = 1;
        x = -x;
    }
    if (y < 0)
    {
        negy = 1;
        y = -y;
    }
    ix = x >> ALT_FSHIFT;
    iy = y >> ALT_FSHIFT;
    fx = ALT_FX_FRACTION_MASK & x;
    fy = ALT_FX_FRACTION_MASK & y;
    ires = ix * iy;
#if !ALTIA_USE_INLINE_FX_MATH
    if (ires > ALT_FX_MAX_VALUE)
        fxoverflow();
#endif
    fres = ((ix) * fy) + ((iy) * fx);
    temp = (fy) * (fx);
    if (temp < 0)
    {
        /* overflow so redo math with pre-shift */
        temp = ((fy >> 1) * (fx >> 1)) >> (ALT_FSHIFT-2);
    }
    else
        temp >>= ALT_FSHIFT;
    res = ((ires << ALT_FSHIFT) + fres + temp);
#if !ALTIA_USE_INLINE_FX_MATH
    if (res < 0)
        fxoverflow();
#endif
    if (negx != negy)
       res = -res;
    return res;
}

ALT_FX_INLINE ALTIA_FIXPT fxdivide(ALTIA_FIXPT x, ALTIA_FIXPT y)
{
    register ALTIA_FIXPT e;
    register ALTIA_FIXPT res;
    register int negx = 0;
    register int negy = 0;

    /* This is a 64 bit divide that relies on the 64 bit multiply.
    ** It uses the Goldschmidt algorithm to compute the reciprocal
    ** of y and then multiply.  This algorithm was derived by work
    ** from Pascal Massimino.
    */

    if ((0 == x) || (0 == y))
        return 0;

    if (x < 0)
    {
        negx = 1;
        x = -x;
    }
    if (y < 0)
    {
        negy = 1;
        y = -y;
    }

    /* Here we assume a word size of 32 so we can only shift up to 30 */
#if (ALT_TWICE_FSHIFT > 30)
    e = ((1L << 30) / y) << (ALT_TWICE_FSHIFT-30); 
#else
    e = ALT_FX_DIV_ONE / y;
#endif
    res = fxmultiply(x, e);
    e = ALT_FX_ONE - (fxmultiply(y, e));
    while (e != 0)
    {
        /* since e is so small we can use regular multiply in the loop */
        res += (res * e) >> ALT_FSHIFT;
        e = (e * e) >> ALT_FSHIFT;
        if (e == ALT_FX_ONE)
            break;
    }
    if (negx != negy)
       res = -res;
    return res;
}

#else /* NOT ALTIA_LIB_FX_MATH AND NOT ALTIA_USE_INLINE_FX_MATH */

extern ALTIA_FIXPT fxmultiply(ALTIA_FIXPT x, ALTIA_FIXPT y);
extern ALTIA_FIXPT fxdivide(ALTIA_FIXPT x, ALTIA_FIXPT y);

#endif /* ALTIA_LIB_FX_MATH || ALTIA_USE_INLINE_FX_MATH */

#define ALT_MULFX(x,y) fxmultiply(x,y)
#define ALT_DIVFX(x,y) fxdivide(x,y)



#endif /* ALTIA_USE_DRIVER_FX_MATH */




#define FMULT(x,y) (ALT_MULFX((x), (y)))
#define FDIV(x,y) (ALT_DIVFX((x), (y)))
#define FADD(x,y) ((x) + (y))
#define FSUB(x,y) ((x) - (y))
#define FSQRT(x) (fxsqrt(x))
#define FSIN(x) (fxsin(x))
#define FCOS(x) (fxcos(x))
#define FATAN2(x,y) (fxatan2((x), (y)))
#define FHYPOT(x,y) (fxhypot((x), (y)))
#define ALTIA_ROUND(x) ALT_ROUND_FX2I(x)

typedef ALTIA_FIXPT ALTIA_FLOAT;
typedef ALTIA_FIXPT ALTIA_DOUBLE;
#define ALTIA_DOUBLE ALTIA_DOUBLE
#define ALTIA_FLOAT ALTIA_FLOAT




#define ALTIA_CTL_ROUND(x) ((x) > 0 ? (int)((x)+0.5) : -(int)(-(x)+(0.499999999999999)))

typedef signed char ALTIA_BYTE;
typedef unsigned char ALTIA_UBYTE;
typedef signed short ALTIA_SHORT;
typedef unsigned short ALTIA_USHORT;

#ifdef MISRA_BOOLEAN
/* PC-Lint MISRA 2012 static analyzer requires _Bool as the boolean type */
typedef _Bool TYPE_ALTIA_BOOLEAN;
#else
/* Use signed char as the boolean type by default */
typedef signed char TYPE_ALTIA_BOOLEAN;
#endif
/* Use a define for ALTIA_BOOLEAN to suppress altiaEx.h from overriding it */
#define ALTIA_BOOLEAN TYPE_ALTIA_BOOLEAN

typedef signed short ALTIA_INDEX;
typedef int ALTIA_BINDEX;
typedef int ALTIA_FBINDEX;
typedef signed short ALTIA_COORD;
typedef unsigned long ALTIA_COLOR;

typedef const char * ALTIA_LPCSTR;
typedef char * ALTIA_LPSTR;

typedef unsigned char ALTIA_UINT8;
typedef unsigned short ALTIA_UINT16;
typedef unsigned long ALTIA_UINT32;

typedef signed char ALTIA_INT8;
typedef signed short ALTIA_INT16;
typedef signed long ALTIA_INT32;

#define ALTIA_TRUE  ((ALTIA_BOOLEAN)1)
#define ALTIA_FALSE ((ALTIA_BOOLEAN)0)

/* Until code generator auto-adjusts char for
 * different UNICODE targets, just set ALTIA_CHAR
 * based on the definition of preprocessor macros.
 */
#if !defined(UNICODE) && !defined(ALTIA_CHAR_UNICODE)
typedef char TYPE_ALTIA_CHAR;
#elif defined(ALTIA_CHAR_UNICODE) && defined(WCHAR_IS_WCHAR_T)
/* For activex integration only */
typedef wchar_t TYPE_ALTIA_CHAR;
#elif defined(WCHAR_IS_WCHAR_T)
/* For C99 compliant C compiler */
#include <wchar.h>
#include <wctype.h>
typedef wchar_t TYPE_ALTIA_CHAR;
#elif defined(WIN32) || defined(WCHAR_IS_USHORT)
typedef unsigned short TYPE_ALTIA_CHAR;
#elif defined(WCHAR_IS_UINT)
typedef unsigned int TYPE_ALTIA_CHAR;
#elif defined(WCHAR_IS_ULONG)
typedef unsigned long TYPE_ALTIA_CHAR;
#elif defined(WCHAR_IS_SHORT)
typedef short TYPE_ALTIA_CHAR;
#elif defined(WCHAR_IS_LONG)
typedef long TYPE_ALTIA_CHAR;
#else
typedef int TYPE_ALTIA_CHAR;
#endif
#define ALTIA_CHAR TYPE_ALTIA_CHAR

/* TRB removed ALTIA_UINT replace with ALTIA_UINT32 to fix 16 bit problem
 * typedef unsigned int ALTIA_UINT; */
typedef int ALTIA_INT;
#ifndef __altiaWindowDefined__
#define __altiaWindowDefined__
typedef void* ALTIA_WINDOW;
#endif
typedef void* ALTIA_HANDLE;



typedef int AltiaEventType;


#define VALUE_ERROR_TOL 1e-6
#define ALTIA_MAX_POPS 16
#define ALTIA_MAX_INPUTPOPS (ALTIA_MAX_POPS*8)
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef nil
#define nil 0
#endif
#define ALTIA_MIN(a, b) (a < b ? a : b)
#define ALTIA_MAX(a, b) (a > b ? a : b)



#define ALTIA_EQUAL(a, b) (ALT_F2FX(a) == ALT_F2FX(b))
#define ALTIA_LESS(a, b) (ALT_F2FX(a) < ALT_F2FX(b))
#define ALTIA_LESSEQUAL(a, b) (ALT_F2FX(a) <= ALT_F2FX(b))
#define ALTIA_GREATER(a, b) (ALT_F2FX(a) > ALT_F2FX(b))
#define ALTIA_GREATEREQUAL(a, b) (ALT_F2FX(a) >= ALT_F2FX(b))
#define ALTIA_ABS(a) (ALT_FX2F(abs(ALT_F2FX(a))))
#define ALTIA_F2I(a) (ALT_FX2I(a))
#define ALTIA_D2I(a) (ALT_FX2I(a))
#define ALTIA_C2FX(a) (ALT_C2FX(a))
#define ALTIA_I2D(a) (ALT_I2FX(a))
#define ALTIA_C2D(a) (ALT_C2FX(a))
#define ALTIA_C2F(a) (ALT_C2FX(a))
#define ALTIA_E2D(a) (ALT_I2FX(a))
#define ALTIA_E2F(a) (ALT_I2FX(a))
#define ALTIA_I2F(a) (ALT_I2F(a))		


/*********************************************************************
 * If UNICODE is defined and ALTIA_WCHAR_SUPPORT is defined, Altia
 * DeepScreen's implementation of wide char functions is enabled. This
 * requires compiling the DeepScreen additional wCharStrings.c file.
 * By default, macros below redefine DeepScreen wCharStrings.c funcs
 * to custom names beginning with _altia_ (e.g., _altia_wcscmp).  This
 * allows them to co-exist with C library wide char funcs.  This is
 * necessary if the target C library actually has wide char funcs
 * available and some application code uses them (but not application
 * code interfacing to DeepScreen).  If a #include of wCharStrings.h
 * and/or altia.h is in application code, occurences of wide char
 * function names (e.g., wcscmp) are redefined to be Altia's version
 * (e.g., _altia_wcscmp).  To force Altia wCharStrings.c func names to
 * exactly match standard C library wide char func names (for example,
 * because the target C library really has NO wide char funcs), define
 * ALTIA_WCHAR_NAMES=0 for DeepScreen and application code compilation.
 ********************************************************************/
#ifndef ALTIA_WCHAR_NAMES
#define ALTIA_WCHAR_NAMES 1
#endif
#if defined(UNICODE) && defined(ALTIA_WCHAR_SUPPORT) && (0 != ALTIA_WCHAR_NAMES) && !defined(ALTIAWCHARNAMES_H)
#define ALTIAWCHARNAMES_H
#define _wfopen   _altia_wfopen
#define fgetwc    _altia_fgetwc
#define wcscmp    _altia_wcscmp
#define wcscpy    _altia_wcscpy
#define wcscat    _altia_wcscat
#define wcsncpy   _altia_wcsncpy
#define wcsncmp   _altia_wcsncmp
#define wcslen    _altia_wcslen
#define wcschr    _altia_wcschr
#define wcsrchr   _altia_wcsrchr
#define wcstod    _altia_wcstod
#define wcstol    _altia_wcstol
#define wcstoul   _altia_wcstoul
#define swprintf  _altia_swprintf
#define iswspace  _altia_iswspace
#define iswpunct  _altia_iswpunct
#define iswprint  _altia_iswprint
#define iswalpha  _altia_iswalpha
#define swscanf   _altia_swscanf
#define wcsstr    _altia_wcsstr
#endif /* UNICODE && ALTIA_WCHAR_SUPPORT && ALTIA_WCHAR_NAMES */

/* Using ALT_ATOD for atof functionality */
#define ALT_ATOF(x) ALT_ATOD((x), NULL)

#ifdef UNICODE
#define ALT_STRCMP wcscmp
#define ALT_STRCPY wcscpy
#define ALT_STRCAT wcscat
#define ALT_STRNCPY wcsncpy
#define ALT_STRNCMP wcsncmp
#define ALT_STRLEN wcslen
#define ALT_STRTOD wcstod
#define ALT_STRTOL wcstol
#define ALT_STRTOUL wcstoul
#define ALT_STRSTR wcsstr

#if !defined(WCHAR_IS_WCHAR_T) || defined(ALTIA_CHAR_UNICODE)
/* WCHAR_IS_WCHART_T is not defined or ALTIA_CHAR_UNICODE (for activex
 * integration) is defined.  In this case, assume compiler is not C99
 * compliant and swprintf() arguments are identical to sprintf() arguments.
 */
#define ALT_SPRINTF swprintf
#else
/* WCHAR_IS_WCHART_T is defined and ALTIA_CHAR_UNICODE is not defined.
 * Assume compiler is compliant with C99 standard for swprintf() func.
 * C99 compliant swprintf(buf, size, format, ...) has extra size arg
 * thereby requiring a custom wrapper function to convert legacy
 * swprintf() argument list to C99 compliant argument list.
 */
#define ALT_SPRINTF _altiaSwprintf
extern int _altiaSwprintf(ALTIA_CHAR *buf, ALTIA_CHAR *format, ...);
#endif

#define ALT_ISSPACE iswspace
#define ALT_ISPUNCT iswpunct
#define ALT_ISPRINT iswprint
#define ALT_ISASCII iswascii
#define ALT_ISALPHA iswalpha
#define ALT_STRTOK  wcstok
#define ALT_STRCHR  wcschr
#define ALT_FGETC fgetwc
#define ALT_FPUTC fputwc
#define ALT_FOPEN _wfopen
#define ALT_CHARMASK 0xffffU
#define ALT_ATOD wcstod
#define ALT_SSCANF swscanf
#define ALT_TEXT(x) L ## x
#define ALT_STRRCHR  wcsrchr
#else
#define ALT_STRCMP strcmp
#define ALT_STRCPY strcpy
#define ALT_STRCAT strcat
#define ALT_STRNCPY strncpy
#define ALT_STRNCMP strncmp
#define ALT_STRLEN strlen
#define ALT_STRTOD strtod
#define ALT_STRTOL strtol
#define ALT_STRTOUL strtoul
#define ALT_STRSTR strstr

#define ALT_SPRINTF sprintf
#define ALT_ISSPACE isspace
#define ALT_ISPUNCT ispunct
#define ALT_ISPRINT isprint
#define ALT_ISASCII isascii
#define ALT_ISALPHA isalpha
#define ALT_STRTOK  strtok
#define ALT_STRCHR  strchr
#define ALT_FGETC fgetc
#define ALT_FPUTC fputc
#define ALT_FOPEN fopen
#define ALT_CHARMASK 0xffU
#define ALT_ATOD strtod
#define ALT_SSCANF sscanf
#define ALT_TEXT(x) x
#define ALT_STRRCHR  strrchr
#endif /* UNICODE */

#define ALT_MEMCPY memcpy



#endif /* ALTIABASE_H */
