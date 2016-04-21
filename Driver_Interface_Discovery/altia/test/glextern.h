/* $Revision: 1.3 $    $Date: 2009-01-28 17:43:24 $
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

#if !defined( __glextern_h__)
#define __glextern_h__

#ifdef RUNGLDATA
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern ArrAnimationSet glaset[];
extern ArrAnimationTrack glatrack[];
extern ArrQuaternion glquat[]; 
extern ArrVector glvect[]; 
extern ArrMatrix glmatrix[]; 
extern F32 glatime[];
extern ArrVertexWeight glvert[]; 
extern ArrVertexWeightArr glvertArr[];
extern ArrWeight glweight[];
extern ArrJoints gljoint[];
extern ArrVert3d glvert3d[];
extern u16 glindices[];
extern ArrMeshBuffer glmeshbuff[];
extern ArrMatSequence glmatseq[];
extern u16 glseq[];
extern u16 gltypes[];

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif

#endif /* __glextern_h__ */
