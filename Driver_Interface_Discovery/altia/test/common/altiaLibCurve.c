/* $Revision: 1.7 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibCurve.c *************************************************
 **
 ** This file contains utility routines to create line lists and
 ** determine containment for open-ended curved lines (splines and ticks),
 ** closed objects with curved outlines (closed splines and ellipses),
 ** and filled objects with curved outlines (filled splines and ellipses).
 ** Containment needs to only be determined if one of these objects has
 ** whole object stimulus defined for it or its group.  The code in this
 ** file is fairly generic, but it must use target specific arrays for
 ** storing points to be drawn.  This makes it target specific code.
 ** If there are no splines, closed splines, filled splines, ellipses, pie
 ** charts, or ticks in a design, this file contains no code.
 ***************************************************************************/


