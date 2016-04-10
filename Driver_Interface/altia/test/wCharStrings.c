/* $Revision: 1.7 $    $Date: 2009-07-31 00:31:22 $
 * Copyright (c) 2004-2009 Altia Inc.
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

/* This file contains definitions for wide string manipulation
 * functions for the situation where the target does not support
 * them.  The file wCharString.h contains declarations that a C
 * source file can include.  The functions defined in this file
 * are good enough for Altia's usage.  They may not be good enough
 * for sophisticated usage by client application code or for the
 * sophisticated usage of the $scan or $format statements in Control.
 *
 * If a new target does not have support for the wide character
 * functions, compile this file with: -DUNICODE -DALTIA_WCHAR_SUPPORT
 */
#if defined(UNICODE) && defined(ALTIA_WCHAR_SUPPORT)

#include <stdio.h>
#include <stdlib.h>

#if !defined(WIN32) || !defined(MFCAPP)
#include <ctype.h>
#endif

#if defined(WIN32) || defined (QNX) || defined (VXWORKS) || !defined (OLD_VARARGS)
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"

void *_wfopen(ALTIA_CHAR *fileName, ALTIA_CHAR *mode)
{
    return NULL;
}

ALTIA_CHAR fgetwc(void *file)
{
    return (ALTIA_CHAR) EOF;
}

int wcscmp(ALTIA_CHAR *a, ALTIA_CHAR *b)
{

/*
{
int i = 0;
printf("comparing ");
while(a[i] != 0)
{
char c = a[i] & 0x0ff;
if (a[i] > 0x0ff)
printf("?");
else
printf("%c", c);
i++;
}
printf(" to ");
i = 0;
while(b[i] != 0)
{
char c = b[i] & 0x0ff;
if (b[i] > 0x0ff)
printf("?");
else
printf("%c", c);
i++;
}
}
*/
    if (a == b)
        return 0;

    while (*a == *b)
    {
        if (*a == 0)
        {
/* printf(" - match!\n"); */
            return 0;
        }
        a++; b++;
    }

    if (*a < *b)
    {
/* printf(" - a < b\n"); */
        return -1;
    }

/* printf(" - a > b\n"); */
    return 1;
}


ALTIA_CHAR *wcscpy(ALTIA_CHAR *out, ALTIA_CHAR *in)
{
    ALTIA_CHAR *ptr = out;
    while (*in != 0)
        *ptr++ = *in++;

    *ptr = 0;
    return out;
}

ALTIA_CHAR *wcscat(ALTIA_CHAR *out, ALTIA_CHAR *in)
{
    ALTIA_CHAR *ptr = out;

    while (*ptr != 0) ptr++;

    while (*in != 0)
        *ptr++ = *in++;

    *ptr = 0;
    return out;
}


ALTIA_CHAR *wcsncpy(ALTIA_CHAR *out, ALTIA_CHAR *in, int total)
{
    ALTIA_CHAR *ptr = out;
    int count = 0;

    while (*in != 0 && count < total)
    {
        *ptr++ = *in++;
        count++;
    }

    while (count < total)
    {
        *ptr++ = 0;
        count++;
    }

    return out;
}

int wcsncmp(ALTIA_CHAR *a, ALTIA_CHAR *b, int total)
{
    int count = 0;

    if (a == b)
        return 0;

    while (*a == *b && count < total)
    {
        if (*a == 0)
            return 0;
        a++; b++; count++;
    }

    if (count == total)
        return 0;

    if (*a < *b)
    {
        return -1;
    }

    return 1;
}

int wcslen(ALTIA_CHAR *a)
{
    int count = 0;
    while (*a++ != 0)
        count++;

    return count;
}

ALTIA_CHAR *wcschr(ALTIA_CHAR *in, ALTIA_CHAR c)
{
    while (*in != c && *in != 0)
        in++;

    if (*in == 0)
        return 0;

    return in;
}

