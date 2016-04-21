/* $Revision: 1.5 $    $Date: 2008-06-07 02:50:40 $
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
/*
 * This file contains the structs and functions for clones
 */
#ifndef __CLONES__
#define __CLONES__

#include "altiaControl.h"


#define CLONEPARENT 0xFFFFFFFF


#define ALTIA_CLONESTRING_SIZE  64



typedef struct
{
/**
 * The user defined unique number of this clone object.  When automatically
 * generating cloned child objects, the number is also auto-generated using
 * the same value as it's auto-generated ID.
 */
    ALTIA_INT num;
/**
 * The ID for this Clone object automatically assign by the last known
 * unused ID.
 */
    ALTIA_UINT32 id;
/**
 * The original cloned object's ID.
 */
    ALTIA_UINT32 origid;
/**
 * The type of this Cloned object.
 * @see  Altialib#AltiaNoObject
 */
    ALTIA_SHORT type;
/**
 * The index of this clone object within it's object type array.
 */
    ALTIA_INDEX cloneidx;
/**
 * The original index of this object that was cloned.
 */
    ALTIA_INDEX origIndex;
/**
 * If this object is a child object and it's parent is not cloned, then this is
 * the parent objects ID.  Otherwise, if no parent or parent is a cloned
 * object, then this is set to -1.  This is used to know if the objects parent
 * index is in the object array's clone data or the original data set.
 */
    ALTIA_UINT32 origParent;

/* DC - 03/26/07 - Changed for performance. */
/**
 * Link to next top level clone.  Since user specifies top level
 * clone numbers that can be accsending or descending or any order,
 * have to manualy search through the clones for matching numbers.
 * If not a top level clone (children), then numbers are same as
 * id so can get by id since it is faster.
 */
    void *PrevClone;
    void *NextClone;


/* DC - 03/21/07 - Changed for performance. */
/**
 * First funcIndex for this top level clone.
 */
    ALTIA_INDEX topFuncIdx;


/**
 * Control data for this clone.
 */
    test_Control_type *control;

} Altia_Clone_type;

typedef struct
{
    ALTIA_INDEX objIdx;     /* Original object index.        */
    ALTIA_SHORT type;       /* Object type.                  */
    ALTIA_INDEX cloneIdx;   /* Object clone Index.           */
    ALTIA_BOOLEAN cloned;   /* Object cloned flag.           */
} Obj_Def;

typedef struct
{
    int count;              /* Count of objects in array.    */
    Obj_Def *ObjList;       /* Object's Obj_Def array.       */
} Obj_List;

typedef struct
{
    ALTIA_INDEX objIdx;     /* Object index being deleted.   */
    ALTIA_SHORT type;       /* Object type.                  */
    ALTIA_BOOLEAN deleted;  /* Object flaged for delete.     */
} Obj_Del;

typedef struct
{
    int count;              /* Count of objects in array.    */
    Obj_Del *DelList;       /* Object's Obj_Del array.       */
} ObjDel_List;

typedef struct
{
    ALTIA_INDEX objIdx;     /* Original object array index.  */
    ALTIA_INDEX cloneIdx;   /* Cloned object array index.    */
    ALTIA_BOOLEAN cloned;   /* Object cloned flag.           */
} Sub_Def;

typedef struct
{
    int count;              /* Count of objects in array.    */
    Sub_Def *ObjList;       /* Object's Sub_Def array.       */
} Sub_List;

typedef struct
{
    ALTIA_INDEX objIdx;     /* Cloned Object index.          */
    ALTIA_BOOLEAN deleted;  /* Object is flaged for delete.  */
} Sub_Del;

typedef struct
{
    int count;              /* Count of objects in array.    */
    Sub_Del *DelList;       /* Object's Sub_Del array.       */
} SubDel_List;

typedef struct
{
    int topParent;          /* Clone index of top object.         */
    int funcnum;            /* Top object clone number.           */
    Obj_List objList;       /* Original objects to clone.         */

    Sub_List funcsList;     /* Original funcs to clone.           */
    Sub_List funcsSList;    /* Original funcs States to clone.    */


/* DC - 03/31/07 - Changed for performance. */
    ALTIA_INDEX topFuncIdx; /* First funcIndex for this group.    */
    Sub_List funcIList;     /* Original funcIndex to clone.       */


    Sub_List funcFList;     /* Original funcFIndex to clone.      */


    Sub_List timerList;     /* Original timers to clone.          */




} Clone_List;

