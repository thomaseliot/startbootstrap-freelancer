/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.3 $    $Date: 2009-05-06 00:03:00 $
 * Copyright (c) 2002 Altia Inc.
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

/** FILE:  egl_changegc.c ****************************************************
 **
 ** This file contains the change gc functions for the general purpose
 ** graphics library.  
 **
 ***************************************************************************/

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "egl_Wrapper.h"

#include <stdio.h>

#include "scrnintstr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "servermd.h"
#include "mi.h"
#ifdef USE_FB
#include "fb.h"
#endif

#ifdef UNDER_CE
#define NO_ASSERT
#endif


/* dixChangeGC(client, pGC, mask, pC32, pUnion)
 * 
 * This function was created as part of the Security extension
 * implementation.  The client performing the gc change must be passed so
 * that access checks can be performed on any tiles, stipples, or fonts
 * that are specified.  ddxen can call this too; they should normally
 * pass NullClient for the client since any access checking should have
 * already been done at a higher level.
 * 
 * Since we had to create a new function anyway, we decided to change the
 * way the list of gc values is passed to eliminate the compiler warnings
 * caused by the DoChangeGC interface.  You can pass the values via pC32
 * or pUnion, but not both; one of them must be NULL.  If you don't need
 * to pass any pointers, you can use either one:
 * 
 *     example calling dixChangeGC using pC32 parameter
 *
 *     CARD32 v[2];
 *     v[0] = foreground;
 *     v[1] = background;
 *     dixChangeGC(client, pGC, GCForeground|GCBackground, v, NULL);
 * 
 *     example calling dixChangeGC using pUnion parameter;
 *     same effect as above
 *
 *     ChangeGCVal v[2];
 *     v[0].val = foreground;
 *     v[1].val = background;
 *     dixChangeGC(client, pGC, GCForeground|GCBackground, NULL, v);
 * 
 * However, if you need to pass a pointer to a pixmap or font, you MUST
 * use the pUnion parameter.
 * 
 *     example calling dixChangeGC passing pointers in the value list
 *     v[1].ptr is a pointer to a pixmap
 *
 *     ChangeGCVal v[2];
 *     v[0].val = FillTiled;
 *     v[1].ptr = pPixmap;
 *     dixChangeGC(client, pGC, GCFillStyle|GCTile, NULL, v);
 * 
 * Note: we could have gotten by with just the pUnion parameter, but on
 * 64 bit machines that would have forced us to copy the value list that
 * comes in the ChangeGC request.
 * 
 * Ideally, we'd change all the DoChangeGC calls to dixChangeGC, but this
 * is far too many changes to consider at this time, so we've only
 * changed the ones that caused compiler warnings.  New code should use
 * dixChangeGC.
 * 
 */

unsigned char DefaultDash[2] = {4, 4};
#define NullClient ((ClientPtr)0)

#define NEXTVAL(_type, _var) { \
      if (pC32) _var = (_type)*pC32++; \
      else { \
	_var = (_type)(pUnion->val); pUnion++; \
      } \
    }

#ifdef NO_ASSERT 
#define NEXT_PTR(_type, _var) { \
    _var = (_type)pUnion->ptr; pUnion++; }
#else
#define NEXT_PTR(_type, _var) { \
    assert(pUnion); _var = (_type)pUnion->ptr; pUnion++; }
#endif

