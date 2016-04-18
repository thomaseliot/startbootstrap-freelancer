/* $Revision: 1.22 $    $Date: 2010-04-19 04:20:50 $
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

/** FILE:  altiaLibLabel.c **************************************************
 **
 ** This file contains the routines to initialize fonts and draw text.
 ** If a design does not use any fonts, this file contains no code.
 **
 ** Font management and text drawing are second in difficulty only to
 ** raster and stencil drawing.  Depending on the target, scaled/stretched/
 ** rotated text drawing may be the most difficult operations of all.  The
 ** command header for each routine in this file identifies the degree of
 ** modification that may be required to support a different target.
 ***************************************************************************/



#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


#define BITSTOBYTES(bits)    (((bits) + 7) / 8)

extern AltiaDevInfo *altiaLibGetDevInfo(void);

extern EGL_COLOR altiaLibGetColor(
#ifdef Altiafp
    ALTIA_WINDOW devinfo,
    ALTIA_COLOR  pixel
#endif
);

extern ALTIA_UINT32 _altiaFindFont(
#ifdef Altiafp
ALTIA_CHAR *name
#endif
);

extern void  altiaLibSetTextDC(
#ifdef Altiafp
    ALTIA_WINDOW            win,
    ALTIA_UINT32            dc,
    Altia_GraphicState_type *gs,
    ALTIA_COORD             width,
    ALTIA_UINT32            *holdFont,
    ALTIA_UINT32            font
#endif
);

extern void altiaLibClearDC(
#ifdef Altiafp
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_UINT32 hbrush,
    ALTIA_UINT32 hpen
#endif
);






/*
 * The following structure type is used if the target does not support
 * egl_FontCreateByName() which is the case if EGL_HAS_FONTCREATEBYNAME is 0.
 */
typedef struct _xFontName {
    ALTIA_CHAR        Registry[10];     /* Not used by Altia */
    ALTIA_CHAR        Foundry[20];      /* Not normally used */
    ALTIA_CHAR        FamilyName[50];   /* Typically <= 32 chars */
    ALTIA_CHAR        WeightName[20];   /* Example: medium, bold, condensed */
    ALTIA_CHAR        Slant[3];         /* Example: r, i, o, ri, ro */
    ALTIA_CHAR        SetwidthName[20]; /* Example: normal */
    ALTIA_CHAR        AddStyleName[10]; /* Not used by Altia */
    unsigned int      PixelSize;
    unsigned int      PointSize;
    unsigned int      ResolutionX;
    unsigned int      ResolutionY;
    ALTIA_CHAR        Spacing[2];          /* Example: m, p, c */
    unsigned int      AverageWidth;
    ALTIA_CHAR        CharSetRegistry[20]; /* Example: chinesebig5 */
    ALTIA_CHAR        CharSetEncoding[10]; /* Example in Linux/UX: 1 */
} X11FontName;

#define FontNameRegistry        (1<<0)
#define FontNameFoundry         (1<<1)
#define FontNameFamilyName      (1<<2)
#define FontNameWeightName      (1<<3)
#define FontNameSlant           (1<<4)
#define FontNameSetwidthName    (1<<5)
#define FontNameAddStyleName    (1<<6)
#define FontNamePixelSize       (1<<7)
#define FontNamePointSize       (1<<8)
#define FontNameResolutionX     (1<<9)
#define FontNameResolutionY     (1<<10)
#define FontNameSpacing         (1<<11)
#define FontNameAverageWidth    (1<<12)
#define FontNameCharSetRegistry (1<<13)
#define FontNameCharSetEncoding (1<<14)

#define SlantRoman              ("R")
#define SlantItalic             ("I")
#define SlantOblique            ("O")
#define SlantReverseItalic      ("RI")
#define SlantReverseOblique     ("RO")

#define SpacingMonoSpaced       ("M")
#define SpacingProportional     ("P")
#define SpacingCharacterCell    ("C")

