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

/** FILE:  altiaLibDash.c **************************************************
 **
 ** This file contains the routines to draw dashed lines that resemble
 ** the styles in Altia.  To do this for single width dash styles, the
 ** target graphics library must have a function to draw a single pixel.
 ** To draw multi-width dash styles, the graphics library's line and
 ** polygon drawing routines are used to draw individual segments of
 ** each line.  If the graphics library line drawing function can do
 ** a good job of drawing single width dash styles as well as other
 ** multi-width dash styles, the code in this file can be significantly
 ** simplified.  If no objects in a design use dashed line styles, this
 ** file contains no code.
 ***************************************************************************/