int
dixChangeGC(client, pGC, mask, pC32, pUnion)
    ClientPtr client;
    register GC 	*pGC;
    register BITS32	mask;
    CARD32		*pC32;
    ChangeGCValPtr	pUnion;
{
    register BITS32 	index2;
    register int 	error = 0;
    PixmapPtr 		pPixmap;
    BITS32		maskQ;

#ifndef NO_ASSERT 
    assert( (pC32 && !pUnion) || (!pC32 && pUnion) );
#endif
    pGC->serialNumber |= GC_CHANGE_SERIAL_BIT;

    maskQ = mask;	/* save these for when we walk the GCque */
    while (mask && !error) 
    {
	index2 = (BITS32) lowbit (mask);
	mask &= ~index2;
	pGC->stateChanges |= index2;
	switch (index2)
	{
	    case GCFunction:
	    {
		CARD8 newalu;
		NEXTVAL(CARD8, newalu);
		if (newalu <= GXset)
		    pGC->alu = newalu;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCPlaneMask:
		NEXTVAL(unsigned long, pGC->planemask);
		break;
	    case GCForeground:
		NEXTVAL(unsigned long, pGC->fgPixel);
		/*
		 * this is for CreateGC
		 */
		if (!pGC->tileIsPixel && !pGC->tile.pixmap)
		{
		    pGC->tileIsPixel = TRUE;
		    pGC->tile.pixel = pGC->fgPixel;
		}
		break;
	    case GCBackground:
		NEXTVAL(unsigned long, pGC->bgPixel);
		break;
	    case GCLineWidth:		/* ??? line width is a CARD16 */
		 NEXTVAL(CARD16, pGC->lineWidth);
		break;
	    case GCLineStyle:
	    {
		unsigned int newlinestyle;
		NEXTVAL(unsigned int, newlinestyle);
		if (newlinestyle <= LineDoubleDash)
		    pGC->lineStyle = newlinestyle;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCCapStyle:
	    {
		unsigned int newcapstyle;
		NEXTVAL(unsigned int, newcapstyle);
		if (newcapstyle <= CapProjecting)
		    pGC->capStyle = newcapstyle;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCJoinStyle:
	    {
		unsigned int newjoinstyle;
		NEXTVAL(unsigned int, newjoinstyle);
		if (newjoinstyle <= JoinBevel)
		    pGC->joinStyle = newjoinstyle;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCFillStyle:
	    {
		unsigned int newfillstyle;
		NEXTVAL(unsigned int, newfillstyle);
		if (newfillstyle <= FillOpaqueStippled)
		    pGC->fillStyle = newfillstyle;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCFillRule:
	    {
		unsigned int newfillrule;
		NEXTVAL(unsigned int, newfillrule);
		if (newfillrule <= WindingRule)
		    pGC->fillRule = newfillrule;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCTile:
	    {
		XID newpix = 0;
		if (pUnion)
		{
		    NEXT_PTR(PixmapPtr, pPixmap);
		}
		else
		{
		    NEXTVAL(XID, newpix);
		    pPixmap = (PixmapPtr)newpix;
		    /*
		    pPixmap = (PixmapPtr)SecurityLookupIDByType(client,
					newpix, RT_PIXMAP, SecurityReadAccess);
                     */
		}
		if (pPixmap)
		{
		    if ((pPixmap->drawable.depth != pGC->depth) ||
			(pPixmap->drawable.pScreen != pGC->pScreen))
		    {
			error = BadMatch;
		    }
		    else
		    {
			pPixmap->refcnt++;
			if (!pGC->tileIsPixel)
			    (* pGC->pScreen->DestroyPixmap)(pGC->tile.pixmap);
			pGC->tileIsPixel = FALSE;
			pGC->tile.pixmap = pPixmap;
		    }
		}
		else
		{
		    error = BadPixmap;
		}
		break;
	    }
	    case GCStipple:
	    {
		XID newstipple = 0;
		if (pUnion)
		{
		    NEXT_PTR(PixmapPtr, pPixmap);
		}
		else
		{
		    NEXTVAL(XID, newstipple)
		    pPixmap = (PixmapPtr)newstipple;
		    /*
		    pPixmap = (PixmapPtr)SecurityLookupIDByType(client,
				newstipple, RT_PIXMAP, SecurityReadAccess);
                     */
		}
		if (pPixmap)
		{
		    if ((pPixmap->drawable.depth != 1) ||
			(pPixmap->drawable.pScreen != pGC->pScreen))
		    {
			error = BadMatch;
		    }
		    else
		    {
			pPixmap->refcnt++;
			if (pGC->stipple)
			    (* pGC->pScreen->DestroyPixmap)(pGC->stipple);
			pGC->stipple = pPixmap;
		    }
		}
		else
		{
		    error = BadPixmap;
		}
		break;
	    }
	    case GCTileStipXOrigin:
		NEXTVAL(INT16, pGC->patOrg.x);
		break;
	    case GCTileStipYOrigin:
		NEXTVAL(INT16, pGC->patOrg.y);
		break;
	    case GCFont:
    	    {
		FontPtr	pFont;
		XID newfont = 0;
		if (pUnion)
		{
		    NEXT_PTR(FontPtr, pFont);
		}
		else
		{
		    NEXTVAL(XID, newfont)
		    pFont = (FontPtr)newfont;
		    /*
		    pFont = (FontPtr)SecurityLookupIDByType(client, newfont,
						RT_FONT, SecurityReadAccess);
		    */
		}
		/*
		if (pFont)
		{
		    pFont->refcnt++;
		    if (pGC->font)
    		        CloseFont(pGC->font, (Font)0);
		    pGC->font = pFont;
		 }
		else
		{
		    error = BadFont;
		}
		*/
		break;
	    }
	    case GCSubwindowMode:
	    {
		unsigned int newclipmode;
		NEXTVAL(unsigned int, newclipmode);
		if (newclipmode <= IncludeInferiors)
		    pGC->subWindowMode = newclipmode;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCGraphicsExposures:
    	    {
		unsigned int newge;
		NEXTVAL(unsigned int, newge);
		if (newge <= xTrue)
		    pGC->graphicsExposures = newge;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    case GCClipXOrigin:
		NEXTVAL(INT16, pGC->clipOrg.x);
		break;
	    case GCClipYOrigin:
		NEXTVAL(INT16, pGC->clipOrg.y);
		break;
	    case GCClipMask:
	    {
		Pixmap pid = 0;
		int    clipType = 0;

		if (pUnion)
		{
		    NEXT_PTR(PixmapPtr, pPixmap);
		}
		else
		{
		    NEXTVAL(Pixmap, pid)
		    if (pid == None)
		    {
			clipType = CT_NONE;
			pPixmap = NullPixmap;
		    }
		    else
			pPixmap = (PixmapPtr)pid;
			/*
		        pPixmap = (PixmapPtr)SecurityLookupIDByType(client,
					pid, RT_PIXMAP, SecurityReadAccess);
                         */
		}

		if (pPixmap)
		{
		    if ((pPixmap->drawable.depth != 1) ||
			(pPixmap->drawable.pScreen != pGC->pScreen))
		    {
			error = BadMatch;
		    }
		    else
		    {
			clipType = CT_PIXMAP;
			pPixmap->refcnt++;
		    }
		}
		else if (!pUnion && (pid != None))
		{
		    error = BadPixmap;
		}
		if(error == Success)
		{
		    (*pGC->funcs->ChangeClip)(pGC, clipType,
					      (pointer)pPixmap, 0);
		}
		break;
	    }
	    case GCDashOffset:
		NEXTVAL(INT16, pGC->dashOffset);
		break;
	    case GCDashList:
	    {
		CARD8 newdash;
		NEXTVAL(CARD8, newdash);
		if (newdash == 4)
		{
		    if (pGC->dash != DefaultDash)
		    {
			xfree(pGC->dash);
			pGC->numInDashList = 2;
			pGC->dash = DefaultDash;
		    }
		}
		else if (newdash != 0)
 		{
		    unsigned char *dash;

		    dash = (unsigned char *)xalloc(2 * sizeof(unsigned char));
		    if (dash)
		    {
			if (pGC->dash != DefaultDash)
			    xfree(pGC->dash);
			pGC->numInDashList = 2;
			pGC->dash = dash;
			dash[0] = newdash;
			dash[1] = newdash;
		    }
		    else
			error = BadAlloc;
		}
 		else
		{
		   error = BadValue;
		}
		break;
	    }
	    case GCArcMode:
	    {
		unsigned int newarcmode;
		NEXTVAL(unsigned int, newarcmode);
		if (newarcmode <= ArcPieSlice)
		    pGC->arcMode = newarcmode;
		else
		{
		    error = BadValue;
		}
		break;
	    }
	    default:
		error = BadValue;
		break;
	}
    } /* end while mask && !error */

    if (pGC->fillStyle == FillTiled && pGC->tileIsPixel)
    {
	/*
	if (!CreateDefaultTile (pGC))
	{
	    pGC->fillStyle = FillSolid;
	    error = BadAlloc;
	}
	*/
    }
    return error;
}

#undef NEXTVAL
#undef NEXT_PTR

/* Publically defined entry to ChangeGC.  Just calls dixChangeGC and tells
 * it that all of the entries are constants or IDs */
int
ChangeGC(pGC, mask, pval)
    register GC 	*pGC;
    register BITS32	mask;
    XID			*pval;
{
    return (dixChangeGC(NullClient, pGC, mask, (CARD32 *)pval, (ChangeGCValPtr)0));
}

/* DoChangeGC(pGC, mask, pval, fPointer)
   mask is a set of bits indicating which values to change.
   pval contains an appropriate value for each mask.
   fPointer is true if the values for tiles, stipples, fonts or clipmasks
   are pointers instead of IDs.  Note: if you are passing pointers you
   MUST declare the array of values as type pointer!  Other data types
   may not be large enough to hold pointers on some machines.  Yes,
   this means you have to cast to (XID *) when you pass the array to
   DoChangeGC.  Similarly, if you are not passing pointers (fPointer = 0) you
   MUST declare the array as type XID (not unsigned long!), or again the wrong
   size data type may be used.  To avoid this cruftiness, use dixChangeGC
   above.

   if there is an error, the value is marked as changed 
   anyway, which is probably wrong, but infrequent.

NOTE:
	all values sent over the protocol for ChangeGC requests are
32 bits long
*/
int
DoChangeGC(pGC, mask, pval, fPointer)
    register GC 	*pGC;
    register BITS32	mask;
    XID			*pval;
    int			fPointer;
{
    if (fPointer)
    /* XXX might be a problem on 64 bit big-endian servers */
	return dixChangeGC(NullClient, pGC, mask, (CARD32 *)0, (ChangeGCValPtr)pval);
    else
	return dixChangeGC(NullClient, pGC, mask, (CARD32 *)pval, (ChangeGCValPtr)0);
}

#ifdef DEBUG_WINDOW
void egl_OpenDebugWindow()
{
    driver_OpenDebugWindow();
}
#endif
