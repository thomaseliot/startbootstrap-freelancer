/* $Revision: 1.19 $    $Date: 2009-10-01 23:11:06 $
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

/** FILE:  altiaLibStencil.c ************************************************
 **
 ** This file contains the routines to create monochrome bitmaps and draw
 ** them.  Monochrome bitmaps are required for rendering Altia stencil
 ** objects and scaled/stretched/rotated text (if bitmap scaling was
 ** enabled at code generation time).  If a design has no such objects,
 ** this file contains no code.
 **
 ** Monochrome and color bitmap drawing are usually the most difficult
 ** operations to port from one target to another.  The comment header
 ** for each routine in this file identifies the amount of target specific
 ** code in the routine to help determine the degree of modification that
 ** may be required to support a different target.
 ***************************************************************************/