#ifndef EGL_USE_FULL_STRING_DRAW
    #define EGL_USE_FULL_STRING_DRAW 0
#elif EGL_USE_FULL_STRING_DRAW
    #if EGL_USE_SINGLE_CHAR_DRAW
        #error EGL_USE_SINGLE_CHAR_DRAW is not compatible with EGL_USE_FULL_STRING_DRAW
    #endif
#endif

/*--- localExtractStringField() -------------------------------------------
 *
 * Local utility to extract a string from the next field of an X11
 * like font specification (which is how the code generator stores
 * the descriptions of the fonts used by objects).  This function
 * should NOT require changes between targets.
 *
 * This function is only required if the target does not support
 * egl_FontCreateByName() which is the case if EGL_HAS_FONTCREATEBYNAME is 0.
 *-------------------------------------------------------------------------*/
#if !EGL_HAS_FONTCREATEBYNAME
static ALTIA_CHAR *localExtractStringField(ALTIA_CHAR *name,
                                           ALTIA_CHAR *buffer, int size,
                                           unsigned int *attrp,
                                           unsigned int bit)
{
	ALTIA_CHAR *buf = buffer;

	if (!*name)
		return 0;
	while (*name && *name != ('-') && size > 0) {
		*buf++ = *name++;
		--size;
	}
	if (size <= 0)
		return 0;
	*buf = ('\0');
	if (buffer[0] != ('*') || buffer[1] != ('\0'))
		*attrp |= bit;
	if (*name == ('-'))
		return name+1;
	return name;
}
#endif /* NOT EGL_HAS_FONTCREATEBYNAME */


/*--- localExtractUnsignedField() -----------------------------------------
 *
 * Local utility to extract a number from the next field of an X11
 * like font specification (which is how the code generator stores
 * the descriptions of the fonts used by objects).  This function should
 * NOT require changes between targets.
 *
 * This function is only required if the target does not support
 * egl_FontCreateByName() which is the case if EGL_HAS_FONTCREATEBYNAME is 0.
 *-------------------------------------------------------------------------*/
#if !EGL_HAS_FONTCREATEBYNAME
static ALTIA_CHAR *localExtractUnsignedField(ALTIA_CHAR *name,
                                             unsigned int *result,
                                             unsigned int *attrp,
                                             unsigned int bit)
{
	ALTIA_CHAR	buf[256];
	ALTIA_CHAR	*c;
	unsigned int	i;
	int       	cval;

	name = localExtractStringField (name, buf, \
                              sizeof (buf) / sizeof (ALTIA_CHAR), attrp, bit);
	if (!name)
		return 0;
	if (!(*attrp & bit))
		return name;
	i = 0;
	for (c = buf; *c; c++) {
		cval = (int) (*c) & 0x0ff;
		if (cval < ('0') || cval > ('9'))
			return 0;
		i = i * 10 + (cval - ('0'));
	}
	*result = i;
	return name;
}
#endif /* EGL_HAS_FONTCREATEBYNAME */


/*--- localX11ParseFontName() ---------------------------------------------
 *
 * Local utility that parses the fields of an X11 like font specification
 * (which is how the code generator stores the descriptions of the fonts
 * used by objects) and puts the fields into individual elements of the
 * X11FontName structure pointed to by fontName.  The integer pointed to
 * by fontNameAttributes is actually a set of bits where each bit
 * identifies if an attribute was found (1) or not found (0) in the
 * specification string.  The caller can use this to decide how to fill
 * out its font description when making a call into the target graphics
 * library to actually load a font.  This function should NOT require
 * changes between targets.
 *
 * This function is only required if the target does not support
 * egl_FontCreateByName() which is the case if EGL_HAS_FONTCREATEBYNAME is 0.
 *-------------------------------------------------------------------------*/