ALTIA_CHAR *wcsrchr(ALTIA_CHAR *in, ALTIA_CHAR c)
{
    int count = wcslen(in);
    ALTIA_CHAR *ptr;

    ptr = in + (count - 1);

    while (count > 0 && *ptr != c)
    {
        ptr--;
        count--;
    }

    if (count > 0)
        return ptr;

    return 0;
}

static char localChars[50];
static char localChars2[50];

double wcstod(ALTIA_CHAR *in, ALTIA_CHAR **out)
{
    char *outCharPtr;
    double result = 0;
    char *inCharPtr = localChars;
    ALTIA_CHAR *widePtr = in;
    int count = 0;

    /* We are doing a number so legitimate characters are
     * all in the regular ASCII range.  Put the wide chars
     * into an array of regular chars and just run the array
     * through the standard strtod().
     */
    while (*widePtr != 0 && count < (sizeof(localChars) - 1))
    {
        if (*widePtr > 0xff)
            *inCharPtr++ = 'Z';
        else
            *inCharPtr++ = (char) (*widePtr & 0x0ff);

        widePtr++;
        count++;
    }

    *inCharPtr = '\0';
    result = strtod(localChars, &outCharPtr);

    if (out != 0)
    {
        count = outCharPtr - localChars;
        *out = &(in[count]);
    }

    return result;
}

long wcstol(ALTIA_CHAR *in, ALTIA_CHAR **out, int base)
{
    char *outCharPtr;
    long result = 0;
    char *inCharPtr = localChars;
    ALTIA_CHAR *widePtr = in;
    int count = 0;

    /* We are doing a number so legitimate characters are
     * all in the regular ASCII range.  Put the wide chars
     * into an array of regular chars and just run the array
     * through the standard strtol().
     */
    while (*widePtr != 0 && count < (sizeof(localChars) - 1))
    {
        if (*widePtr > 0xff)
            *inCharPtr++ = 'Z';
        else
            *inCharPtr++ = (char) (*widePtr & 0x0ff);

        widePtr++;
        count++;
    }

    *inCharPtr = '\0';
    result = strtol(localChars, &outCharPtr, base);

    if (out != 0)
    {
        count = outCharPtr - localChars;
        *out = &(in[count]);
    }

    return result;
}

unsigned long wcstoul(ALTIA_CHAR *in, ALTIA_CHAR **out, int base)
{
    char *outCharPtr;
    unsigned long result = 0;
    char *inCharPtr = localChars;
    ALTIA_CHAR *widePtr = in;
    int count = 0;

    /* We are doing a number so legitimate characters are
     * all in the regular ASCII range.  Put the wide chars
     * into an array of regular chars and just run the array
     * through the standard strtoul().
     */
    while (*widePtr != 0 && count < (sizeof(localChars) - 1))
    {
        if (*widePtr > 0xff)
            *inCharPtr++ = 'Z';
        else
            *inCharPtr++ = (char) (*widePtr & 0x0ff);

        widePtr++;
        count++;
    }

    *inCharPtr = '\0';
    result = strtoul(localChars, &outCharPtr, base);

    if (out != 0)
    {
        count = outCharPtr - localChars;
        *out = &(in[count]);
    }

    return result;
}

