/* $Revision: 1.4 $    $Date: 2009-10-01 23:09:53 $
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

#if !defined( __gldata_h__)
#define __gldata_h__

#ifndef __IRR_TYPES_H_INCLUDED__
/* Header included by DeepScreen Code */
typedef char c8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
    #ifdef ALTIAFIXPOINT
    typedef int F32;
    #else
    typedef float F32;
    #endif
#else
/* Header included by 3D Object */
using namespace irr;
    #ifdef ALTIAFIXPOINT
    typedef int F32;
    #else
    typedef float F32;
    #endif
#endif 
typedef unsigned char glbool;

typedef struct 
{
    c8 *AnimationName;
    s32 trackIndex;
    s32 trackCnt;
}ArrAnimationSet;

typedef struct
{
    s32 sequenceIdx;
    s32 sequenceCnt;
    s32 typeIdx;
    s32 typeCnt;
    s32 quatsIdx;
    s32 quatsCnt;
    s32 matricesIdx;
    s32 matricesCnt;
}ArrMatSequence;

typedef struct 
{
    s32 jointNr;
    s32 keyType; /* 0=rotation, 1=scale, 2=position, 3=matrix */
    s32 quaternionIndex;
    s32 quaternionCnt;
    s32 vectorIndex;
    s32 vectorCnt;
    s32 matrixIndex;
    s32 matrixCnt;
    s32 matRotateIndex;
    s32 matRotateCnt;
    s32 timesIndex;
    s32 timesCnt;
}ArrAnimationTrack;

typedef struct
{
    F32 X, Y, Z, W;
}ArrQuaternion;

typedef struct
{
    F32 X, Y, Z;
}ArrVector;

typedef struct
{
    F32 X, Y;
}ArrVector2d;

typedef struct
{
    ArrVector   Pos;
    ArrVector   Normal;
    u32         Color;
    ArrVector2d TCoords;
}ArrVert3d;

typedef struct 
{
    s32 meshCnt;
    s32 meshIndex; 
    s32 indicesCnt;
    s32 indicesIndex;
    u16 MaterialType;
    u32 AmbientColor;
    u32 DiffuseColor;
    u32 EmissiveColor;
    u32 SpecularColor;
    F32 Shininess;
    F32 MaterialTypeParam;
    F32 MaterialTypeParam2;
    F32 Thickness;
    c8* texname1;
    c8* texname2;
    c8* texname3;
    c8* texname4;
    s32 texmatrix1;
    s32 texmatrix2;
    s32 texmatrix3;
    s32 texmatrix4;
    s32 texwrap1;
    s32 texwrap2;
    s32 texwrap3;
    s32 texwrap4;

}ArrMeshBuffer;

typedef struct
{
    F32 M[16];
    glbool definitelyIdentityMatrix;
}ArrMatrix;

typedef struct
{
    F32 weight[4];
    s32 joint[4];
    s32 weightCount;
}ArrVertexWeight;

typedef struct
{
    s32 vertexWeightIndex;
    s32 vertexWeightCnt;
}ArrVertexWeightArr;

typedef struct
{
    s32 buffer;
    s32 vertex;
    F32 weight;
}ArrWeight;

typedef struct
{
    s32 Parent; /* index of parent */
    s32 weightsIndex;
    s32 weightsCnt;
    ArrMatrix MatrixOffset;
    ArrMatrix LocalMatrix;
    ArrMatrix GlobalMatrix;
    ArrMatrix AnimatedMatrix;
    ArrMatrix LocalAnimatedMatrix;
    ArrMatrix CombinedAnimationMatrix;
    c8 *Name;
    glbool IsVirtualJoint;
    glbool WasAnimatedThisFrame;
}ArrJoints;

#endif /* __gldata_h__ */
