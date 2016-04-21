/* $Revision: 1.4 $    $Date: 2007-03-09 02:19:59 $
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
 * This file contains the control code from the design converted to
 * C code.  If the design has no control code or if no control code
 * was selected for code generation, this file will be empty.
 */
#ifndef ALTIACONTROL_H
#define ALTIACONTROL_H




typedef struct
{
    ALTIA_INDEX    index;
    ALTIA_INDEX    nameIndex;

 }Altia_WhenDelay_type;



typedef struct 
{
    void *p;
    int vtype;
} VArg_t;

#ifndef ALTIA_CNTLQ_SIZE
#define ALTIA_CNTLQ_SIZE 256
#endif

/* This test_Control_type can't be in the
 * test_Data_type structure since globalVars make
 * it to design dependent (ie it can never be copied
 * to the AltiaData_type structure).  So we keep it it
 * a design dependent data structure and always access it
 * that way
 */
typedef struct
{




Altia_WhenDelay_type _whenDelay[ALTIA_CNTLQ_SIZE];


ALTIA_INT _delayTail;
ALTIA_INT _delayHead;



} test_Control_type;



#endif /* ALTIACONTROL_H */
