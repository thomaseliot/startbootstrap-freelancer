/* $Revision: 1.21 $    $Date: 2009-09-30 21:07:24 $
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
#include <stdlib.h>

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

/* 6/21/01:  In DeepScreen 1.0 release, we referenced errno but this
 * causes problems when linking with a multi-threaded application
 * because errno is a function instead of a variable.  So, we don't
 * reference errno anymore.
 * #ifndef UNDER_CE
 * #include <errno.h>
 * #endif
 */

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaData.h"



#if defined(UNICODE) && defined(WCHAR_IS_WCHAR_T) && !defined(ALTIA_CHAR_UNICODE)

/* Default size definition for the static buffer.  Can be overriden at
** compile time.  The ALTIA_CUSTOMSVALUE_SIZE definition is calculated
** in altiaTypes.h based upon the contents of the .dsn used for code
** generation.
**
** We put at additional minimum size into this check because the ALTIA_SPRINTF
** function is used in multiple places requiring the following minimum sizes:
**
** builtins.c:        128 (error string for nonaffine xform error formatting)
** builtins.c:         24 (clone animation name building)
** cloneUtl.c:         24 (clone animation name building)
** control.c:          96 (number to string conversion)
** mltextAnimate.c:     2 (single character string construction)
** openglAniamte.c:   521 (error string for failed file load)
** skinAnimate.c:    1024 (animation string construction)
** textioAnimate.c:    64 (string construction)
** tickmarkAnimate.c: 128 (tick mark labels)
** altiaDraw.c:        64 (error string for invalid layers)
** altiaTypes.h:       ALTIA_CUSTOMSVALUE_SIZE
*/
#ifndef ALTIA_SWPRINTF_BUFFER_SIZE
    /* Determine minimum size (code based upon contents of .dsn).
    ** !!! ORDER IS IMPORTANT -- MUST BE FROM HIGH TO LOW !!!
    */


    
    
    /* Default minimum size for character buffer */
    #define ALTIA_SWPRINTF_MIN_SIZE 128
    

    /* Buffer size based upon static memory code generation options
    ** unless it is less than the minimum required based upon
    ** calculations above.
    **
    ** NOTE:  using an extra 150 characters which will allow approximately
    **        50 special character tokens (i.e. %c, %d, etc).
    */
    #if (ALTIA_CUSTOMSVALUE_SIZE < ALTIA_SWPRINTF_MIN_SIZE)
        #define ALTIA_SWPRINTF_BUFFER_SIZE (ALTIA_SWPRINTF_MIN_SIZE + 150)
    #else
        #define ALTIA_SWPRINTF_BUFFER_SIZE (ALTIA_CUSTOMSVALUE_SIZE + 150)
    #endif
#endif /* !defined(ALTIA_SWPRINTF_BUFFER_SIZE) */

/* static memory buffer for building string output */
static ALTIA_CHAR _altia_swprintf_buffer[ALTIA_SWPRINTF_BUFFER_SIZE];

/*----------------------------------------------------------------------
 * For C99 compliant C compiler, UNICODE and WCHAR_IS_CHAR_T will be
 * defined and ALTIA_CHAR_UNICODE (for activex integration only) will
 * not be defined.  A C99 compliant swprintf(buf, size, format, ...)
 * has extra size arg and requires an 'l' (ell) length modifier on a
 * string 's' or character 'c' conversion to force handling as a wide
 * string or wide character.  These differences require this custom
 * wrapper function to convert legacy swprintf() arguments to C99
 * compliant swprintf() arguments.
 *----------------------------------------------------------------------*/