ALTIA_CHAR * swprintf(ALTIA_CHAR *buffer, ALTIA_CHAR *format, ... )
{
    int         varInt;
    double      varDouble;
    va_list     marker;
    ALTIA_CHAR *varWStr;
    ALTIA_CHAR *formatPtr = format;
    ALTIA_CHAR *bufferPtr = buffer;
    char       *charPtr = localChars;

#if defined(WIN32) || defined (QNX) || defined (VXWORKS) || !defined (OLD_VARARGS)
    va_start(marker, format);   
#else
    va_start(marker);
#endif

    while (*formatPtr != 0)
    {
        if (*formatPtr != '%' || (*formatPtr == '%' && formatPtr[1] == '%'))
        {
            *bufferPtr++ = *formatPtr;
            if (*formatPtr == '%')
                formatPtr++;
            formatPtr++;
        }
        else
        {
            charPtr = localChars;
            while (*formatPtr != 0 && *formatPtr != 'c'
                   && *formatPtr != 'd' && *formatPtr != 'i'
                   && *formatPtr != 'o' && *formatPtr != 'u'
                   && *formatPtr != 'x' && *formatPtr != 'X'
                   && *formatPtr != 'e' && *formatPtr != 'E'
                   && *formatPtr != 'g' && *formatPtr != 'G'
                   && *formatPtr != 'f'
                   && *formatPtr != 's')
            {
                *charPtr++ = (char) (*formatPtr++ & 0x0ff);
            }
            *charPtr++ = *formatPtr;
            *charPtr = '\0';

            switch((char) (*formatPtr))
            {
                case 'c':
                    /* Get the character argument as an int because a char
                     * or short is automatically promoted to an int when
                     * it is passed as an argument in C.  The va_arg()
                     * function needs to work on the actual type, not
                     * the original type.
                     */
                    varInt = va_arg(marker, int);
                    varInt &= ALT_CHARMASK;
                    if (localChars[0] == '%' && localChars[1] == 'c'
                        && localChars[2] == '\0')
                    {
                        /* A simple single char conversion */
                        *bufferPtr++ = (ALTIA_CHAR) varInt;
                    }
                    else
                    {
                        sprintf(localChars2, localChars, varInt);
                        charPtr = localChars2;
                        while (*charPtr != '\0')
                            *bufferPtr++ = (ALTIA_CHAR) (*charPtr++) & 0x0ff;
                    }
                    break;

                case 'd':
                case 'i':
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    varInt = va_arg(marker, int);
                    sprintf(localChars2, localChars, varInt);
                    charPtr = localChars2;
                    while (*charPtr != '\0')
                        *bufferPtr++ = (ALTIA_CHAR) (*charPtr++) & 0x0ff;
                    break;

                case 'e':
                case 'E':
                case 'g':
                case 'G':
                case 'f':
                    varDouble = va_arg(marker, double);
                    sprintf(localChars2, localChars, varDouble);
                    charPtr = localChars2;
                    while (*charPtr != '\0')
                        *bufferPtr++ = (ALTIA_CHAR) (*charPtr++) & 0x0ff;
                    break;

                case 's':
                    varWStr = va_arg(marker, ALTIA_CHAR *);
                    while (*varWStr != 0)
                        *bufferPtr++ = *varWStr++;
                    break;

                case '\0':
                default:
                    break;
            }

            if (*formatPtr != 0)
                formatPtr++;
        }
    }

    *bufferPtr = 0;
    return buffer;
}

int iswspace(int c)
{
    if (c == ' ' || c == '\t')
        return 1;
    else
        return 0;
}

int iswpunct(int c)
{
    if (c > 255)
        return 0;
    else
        return (ispunct(c));
}

int iswprint(int c)
{
    if (c >= ' ' && c != 127)
        return 1;
    else
        return 0;
}

int iswalpha(int c)
{
    if (c > 255)
        return 1;
    else
        return (isalpha(c));
}