#if !EGL_HAS_FONTCREATEBYNAME
static ALTIA_BOOLEAN localX11ParseFontName(ALTIA_CHAR *fontNameString,
                                           X11FontName *fontName,
                                           unsigned int *fontNameAttributes)
{
	ALTIA_CHAR	*name = fontNameString;
	X11FontName	temp;
	unsigned int	attributes = 0;
	int i = 0;
	int len;
	int j;

#define GetString(field,bit)\
	if (!(name = localExtractStringField \
		(name, temp.field, sizeof (temp.field) / sizeof (ALTIA_CHAR),\
		&attributes, bit))) \
		return false;

#define GetUnsigned(field,bit)\
	if (!(name = localExtractUnsignedField \
		(name, &temp.field, \
		&attributes, bit))) \
		return false;

	if (name == NULL)
	    return false;

	len = (int)ALT_STRLEN(name);
	for (j = 0; j < len; j++)
	{
	    if (name[j] == ('-'))
	        i++;
	}

	/* Accept various sets of fields in font specification */
	if (i == 6)
	{
	    GetString (Foundry, FontNameFoundry)
	    GetString (FamilyName, FontNameFamilyName)
	    GetString (WeightName, FontNameWeightName)
	    GetString (Slant, FontNameSlant)
	    GetString (SetwidthName, FontNameSetwidthName)
	    GetUnsigned (PointSize, FontNamePointSize)
	}else if ((i == 7) || (i == 8) || (i == 10) || (i == 12) || (i == 13))
	{/* looking for - seperated and maybe a single -- seperator */
	    GetString (Foundry, FontNameFoundry)
	    GetString (FamilyName, FontNameFamilyName)
	    GetString (WeightName, FontNameWeightName)
	    GetString (Slant, FontNameSlant)
	    GetString (SetwidthName, FontNameSetwidthName)
	    if ((i == 8) || (i == 10) || (i == 12) || (i == 13))
	    {
		GetString (AddStyleName, FontNameAddStyleName)
	    }
	    GetUnsigned (PixelSize, FontNamePixelSize)
	    GetUnsigned (PointSize, FontNamePointSize)
	    if ((i == 10) || (i == 12) || (i == 13))
	    {
		GetUnsigned (ResolutionX, FontNameResolutionX)
		GetUnsigned (ResolutionY, FontNameResolutionY)
	    }
	    if (i == 12 || i == 13)
	    {
		GetString (Spacing, FontNameSpacing)
		GetUnsigned (AverageWidth, FontNameAverageWidth)
	    }
	    if (i == 13)
		GetString (CharSetRegistry, FontNameCharSetRegistry)
	}else
	{
	    GetString (Registry, FontNameRegistry)
	    GetString (Foundry, FontNameFoundry)
	    GetString (FamilyName, FontNameFamilyName)
	    GetString (WeightName, FontNameWeightName)
	    GetString (Slant, FontNameSlant)
	    GetString (SetwidthName, FontNameSetwidthName)
	    GetString (AddStyleName, FontNameAddStyleName)
	    GetUnsigned (PixelSize, FontNamePixelSize)
	    GetUnsigned (PointSize, FontNamePointSize)
	    GetUnsigned (ResolutionX, FontNameResolutionX)
	    GetUnsigned (ResolutionY, FontNameResolutionY)
	    GetString (Spacing, FontNameSpacing)
	    GetUnsigned (AverageWidth, FontNameAverageWidth)
	    GetString (CharSetRegistry, FontNameCharSetRegistry)
	    if (!*name) {
		    temp.CharSetEncoding[0] = ('\0');
		    attributes |= FontNameCharSetEncoding;
	    } else {
		    GetString (CharSetEncoding, FontNameCharSetEncoding)
	    }
	}
	*fontName = temp;
	*fontNameAttributes = attributes;
	return true;
}
#endif /* EGL_HAS_FONTCREATEBYNAME */