typedef struct
{
    int topClone;              /* Clone Number of top object to delete.*/
    ObjDel_List objList;       /* Cloned objects to delete.            */

    SubDel_List funcsList;     /* Cloned funcs to delete.              */
    SubDel_List funcsSList;    /* Cloned funcs States to deleted.      */


    SubDel_List funcIList;     /* Cloned funcIndex to delete.          */


    SubDel_List funcFList;     /* Cloned funcFIndex to delete.         */


    SubDel_List timerList;     /* Cloned timers to delete.             */




} Delete_List;

/*
 * clones.c external functions
 */
extern ALTIA_INDEX _altiaFindCloneByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_BOOLEAN _altiaIsTopLevelCloneByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INDEX _altiaFindCloneObjByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INDEX _altiaFindCloneByID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_INDEX _altiaFindCloneObjByID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_INDEX _altiaFindCloneByOriginalID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_INDEX _altiaFindCloneObjByOriginalID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern Altia_Clone_type *_altiaGetCloneByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern Altia_Clone_type *_altiaGetCloneByID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern Altia_Clone_type *_altiaGetCloneByType(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_SHORT
#endif
);

extern ALTIA_BOOLEAN _altiaIsIdCloned(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_UINT32 _altiaGetCloneIDByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INT _altiaGetCloneNumberByID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_SHORT _altiaGetCloneType(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INDEX _altiaGetCloneObj(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INT _altiaGetCloneNumByType(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_SHORT
#endif
);

extern ALTIA_INT _altiaGetCloneNum(
#ifdef Altiafp
ALTIA_INDEX
#endif
);

extern ALTIA_UINT32 _altiaGetCloneId(
#ifdef Altiafp
ALTIA_INDEX
#endif
);

extern ALTIA_BOOLEAN _altiaCreatClone(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_SHORT,
ALTIA_INT
#endif
);

extern ALTIA_BOOLEAN _altiaDelCloneByID(
#ifdef Altiafp
ALTIA_UINT32
#endif
);

extern ALTIA_BOOLEAN _altiaDelCloneByNumber(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_BOOLEAN _altiaDeleteClone(
#ifdef Altiafp
Altia_Clone_type *
#endif
);

extern ALTIA_BOOLEAN _altiaIsConeFunc(
#ifdef Altiafp
ALTIA_CHAR *
#endif
);

/* DC - 03/31/07 - Changed for performance. */
extern ALTIA_CHAR * _altiaGetCloneFuncNumber(
#ifdef Altiafp
ALTIA_CHAR *,
ALTIA_INT *,
ALTIA_INDEX *
#endif
);

extern ALTIA_BOOLEAN _altiaIsNonClonedParent(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_SHORT
#endif
);

extern Altia_DynamicObject_type *_altiaGetClonesParent(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_SHORT,
ALTIA_INDEX
#endif
);

extern void _altiaCloneUpdateParentExtent(
#ifdef Altiafp
ALTIA_INDEX,
ALTIA_INDEX,
ALTIA_SHORT
#endif
);

extern ALTIA_CHAR *_altiaAllocFuncString(
#ifdef Altiafp
ALTIA_INT,
ALTIA_CHAR *
#endif
);

extern ALTIA_BOOLEAN _altiaMakeFuncString(
#ifdef Altiafp
ALTIA_INT,
ALTIA_CHAR *,
ALTIA_CHAR *,
ALTIA_INT
#endif
);

extern void _altiaSyncCloneExtent();

extern Altia_Timer_type *_altiaGetCloneTimer(
#ifdef Altiafp
ALTIA_INDEX
#endif
);

extern AltiaEventType _altiaCloneTranslateID(
#ifdef Altiafp
AltiaEventType,
AltiaEventType
#endif
);


extern void _altiaSetCloneControl(
#ifdef Altiafp
ALTIA_INT
#endif
);

extern ALTIA_INT _ExecClone;
extern ALTIA_BOOLEAN _CloneOveride;


#endif /* __CLONES__ */