int swscanf(ALTIA_CHAR *buffer, ALTIA_CHAR *format, ... )
{
    ALTIA_CHAR  *varCharPtr;
    int         *varIntPtr;
    double      *varDoublePtr;
    va_list     marker;
    ALTIA_CHAR *varWStr;
    ALTIA_CHAR *varWStrStart;
    ALTIA_CHAR *formatPtr = format;
    ALTIA_CHAR *bufferPtr = buffer;
    char       *charPtr = localChars;
    char       *char2Ptr = localChars2;
    int         matchCount = 0;
    int         lastFormatWasChar = 0;

#if defined(WIN32) || defined (QNX) || defined (VXWORKS) || !defined (OLD_VARARGS)
    va_start(marker, format);   
#else
    va_start(marker);
#endif


    while (*formatPtr != 0 && *bufferPtr != 0)
    {
        if (lastFormatWasChar == 0)
        {
            /* Move through white space */
            while(*bufferPtr == ' ' || *bufferPtr == '\t')
                bufferPtr++;
            while(*formatPtr == ' ' || *formatPtr == '\t')
                formatPtr++;
        }

        if (*formatPtr == 0 || *bufferPtr == 0)
            break;

        lastFormatWasChar = 0;

        if (*formatPtr != '%' || (*formatPtr == '%' && formatPtr[1] == '%'))
        {
            if (*bufferPtr != *formatPtr)
                break;

            if (*formatPtr == '%')
                formatPtr++;
            formatPtr++;
            bufferPtr++;
        }
        else
        {
            charPtr = localChars;
            while (*formatPtr != 0 && *formatPtr != 'c'
                   && *formatPtr != 'd' && *formatPtr != 'i'
                   && *formatPtr != 'o' && *formatPtr != 'u'
                   && *formatPtr != 'x' && *formatPtr != 'X'
                   && *formatPtr != 'e' && *formatPtr != 'E'
                   && *formatPtr != 'g' && *formatPtr != 'G'
                   && *formatPtr != 'f'
                   && *formatPtr != 's')
            {
                *charPtr++ = (char) (*formatPtr++ & 0x0ff);
            }
            *charPtr++ = *formatPtr;
            *charPtr = '\0';

            switch((char) (*formatPtr++))
            {
                case 'c':
                    varCharPtr = va_arg(marker, ALTIA_CHAR *);
                    *varCharPtr = *bufferPtr++;
                    lastFormatWasChar = 1;
                    matchCount++;
                    break;

                case 'd':
                case 'i':
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    varIntPtr = va_arg(marker, int *);

                    char2Ptr = localChars2;
                    while (*bufferPtr != 0 && *bufferPtr != ' '
                           && *bufferPtr != '\t')
                        *char2Ptr++ = (char) (*bufferPtr++ & 0x0ff);

                    *char2Ptr = '\0';

                    if (sscanf(localChars2, localChars, varIntPtr) == 1)
                        matchCount++;

                    break;

                case 'e':
                case 'E':
                case 'g':
                case 'G':
                case 'f':
                    varDoublePtr = va_arg(marker, double *);

                    char2Ptr = localChars2;
                    while (*bufferPtr != 0 && *bufferPtr != ' '
                           && *bufferPtr != '\t')
                        *char2Ptr++ = (char) (*bufferPtr++ & 0x0ff);

                    *char2Ptr = '\0';

                    if (sscanf(localChars2, localChars, varDoublePtr) == 1)
                        matchCount++;

                    break;

                case 's':
                    varWStrStart = varWStr = va_arg(marker, ALTIA_CHAR *);

                    while (*bufferPtr != 0 && *bufferPtr != ' '
                           && *bufferPtr != '\t')
                        *varWStr++ = *bufferPtr++;

                    *varWStr = 0;
                    if (varWStr != varWStrStart)
                        matchCount++;
                    break;

                case '\0':
                default:
                    break;
            }
        }
    }

    return matchCount;
}

ALTIA_CHAR * wcsstr(ALTIA_CHAR * wcs1, ALTIA_CHAR * wcs2)
{
    ALTIA_CHAR *cp = (ALTIA_CHAR *)wcs1;
    ALTIA_CHAR *s1, *s2;

    while(*cp)
    {
        s1 = cp;
        s2 = (ALTIA_CHAR *)wcs2;

        while(*s1 && *s2 && !(*s1-*s2))
            s1++, s2++;

        if (!*s2)
            return cp;

        cp++;
    }

    return NULL;
}

#endif