/*--- localGetLogFontCharSet() --------------------------------------------
 *
 * Local utility that is very target specific and can be somewhat
 * complicated if the target graphics library supports many character
 * sets or it is necessary to set the character set from the type
 * face name.  This function can do that work by comparing the character
 * set name stored in the design specific data file during code
 * generation against character set names supported by the target.
 * If the font name stored by Altia had no character set, it can also
 * look at the type face name to come up with a supported character set.
 *
 * This function is only required if the target does not support
 * egl_FontCreateByName() which is the case if EGL_HAS_FONTCREATEBYNAME is 0.
 *-------------------------------------------------------------------------*/
#if !EGL_HAS_FONTCREATEBYNAME
static EGL_ORD localGetLogFontCharSet(ALTIA_CHAR *cset)
{
    /* On this target, only character sets are ISO_8859_1 and UNICODE */
    EGL_ORD char_set = EGL_FONT_ISO_8859_1;

    if (cset != NULL)
    {
	if (ALT_STRCMP(cset, ALT_TEXT("unicode")) == 0
	    || ALT_STRCMP(cset, ALT_TEXT("Unicode")) == 0
	    || ALT_STRCMP(cset, ALT_TEXT("UNICODE")) == 0)
	    char_set = EGL_FONT_UNICODE;
    }

    return char_set;

}
#endif /* EGL_HAS_FONTCREATEBYNAME */


#if 0
/*--- localStrNCpy --------------------------------------------------------
 *
 * Local utility to handle string copy.  This is easy if we are not
 * on a Unicode system - just use strnpcy().  Otherwise, the source a
 * wide string that we need to put into a regular byte string.
 *-------------------------------------------------------------------------*/
static void localStrNCpy(char *result, ALTIA_CHAR *source, int total)
{
#ifndef UNICODE
    strncpy(result, source, total);
#else
    int count = 0;

    while (*source != ('\0') && count < total)
    {
        *result++ = (*source++) & 0x0ff;
        count++;
    }
    while (count < total)
    {
        *result++ = 0;
        count++;
    }
#endif
}
#endif


/*--- altiaLibLoadFont() --------------------------------------------------
 *
 * This function is called from other Altia code to load a font
 * from a font name stored in the design specific data file during code
 * generation.  The font name is stored as an X11 like font specification.
 * This is very target specific code because different target graphics
 * libraries usually handle font initialization very differently.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
ALTIA_UINT32 altiaLibLoadFont(ALTIA_CHAR *name)
{
  ALTIA_UINT32 id;

  id = _altiaFindFont(name);

#if EGL_HAS_FONTCREATEBYNAME
    /* This target has a function to create a font directly from the
     * string name provided by the Altia code generator.
     */
    if (id == 0)
    {
	AltiaDevInfo *devInfo = altiaLibGetDevInfo();
	EGL_FONT_ID fontId;

	if (devInfo->fontDrvId == (EGL_FONT_DRIVER_ID) 0)
	    return 0;

	/* See if the graphics library can create the font directly from
	 * the given string name all on its own (for example, an X11 target
	 * can do this if the string name is an X11 style font name).
	 */
	if ((fontId = egl_FontCreateByName(devInfo->fontDrvId, name)) != NULL)
	    id = (ALTIA_UINT32) fontId;
    }

