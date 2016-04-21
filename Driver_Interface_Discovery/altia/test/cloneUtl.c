/* $Revision: 1.13 $    $Date: 2009-05-15 17:14:12 $
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
 *
 * DC 2-12-07  Rewrote how functions are cloned.  Added CloneList concept to
 *             resolve Cloned objects /children list to know which functions
 *             needed to be cloned. Now a 3 pass procedure.  Pass 1, use
 *             CloneList to build list of all objects to be cloned. Pass 2,
 *             Actualy clone all objects and functions.  Pass 3, Find and
 *             clone all funcIndex and it's related funcFIndexs that can
 *             not be found through objects and states clone process.
 *
 * DC 2-11-07  Added cloning of TimerStates and Connections that are valid
 *             to clone objects.  Make's use of the CloneList.
 */