#include <stdarg.h>
#ifdef Altiafp
int _altiaSwprintf(ALTIA_CHAR *buf, ALTIA_CHAR *format_in, ...)
#else
int altiaSwprintf(buf, format_in, ...)
ALTIA_CHAR *buf;
ALTIA_CHAR *format_in; 
#endif
{
    static ALTIA_CHAR *work_buf = NULL;
    static int work_len = 0;
    ALTIA_CHAR *format_out;
    int ret_val = -1;
    int fmt_len;
    va_list ap;

    va_start(ap, format_in);

    /* Default is to just use incoming format as the outgoing format */
    format_out = format_in;

    /* All of the work that is going to happen next is to inject an
     * 'l' (ell) modifier on string 's' or character 'c' conversion
     * specifiers.  Argh!  A lot of work, but it must be done since
     * DeepScreen generated code assumes the swprint() referred to
     * by the ALT_SPRINTF and API_SPRINTF macros doesn't need an 'l'
     * operator to recognize string args for 's' as wide strings and
     * value args for 'c' as wide characters.
     */
    if (format_in != NULL && (fmt_len = (int)ALT_STRLEN(format_in)) > 0)
    {
        int fmt_i, work_i, mod_cnt;

        /* Figure out how many conversion specifiers are in the format */
        for (fmt_i = 0, mod_cnt = 0; fmt_i < fmt_len; fmt_i++)
            if (format_in[fmt_i] == '%') mod_cnt++;

        /* If there are conversion specifiers, lets deal with them */
        if (mod_cnt)
        {
            if ((work_len < (fmt_len + mod_cnt + 1)))
            {

                /* Ensure calculated size fits into static memory buffer */
                work_len = fmt_len + mod_cnt + 1;
                if (work_len <= ALTIA_SWPRINTF_BUFFER_SIZE)
                {
                    work_buf = _altia_swprintf_buffer;
                }
                else
                {
                    /* Static buffer is too small */
                    work_buf = NULL;
                }


            }

            if (work_buf == NULL)
                work_len = 0;
            else
            {
                fmt_i = work_i = 0;
                while (fmt_i < fmt_len)
                {
                    if (format_in[fmt_i] == '%' && format_in[fmt_i+1] == '%')
                    {
                        /* Just an escape of '%' to really print '%' */
                        work_buf[work_i++] = format_in[fmt_i++];
                        work_buf[work_i++] = format_in[fmt_i++];
                    }
                    else if (format_in[fmt_i] != '%')
                    {
                        /* Just a regular character */
                        work_buf[work_i++] = format_in[fmt_i++];
                        continue;
                    }
                    else
                    {
                        /* % is beginning of a conversion specification */
                        int spec_done = 0;
                        int found_ell = 0;
                        work_buf[work_i++] = format_in[fmt_i++];
                        while (!spec_done && fmt_i < fmt_len)
                        {
                            switch (format_in[fmt_i])
                            {
                                case '#':
                                case '0':
                                case '-':
                                case '+':
                                case ',':
                                case 'I':
                                case '.':
                                case '*':
                                case '$':
                                case 'h':
                                case 'L':
                                case 'j':
                                case 'z':
                                case 't':
                                    /* Just pass flags, precision, and
                                     * most length modifiers unchanged.
                                     */
                                    work_buf[work_i++] = format_in[fmt_i++];
                                    break;

                                case 'l':
                                case 'w':
                                    /* Pass long length modifiers unchanged,
                                     * but make note of its detection so
                                     * we don't duplicate it.
                                     */
                                    found_ell = 1;
                                    work_buf[work_i++] = format_in[fmt_i++];
                                    break;

                                case 's':
                                case 'c':
                                    /* Pass string and character conversion
                                     * specifiers along with a long length
                                     * modifier if not already detected.
                                     */
                                    if (!found_ell)
                                        work_buf[work_i++] = 'l';
                                    work_buf[work_i++] = format_in[fmt_i++];
                                    /* And this specification is done! */
                                    spec_done = 1;
                                    break;

                                default:
                                    /* Have a number 0 to 9 or a conversion
                                     * specifier other than s or c.  Pass 
                                     * them unchanged.
                                     */

                                    /* If not 0 to 9, must be a conversion
                                     * specifier so this specification is
                                     * done!
                                     */
                                    if (format_in[fmt_i] < '0'
                                        || format_in[fmt_i] > '9')
                                        spec_done = 1;

                                    work_buf[work_i++] = format_in[fmt_i++];
                                    break;
                            }
                        }
                    }
                }

                /* Yahoo!  work_buf should contain C99 compliant format */
                work_buf[work_i] = '\0';
                format_out = work_buf;
            }
        }
    }

    /* Tell vswprintf() there's space for 32767 characters.  The
     * actual space is not known because it was not provided by
     * the caller (since the caller was expecting to make a call
     * to the legacy version of swprintf() that does not take a
     * size argument).  The 0x7fff value is large enough to
     * not go negative on a 16-bit target.
     */

    /*
     * {
     *   char wide2char[512];
     *   wcstombs(wide2char, format_in, 512);
     *   printf("Format in:  \"%s\"\n", wide2char);
     *   wcstombs(wide2char, format_out, 512);
     *   printf("Format out: \"%s\"\n", wide2char);
     * }
     */

    ret_val = vswprintf(buf, 0x7fff, format_out, ap);
    va_end(ap);

    return ret_val;
}
#endif /* UNICODE && WCHAR_IS_WCHAR_T && !ALTIA_CHAR_UNICODE */