#else /* NOT EGL_HAS_FONTCREATEBYNAME */
    /* This target requires parsing the font string name into elements of
     * a data structure and calling egl_FontCreate() with the data structure.
     */
    if (id == 0)
    {
	EGL_FONT_DEF fontDef;
	EGL_FONT_DESC desc;
	EGL_FONT_DESC_PRIORITY priority;
	X11FontName fontName;
	unsigned int fontattr;
	AltiaDevInfo *devInfo = altiaLibGetDevInfo();

	if (devInfo->fontDrvId == (EGL_FONT_DRIVER_ID) 0)
	    return 0;

	if (!localX11ParseFontName(name, &fontName, &fontattr))
	{
	    _altiaErrorMessage(ALT_TEXT("Could not parse font name"));
	    return 0;
	}


	/************************************************************
	 * Face name may be optionally put into foundry field.
	 * If it is present, it has highest priority.
	 */
	if (fontattr & FontNameFoundry)
	{
	    ALT_STRNCPY(desc.faceName, fontName.Foundry,
	                 EGL_FONT_FACE_NAME_MAX_LENGTH);
	    /*
	    localStrNCpy(desc.faceName, fontName.Foundry,
	                 EGL_FONT_FACE_NAME_MAX_LENGTH);
             */

	    desc.faceName[EGL_FONT_FACE_NAME_MAX_LENGTH - 1] = '\0';
	    priority.faceName = 7;
	}
	else
	    priority.faceName = EGL_FONT_DONT_CARE;


	/************************************************************
	 * Font pixel size is highest priority (after a face name if
	 * one is given).
	 */
	if ((fontattr & FontNamePixelSize) && (fontName.PixelSize != 0))
	{
	    desc.pixelSize.eglMin = (EGL_ORD)fontName.PixelSize;
	    desc.pixelSize.eglMax = (EGL_ORD)fontName.PixelSize;
	    priority.pixelSize = 6;
	}
	else
	    priority.pixelSize = EGL_FONT_DONT_CARE;


	/************************************************************
	 * Font spacing has next highest priority if it is present.
	 */
	priority.spacing = EGL_FONT_DONT_CARE;
	if (fontattr & FontNameSpacing)
	{
	    if (ALT_STRCMP(fontName.Spacing, ALT_TEXT("m")) == 0
	        || ALT_STRCMP(fontName.Spacing, ALT_TEXT("c")))
	    {
		desc.spacing = EGL_FONT_MONO_SPACED;
		priority.spacing = 5;
	    }
	    else if (ALT_STRCMP(fontName.Spacing, ALT_TEXT("p")) == 0)
	    {
		desc.spacing = EGL_FONT_PROPORTIONAL;
		priority.spacing = 5;
	    }
	}


	/************************************************************
	 * Font weight has priority after spacing.
	 */
	priority.weight = 4;
	desc.weight.eglMin = EGL_FONT_BOLD_OFF;
	if (fontattr & FontNameWeightName)
	{
	    if (ALT_STRCMP(fontName.WeightName, ALT_TEXT("bold")) == 0)
		desc.weight.eglMin = EGL_FONT_BOLD;
	    else if (ALT_STRCMP(fontName.WeightName, ALT_TEXT("demibold")) == 0)
		desc.weight.eglMin = EGL_FONT_BOLD_LIGHT;
	}
	desc.weight.eglMax = desc.weight.eglMin;


	/************************************************************
	 * Font slant has priority after font weight.
	 */
	priority.italic = 3;
	if ((fontattr & FontNameSlant)
	    && (ALT_STRCMP(fontName.Slant, ALT_TEXT("i")) == 0
	        || ALT_STRCMP(fontName.Slant, ALT_TEXT("o")) == 0))
	    desc.italic = EGL_FONT_ITALIC;
	else
	    desc.italic = EGL_FONT_UPRIGHT;


	/************************************************************
	 * Font family name is next in priority
	 */
	if (fontattr & FontNameFamilyName)
	{
	    ALT_STRNCPY(desc.familyName, fontName.FamilyName,
	                 EGL_FONT_FAMILY_NAME_MAX_LENGTH);
	    /*
	    localStrNCpy(desc.familyName, fontName.FamilyName,
	                 EGL_FONT_FAMILY_NAME_MAX_LENGTH);
             */
	    desc.familyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH - 1] = '\0';
	    priority.familyName = 2;
	}
	else if (!(fontattr & FontNameFoundry))
	{
	    _altiaErrorMessage(ALT_TEXT("Font name must have a Font Family"));
	    return 0;
	}
	else
	    priority.familyName = EGL_FONT_DONT_CARE;


	/************************************************************
	 * Font character set is lowest priority
	 */
	if (fontattr & FontNameCharSetRegistry)
	{
	    desc.charSet = localGetLogFontCharSet(fontName.CharSetRegistry);
	    priority.charSet = 1;
	}
	else
	    priority.charSet = EGL_FONT_DONT_CARE;

	/* We are ready to get a font... */
	if (egl_FontFind(devInfo->fontDrvId, &desc, &priority, &fontDef)
	    == EGL_STATUS_OK)
	{
	    EGL_FONT_ID fontId = egl_FontCreate(devInfo->fontDrvId, &fontDef);
	    if (fontId != NULL)
		id = (ALTIA_UINT32) fontId;
	}
    }
