/* $Revision: 1.5 $    $Date: 2004-06-02 17:43:59 $
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

#define ARCRADIUS_ID   1
#define TICKRADIUS_ID   (1<<1)
#define STARTANGLE_ID   (1<<2)
#define SPAN_ID   (1<<3)
#define MAJORNUM_ID   (1<<4)
#define MINORNUM_ID   (1<<5)
#define SUBMINORNUM_ID   (1<<6)
#define MAJORLEN_ID   (1<<7)
#define MINORLEN_ID (1<<8)
#define SUBMINORLEN_ID   (1<<9)
#define MAJORWIDTH_ID  (1<<10)
#define MINORWIDTH_ID  (1<<11)
#define SUBMINORWIDTH_ID  (1<<12)
#define MAJORSHAPE_ID  (1<<13)
#define MINORSHAPE_ID (1<<14)
#define SUBMINORSHAPE_ID (1<<15)
#define TICKPOS_ID (1<<16)
#define ARCPOS_ID (1<<17)
#define LABELPOS_ID (1<<18)
#define LABELINC_ID (1<<19)
#define INITLABEL_ID (1<<20)
#define ARCWIDTH_ID (1<<21)

/* typedef enum ShapeTypes {LINE_SHAPE, FILLED_CIRCLE_SHAPE, MAX_SHAPE}; */
#define LINE_SHAPE 0 
#define FILLED_CIRCLE_SHAPE 1
#define MAX_SHAPE 2
/* typedef enum LabelPosTypes {NO_LABEL, LABEL_BOTTOM, LABEL_TOP, MAX_LABELPOS}; */
#define NO_LABEL 0
#define LABEL_BOTTOM 1
#define LABEL_TOP 2
#define MAX_LABELPOS 3
/* typedef enum ArcPosTypes {NO_ARC, ARC_CENTER, MAX_ARCPOS}; */
#define NO_ARC 0
#define ARC_CENTER 1
#define MAX_ARCPOS 2
/* typedef enum TickPosTypes {TOPS, CENTERED, BOTTOMS, MAX_TICKPOS}; */
#define TOPS 0
#define CENTERED 1
#define BOTTOMS 2
#define MAX_TICKPOS 3