/*----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN altiaLibIsNumber(ALTIA_CHAR *string,
                               AltiaEventType *dval)
#else
ALTIA_BOOLEAN altiaLibIsNumber(string, dval)
ALTIA_CHAR *string;
AltiaEventType *dval;
#endif
{
    ALTIA_CHAR *end;

#ifdef YES_STRTO_RANGE_CHECK
    /* On some targets, strto functions do not stop the parse of the string
     * when they encounter a non-numeric character with character code greater
     * than 127 (i.e., most significant bit is set so the character code will
     * look negative if the strto implementation is not careful).  So, let's
     * check for a character value out of range and immediately return false
     * if one is found.  This logic may not work for a Unicode string, but it
     * works for a byte string.
     */
    end = string;
    while (*end != '\0')
    {
	if (*end++ < 0)
	    return false;
    }
#endif /* YES_STRTO_RANGE_CHECK */



    /* lets see if its a signed integer */
    *dval = ALT_STRTOL(string, &end, 0);
    if (end == string) {
	return false;
    }

    while ((*end != '\0') && ALT_ISSPACE(*end)) {
        end++;
    }

    if (*end != '\0')
    {
	/* We need to try strtol incase its a hex number */
	*dval = ALT_STRTOUL(string, &end, 0);

	/* 6/21/01:  In DeepScreen 1.0 release, we referenced errno but this
	 * causes problems when linking with a multi-threaded application
	 * because errno is a function instead of a variable.  So, we don't
	 * reference errno anymore.
	 * #ifndef UNDER_CE
	 * if (errno == ERANGE)
	 *    return false;
	 * #endif
	 */

	if (end == string) 
	    return false;
	while ((*end != '\0') && ALT_ISSPACE(*end)) {
	    end++;
	}
	if (*end != '\0')
	    return false;
    }
    return true;
}

/*----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN altiaLibIsInteger(ALTIA_CHAR *string,
                                int *value)
#else
ALTIA_BOOLEAN altiaLibIsInteger(string, value)
ALTIA_CHAR *string;
int *value;
#endif
{
    AltiaEventType dval;
    if (altiaLibIsNumber(string, &dval))
    {
        *value = (int) dval;
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------*/
ALTIA_INDEX altiaFindObj(AltiaEventType id, ALTIA_SHORT *otype)
{
    ALTIA_INDEX i;

    for (i = 0; i < test_dobjs_count; i++)
    {
        if (ALTIA_DOBJ_ID(i) == (ALTIA_UINT32)id)
        {
            *otype = AltiaDynamicObject;
            return i;
        }
    }


    for (i = 0; i < test_sobjs_count; i++)
    {
        if (ALTIA_SOBJ_ID(i) == (ALTIA_UINT32)id)
        {
            *otype = AltiaStaticObject;
            return i;
        }
    }

    return -1;
}

/*----------------------------------------------------------------------------*/
Altia_Transform_type *altiaGetTotalTrans(ALTIA_INDEX objIdx,
                                         ALTIA_SHORT objType)
{

    if(objType == AltiaDynamicObject)
        return ALTIA_DOBJ_TOTAL_ADDR(objIdx);


    if(objType == AltiaStaticObject)
        return ALTIA_SOBJ_TOTAL_ADDR(objIdx);

    return 0;
}

/*----------------------------------------------------------------------------*/
Altia_Extent_type *altiaGetCurrentExtent(ALTIA_INDEX objIdx,
                                         ALTIA_SHORT objType)
{

    if(objType == AltiaDynamicObject)
    {

        /* Special processing for cached text-io objects */
        if (AltiaTextioDraw == ALTIA_DOBJ_DRAWTYPE(objIdx))
        {
            /* Check if the extent needs to be processed */
            ALTIA_INDEX textio = ALTIA_DOBJ_DRAWINDEX(objIdx);
            if (NULL != ALTIA_TEXT_CUSTS(textio))
            {
                altiaLibTextioProcess(textio, objIdx, ALTIA_FALSE);
            }
        }


        return ALTIA_DOBJ_EXTENT_ADDR(objIdx);
    }


    if(objType == AltiaStaticObject)
    {
        return ALTIA_SOBJ_EXTENT_ADDR(objIdx);
    }

    return (Altia_Extent_type *)0;
}

/*----------------------------------------------------------------------------*/
ALTIA_SHORT altiaGetDrawType(ALTIA_INDEX objIdx,
                             ALTIA_SHORT objType)
{

    if(objType == AltiaDynamicObject)
        return ALTIA_DOBJ_DRAWTYPE(objIdx);


    if(objType == AltiaStaticObject)
        return ALTIA_SOBJ_DRAWTYPE(objIdx);

    return 0;
}