#endif /* EGL_HAS_FONTCREATEBYNAME */

    if (id == 0)
	_altiaErrorMessage(ALT_TEXT("Could not find font"));

    return id;
}

void altiaUnloadFont(ALTIA_UINT32 fontId)
{
    egl_FontDestroy((EGL_FONT_ID)fontId);
}






/*--- altiaLibLabelDraw() -------------------------------------------------
 *
 * This function is called from other Altia code to draw text.
 * This is very target specific code because different target graphics
 * libraries handle text drawing very differently.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibLabelDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       Altia_GraphicState_type *gs, Altia_Label_type *label,
                       Altia_Transform_type *trans)
{
    EGL_FONT_METRICS fontMetrics;
    Altia_Extent_type wex;
    ALTIA_UINT32 font = _altiaGetFont(gs->font);
    ALTIA_COORD x = 0, y = 0;
    int height;
    ALTIA_BOOLEAN isTransformed = false;
    ALTIA_BOOLEAN singleCharDraw = false;

    /* Give up if we don't have a font driver or a valid font */
    if (((AltiaDevInfo *) win)->fontDrvId == (EGL_FONT_DRIVER_ID) 0
        || font == 0)
    {
        return;
    }

    altiaLibGetFullWindowExtent(win, &wex);

    height = label->extent.y1 - label->extent.y0;




    /* If the text is not to be drawn one character at a time,
     * additional information about the text is needed.
     */
#if !EGL_USE_FULL_STRING_DRAW
    if (!singleCharDraw)
#endif
    {
        /* Put x and y through the transform, even if the text is
         * not transformed, so that translations still applied.
         */
        altiaLibTransform(trans, 0, 0, &x, &y);

        /* The y value is currently the lower left corner where the
         * text should be placed (where the bottom of the screen is
         * y = 0).  We must get the y value for the top of the text
         * which is done by adding height to y.  Note that height is
         * not necessarily the height of a single line of text because
         * the text might be a line from a MLTO.  The height is the
         * top of the text line that we are suppose to be drawing.
         */
        y += height;

        /* No matter what the target documentation says, the y for
         * text drawing is the baseline of the text so we need to
         * subtract the ascent from the current y value.  If a target
         * draws text using the bottom of the text as the reference
         * point (as is the case for altiaGL and Altia accelerated
         * targets), it should just return 0 for fontMetrics.maxDescent.
         */
        if (egl_FontMetricsGet((EGL_FONT_ID) font, &fontMetrics) == EGL_STATUS_OK)
        {
            y += (ALTIA_COORD)(fontMetrics.maxDescent - fontMetrics.height);
        }

        /* On this target, the top of the screen is the origin
         * (whereas Altia's origin is the bottom) so convert to
         * target's top-left coordinate system.
         */
        y = wex.y1 - y;
    }



        altiaLibSetTextDC(win, dc, gs, wex.y1, NULL, font);

        {
            egl_TextDraw(win, (EGL_GC_ID) dc, x, y, -1, label->name);
        }

        altiaLibClearDC(win, dc, 0, 0);



}


