/* $Revision: 1.8 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibEllipse.c ************************************************
 **
 ** This file contains the routines to draw, compute extent, and compute
 ** containment for ellipses (this includes circles which are a simple form
 ** of an ellipse).  For the typical target, ellipses are drawn using
 ** line and polygon drawing routines found in other source files for the
 ** target.  In such cases, this file requires few if any modifications
 ** from one target to the next.  If a design has no ellipses, ticks, or pie
 ** chart objects (tick objects draw ellipses), this file contains no code.
 ***************************************************************************/


