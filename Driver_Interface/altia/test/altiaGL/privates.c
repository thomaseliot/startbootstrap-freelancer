/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Xorg: privates.c,v 1.4 2001/02/09 02:04:40 xorgcvs Exp $ */
/*

Copyright 1993, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/programs/Xserver/dix/privates.c,v 3.8 2001/12/14 19:59:32 dawes Exp $ */

#include "scrnintstr.h"
#include "misc.h"
#include "gcstruct.h"
#include "servermd.h"
#include "Xmd.h"

/*
 *  See the Wrappers and devPrivates section in "Definition of the
 *  Porting Layer for the X v11 Sample Server" (doc/Server/ddx.tbl.ms)
 *  for information on how to use devPrivates.
 */

/*
 *  screen private machinery
 */

int  screenPrivateCount = 0;

void
ResetScreenPrivates(pScreen)
ScreenPtr pScreen;
{
    screenPrivateCount = 0;
    xfree(pScreen->devPrivates);
    pScreen->devPrivates = 0;
}

/* this can be called after some screens have been created,
 * so we have to worry about resizing existing devPrivates
 */
int
AllocateScreenPrivateIndex(pScreen)
ScreenPtr pScreen;
{
    int		idx;
    DevUnion	*nprivs;

    idx = screenPrivateCount++;
    nprivs = (DevUnion *)xrealloc(pScreen->devPrivates,
				      screenPrivateCount * sizeof(DevUnion));
    if (!nprivs)
    {
	screenPrivateCount--;
	return -1;
    }
    /* Zero the new private */
    bzero(&nprivs[idx], sizeof(DevUnion));
    pScreen->devPrivates = nprivs;
    return idx;
}


/*
 *  gc private machinery 
 */

static int  gcPrivateCount = 0;

void
ResetGCPrivates(pScreen)
ScreenPtr pScreen;
{
    gcPrivateCount = 0;
    xfree(pScreen->GCPrivateSizes);
    pScreen->GCPrivateSizes = 0;
}

int
AllocateGCPrivateIndex()
{
    return gcPrivateCount++;
}

Bool
AllocateGCPrivate(pScreen, index2, amount)
    register ScreenPtr pScreen;
    int index2;
    unsigned amount;
{
    unsigned oldamount;

    /* Round up sizes for proper alignment */
    amount = ((amount + (sizeof(long) - 1)) / sizeof(long)) * sizeof(long);

    if (index2 >= pScreen->GCPrivateLen)
    {
	unsigned *nsizes;
	nsizes = (unsigned *)xrealloc(pScreen->GCPrivateSizes,
				      (index2 + 1) * sizeof(unsigned));
	if (!nsizes)
	    return FALSE;
	while (pScreen->GCPrivateLen <= index2)
	{
	    nsizes[pScreen->GCPrivateLen++] = 0;
	    pScreen->totalGCSize += sizeof(DevUnion);
	}
	pScreen->GCPrivateSizes = nsizes;
    }
    oldamount = pScreen->GCPrivateSizes[index2];
    if (amount > oldamount)
    {
	pScreen->GCPrivateSizes[index2] = amount;
	pScreen->totalGCSize += (amount - oldamount);
    }
    return TRUE;
}


/*
 *  pixmap private machinery
 */
#ifdef PIXPRIV
static int  pixmapPrivateCount = 0;

void
ResetPixmapPrivates()
{
    pixmapPrivateCount = 0;
}

int
AllocatePixmapPrivateIndex()
{
    return pixmapPrivateCount++;
}

Bool
AllocatePixmapPrivate(pScreen, index2, amount)
    register ScreenPtr pScreen;
    int index2;
    unsigned amount;
{
    unsigned oldamount;

    /* Round up sizes for proper alignment */
    amount = ((amount + (sizeof(long) - 1)) / sizeof(long)) * sizeof(long);

    if (index2 >= pScreen->PixmapPrivateLen)
    {
	unsigned *nsizes;
	nsizes = (unsigned *)xrealloc(pScreen->PixmapPrivateSizes,
				      (index2 + 1) * sizeof(unsigned));
	if (!nsizes)
	    return FALSE;
	while (pScreen->PixmapPrivateLen <= index2)
	{
	    nsizes[pScreen->PixmapPrivateLen++] = 0;
	    pScreen->totalPixmapSize += sizeof(DevUnion);
	}
	pScreen->PixmapPrivateSizes = nsizes;
    }
    oldamount = pScreen->PixmapPrivateSizes[index2];
    if (amount > oldamount)
    {
	pScreen->PixmapPrivateSizes[index2] = amount;
	pScreen->totalPixmapSize += (amount - oldamount);
    }
    pScreen->totalPixmapSize = BitmapBytePad(pScreen->totalPixmapSize * 8);
    return TRUE;
}
#endif
