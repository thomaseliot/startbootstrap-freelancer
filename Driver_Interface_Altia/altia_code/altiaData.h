/* $Revision: 1.9 $    $Date: 2009-09-30 23:34:33 $
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
#ifndef ALTIADATA_H
#define ALTIADATA_H


/******************************************************************************
 DYNAMIC OBJECT TYPE
 *****************************************************************************/


/* Not using builtins so object attributes are fixed */
#define DYNAMIC_ATTRIBUTES                      test_dobjsROM
#define DYNAMIC_STATE(i,m)                      (ALTIA_DOBJ_CUSTOM(i) ? test_dstates[ALTIA_DOBJ_STATEINDEX(i)].m : \
                                                                        (ALTIA_INT32)test_dstatesROM[ALTIA_DOBJ_STATEINDEX(i)].m)
#define DYNAMIC_STATE_ADDR(i,m)                 (ALTIA_DOBJ_CUSTOM(i) ? &test_dstates[ALTIA_DOBJ_STATEINDEX(i)].m : NULL)
#define DYNAMIC_SET_STATE(i,m,v)                { if (ALTIA_DOBJ_CUSTOM(i)) test_dstates[ALTIA_DOBJ_STATEINDEX(i)].m = (v); }

/* Constant */
#define ALTIA_DOBJ_ID(i)                        (ALTIA_UINT32)((ALTIA_INT32)test_dobjsROM[(i)].id)
#define ALTIA_DOBJ_DRAWTYPE(i)                  (ALTIA_SHORT)test_dobjsROM[(i)].drawType
#define ALTIA_DOBJ_DRAWINDEX(i)                 (ALTIA_INDEX)test_dobjsROM[(i)].drawIndex
#define ALTIA_DOBJ_PARENT(i)                    (ALTIA_INDEX)DYNAMIC_ATTRIBUTES[(i)].parent
#define ALTIA_DOBJ_FIRSTFUNC(i)                 (ALTIA_INDEX)test_dobjsROM[(i)].firstFunc
#define ALTIA_DOBJ_FUNCCNT(i)                   (ALTIA_SHORT)test_dobjsROM[(i)].funcCnt
#define ALTIA_DOBJ_BEHAVE_RPTX(i)               (ALTIA_FLOAT)test_dobjsROM[(i)].behave_rptx
#define ALTIA_DOBJ_BEHAVE_RPTY(i)               (ALTIA_FLOAT)test_dobjsROM[(i)].behave_rpty
#define ALTIA_DOBJ_GROUPTRANSIDX(i)             (ALTIA_INDEX)test_dobjsROM[(i)].groupTransIdx
#define ALTIA_DOBJ_PREMULT(i)                   (ALTIA_SHORT)test_dobjsROM[(i)].preMult
#define ALTIA_DOBJ_CUSTOM(i)                    (ALTIA_SHORT)test_dobjsROM[(i)].custom
#define ALTIA_DOBJ_STATEINDEX(i)                (ALTIA_SHORT)test_dobjsROM[(i)].stateIndex
#define ALTIA_DOBJ_LAYER(i)                     (ALTIA_SHORT)test_dobjsROM[(i)].layer

/* Variable */
#define ALTIA_DOBJ_FIRSTCHILD(i)                (ALTIA_INDEX)test_dobjsRAM[(i)].firstChild
#define ALTIA_DOBJ_CHILDCNT(i)                  (ALTIA_SHORT)test_dobjsRAM[(i)].childCnt
#define ALTIA_DOBJ_CURRENTFUNC(i)               (ALTIA_INDEX)test_dobjsCurFunc[(i)]
#define ALTIA_DOBJ_CURRENTVALUE(i)              (AltiaEventType)test_dobjsCurValue[(i)]
#define ALTIA_DOBJ_EXTENT_ADDR(i)               (&test_dobjsRAM[(i)].extent)
#define ALTIA_DOBJ_TOTAL_ADDR(i)                (&test_dobjsRAM[(i)].total)
#define ALTIA_DOBJ_TRANS_ADDR(i)                (&test_dobjsRAM[(i)].trans)

/* State Data */
#define ALTIA_DOBJ_MAPPED(i)                    (ALTIA_BOOLEAN)(DYNAMIC_STATE(i,mapped))
#define ALTIA_DOBJ_ALPHA(i)                     (ALTIA_SHORT)(DYNAMIC_STATE(i,alpha))
#define ALTIA_DOBJ_BRUSH(i)                     (ALTIA_BYTE)(DYNAMIC_STATE(i,brush))
#define ALTIA_DOBJ_PATTERN(i)                   (ALTIA_BYTE)(DYNAMIC_STATE(i,pattern))
#define ALTIA_DOBJ_FOREGROUND(i)                (ALTIA_COLOR)(DYNAMIC_STATE(i,foreground))
#define ALTIA_DOBJ_BACKGROUND(i)                (ALTIA_COLOR)(DYNAMIC_STATE(i,background))
#define ALTIA_DOBJ_LASTTRANS_ADDR(i)            DYNAMIC_STATE_ADDR(i,lastTrans)

#define ALTIA_SET_DOBJ_MAPPED(i,v)              DYNAMIC_SET_STATE(i,mapped,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_DOBJ_ALPHA(i,v)               DYNAMIC_SET_STATE(i,alpha,(ALTIA_SHORT)(v))
#define ALTIA_SET_DOBJ_BRUSH(i,v)               DYNAMIC_SET_STATE(i,brush,(ALTIA_BYTE)(v))
#define ALTIA_SET_DOBJ_PATTERN(i,v)             DYNAMIC_SET_STATE(i,pattern,(ALTIA_BYTE)(v))
#define ALTIA_SET_DOBJ_FOREGROUND(i,v)          DYNAMIC_SET_STATE(i,foreground,(ALTIA_COLOR)((ALTIA_INT32)(v)))
#define ALTIA_SET_DOBJ_BACKGROUND(i,v)          DYNAMIC_SET_STATE(i,background,(ALTIA_COLOR)((ALTIA_INT32)(v)))

/* Writers */
#define ALTIA_SET_DOBJ_FIRSTCHILD(i,v)          test_dobjsRAM[(i)].firstChild = (ALTIA_INDEX)(v)
#define ALTIA_SET_DOBJ_CHILDCNT(i,v)            test_dobjsRAM[(i)].childCnt = (ALTIA_INDEX)(v)
#define ALTIA_SET_DOBJ_CURRENTFUNC(i,v)         test_dobjsCurFunc[(i)] = (ALTIA_INDEX)(v)
#define ALTIA_SET_DOBJ_CURRENTVALUE(i,v)        test_dobjsCurValue[(i)] = (AltiaEventType)(v)
#define ALTIA_SET_DOBJ_PARENT(i,v)              DYNAMIC_ATTRIBUTES[(i)].parent = (ALTIA_INDEX)(v)


/******************************************************************************
 STATIC OBJECT TYPE
 *****************************************************************************/


/* Not using builtins so object attributes are fixed */
#define STATIC_ATTRIBUTES                       test_sobjsROM

/* Constant */
#define ALTIA_SOBJ_ID(i)                        (ALTIA_UINT32)((ALTIA_INT32)test_sobjsROM[(i)].id)
#define ALTIA_SOBJ_DRAWTYPE(i)                  (ALTIA_SHORT)test_sobjsROM[(i)].drawType
#define ALTIA_SOBJ_DRAWINDEX(i)                 (ALTIA_INDEX)test_sobjsROM[(i)].drawIndex
#define ALTIA_SOBJ_BASETRANS(i)                 (ALTIA_INDEX)test_sobjsROM[(i)].basetrans
#define ALTIA_SOBJ_MAPPED(i)                    (ALTIA_BOOLEAN)test_sobjsROM[(i)].mapped
#define ALTIA_SOBJ_LAYER(i)                     (ALTIA_SHORT)test_sobjsROM[(i)].layer
#define ALTIA_SOBJ_FOREGROUND(i)                (ALTIA_COLOR)((ALTIA_INT32)STATIC_ATTRIBUTES[(i)].foreground)
#define ALTIA_SOBJ_BACKGROUND(i)                (ALTIA_COLOR)((ALTIA_INT32)STATIC_ATTRIBUTES[(i)].background)
#define ALTIA_SOBJ_BRUSH(i)                     (ALTIA_BYTE)STATIC_ATTRIBUTES[(i)].brush
#define ALTIA_SOBJ_PATTERN(i)                   (ALTIA_BYTE)STATIC_ATTRIBUTES[(i)].pattern
#define ALTIA_SOBJ_PARENT(i)                    (ALTIA_INDEX)STATIC_ATTRIBUTES[(i)].parent
#define ALTIA_SOBJ_ALPHA(i)                     (ALTIA_SHORT)STATIC_ATTRIBUTES[(i)].alpha

/* Variable */
#define ALTIA_SOBJ_TOTAL(i)                     test_sobjsRAM[(i)].total
#define ALTIA_SOBJ_EXTENT(i)                    test_sobjsRAM[(i)].extent
#define ALTIA_SOBJ_OTRANS(i)                    test_sobjsRAM[(i)].otrans

#define ALTIA_SOBJ_TOTAL_ADDR(i)                (&test_sobjsRAM[(i)].total)
#define ALTIA_SOBJ_EXTENT_ADDR(i)               (&test_sobjsRAM[(i)].extent)
#define ALTIA_SOBJ_OTRANS_ADDR(i)               (&test_sobjsRAM[(i)].otrans)

/* Writers */
#define ALTIA_SET_SOBJ_PARENT(i,v)              STATIC_ATTRIBUTES[(i)].parent = (ALTIA_INDEX)(v)
#define ALTIA_SET_SOBJ_FOREGROUND(i,v)          STATIC_ATTRIBUTES[(i)].foreground = (ALTIA_COLOR)(v)
#define ALTIA_SET_SOBJ_BACKGROUND(i,v)          STATIC_ATTRIBUTES[(i)].background = (ALTIA_COLOR)(v)
#define ALTIA_SET_SOBJ_BRUSH(i,v)               STATIC_ATTRIBUTES[(i)].brush = (ALTIA_BYTE)(v)
#define ALTIA_SET_SOBJ_PATTERN(i,v)             STATIC_ATTRIBUTES[(i)].pattern = (ALTIA_BYTE)(v)
#define ALTIA_SET_SOBJ_ALPHA(i,v)               STATIC_ATTRIBUTES[(i)].alpha = (ALTIA_SHORT)(v)


/******************************************************************************
 RASTER OBJECT TYPE
 *****************************************************************************/




/* Constant */
#define ALTIA_RASTER_RPIXELS(i)                 (ALTIA_BINDEX)test_rastersROM[(i)].rpixels
#define ALTIA_RASTER_GPIXELS(i)                 (ALTIA_BINDEX)test_rastersROM[(i)].gpixels
#define ALTIA_RASTER_BPIXELS(i)                 (ALTIA_BINDEX)test_rastersROM[(i)].bpixels
#define ALTIA_RASTER_APIXELS(i)                 (ALTIA_BINDEX)test_rastersROM[(i)].apixels
#define ALTIA_RASTER_MASK(i)                    (ALTIA_BINDEX)test_rastersROM[(i)].mask
#define ALTIA_RASTER_COLORS(i)                  (ALTIA_INDEX)test_rastersROM[(i)].colors
#define ALTIA_RASTER_RCNT(i)                    (ALTIA_UINT32)((ALTIA_INT32)test_rastersROM[(i)].rCnt)
#define ALTIA_RASTER_GCNT(i)                    (ALTIA_UINT32)((ALTIA_INT32)test_rastersROM[(i)].gCnt)
#define ALTIA_RASTER_BCNT(i)                    (ALTIA_UINT32)((ALTIA_INT32)test_rastersROM[(i)].bCnt)
#define ALTIA_RASTER_ACNT(i)                    (ALTIA_UINT32)((ALTIA_INT32)test_rastersROM[(i)].aCnt)
#define ALTIA_RASTER_MASKCNT(i)                 (ALTIA_UINT32)((ALTIA_INT32)test_rastersROM[(i)].maskCnt)
#define ALTIA_RASTER_COLORCNT(i)                (ALTIA_SHORT)test_rastersROM[(i)].colorCnt
#define ALTIA_RASTER_RASTERTYPE(i)              (ALTIA_SHORT)test_rastersROM[(i)].rasterType
#define ALTIA_RASTER_X(i)                       (ALTIA_COORD)test_rastersROM[(i)].x
#define ALTIA_RASTER_Y(i)                       (ALTIA_COORD)test_rastersROM[(i)].y
#define ALTIA_RASTER_WIDTH(i)                   (ALTIA_SHORT)test_rastersROM[(i)].width
#define ALTIA_RASTER_HEIGHT(i)                  (ALTIA_SHORT)test_rastersROM[(i)].height

/* Variable */
#define ALTIA_RASTER_ID(i)                      (ALTIA_UINT32)test_rasterid[(i)].id
#define ALTIA_RASTER_MASKID(i)                  (ALTIA_UINT32)test_rasterid[(i)].maskId
#define ALTIA_RASTER_SAVEMASKBITS(i)            (ALTIA_UINT32)test_rasterid[(i)].saveMaskBits
#define ALTIA_RASTER_SAVEBITS(i)                (ALTIA_UINT32)test_rasterid[(i)].saveBits
#define ALTIA_RASTER_SCALEDID(i)                (ALTIA_UINT32)test_rasterid[(i)].scaledId
#define ALTIA_RASTER_SCALEDMASKID(i)            (ALTIA_UINT32)test_rasterid[(i)].scaledMaskId
#define ALTIA_RASTER_SCALEDMASKBITS(i)          (ALTIA_UINT32)test_rasterid[(i)].scaledMaskBits
#define ALTIA_RASTER_PRIVATE(i,j)               test_rastersROM[(i)].private[(j)]
#define ALTIA_RASTER_PRIVATE_PTR(i)             test_rastersROM[(i)].private

/* Writers */
#define ALTIA_SET_RASTER_ID(i,v)                test_rasterid[(i)].id = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_MASKID(i,v)            test_rasterid[(i)].maskId = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_SAVEMASKBITS(i,v)      test_rasterid[(i)].saveMaskBits = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_SAVEBITS(i,v)          test_rasterid[(i)].saveBits = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_SCALEDID(i,v)          test_rasterid[(i)].scaledId = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_SCALEDMASKID(i,v)      test_rasterid[(i)].scaledMaskId = (ALTIA_UINT32)(v)
#define ALTIA_SET_RASTER_SCALEDMASKBITS(i,v)    test_rasterid[(i)].scaledMaskBits = (ALTIA_UINT32)(v)



/******************************************************************************
 FUNCTION NAME TYPE
 *****************************************************************************/

/* Constant */
#define ALTIA_FNAME_FIRST(i)                    (ALTIA_INDEX)test_funcs[(i)].first
#define ALTIA_FNAME_COUNT(i)                    (ALTIA_SHORT)test_funcs[(i)].count
#define ALTIA_FNAME_OBJECT(i)                   (ALTIA_INDEX)test_funcs[(i)].object
#define ALTIA_FNAME_ANIMATETYPE(i)              (ALTIA_SHORT)test_funcs[(i)].animateType


/******************************************************************************
 CUSTOM I-VALUE TYPE
 *****************************************************************************/

/* Constant */
#define ALTIA_IVAL_ID(i)                        (ALTIA_UINT32)((ALTIA_INT32)test_custIROM[(i)].id)
#define ALTIA_IVAL_MIN(i)                       (ALTIA_INT)test_custIROM[(i)].min
#define ALTIA_IVAL_MAX(i)                       (ALTIA_INT)test_custIROM[(i)].max

/* Variable */
#define ALTIA_IVAL_FLAG(i)                      (ALTIA_UBYTE)((ALTIA_BYTE)test_custIRAM[(i)].flag)
#define ALTIA_IVAL_VALUE(i)                     (ALTIA_INT)test_custIRAM[(i)].value

/* Writers */
#define ALTIA_SET_IVAL_FLAG(i,v)                test_custIRAM[(i)].flag = (ALTIA_BYTE)(v)
#define ALTIA_SET_IVAL_VALUE(i,v)               test_custIRAM[(i)].value = (ALTIA_INT)(v)


/******************************************************************************
 CUSTOM F-VALUE TYPE
 *****************************************************************************/

/* Constant */
#define ALTIA_FVAL_ID(i)                        (ALTIA_UINT32)((ALTIA_INT32)test_custFROM[(i)].id)
#define ALTIA_FVAL_MIN(i)                       (ALTIA_FLOAT)test_custFROM[(i)].min
#define ALTIA_FVAL_MAX(i)                       (ALTIA_FLOAT)test_custFROM[(i)].max

/* Variable */
#define ALTIA_FVAL_FLAG(i)                      (ALTIA_UBYTE)((ALTIA_BYTE)test_custFRAM[(i)].flag)
#define ALTIA_FVAL_VALUE(i)                     (ALTIA_FLOAT)test_custFRAM[(i)].value

/* Writers */
#define ALTIA_SET_FVAL_FLAG(i,v)                test_custFRAM[(i)].flag = (ALTIA_BYTE)(v)
#define ALTIA_SET_FVAL_VALUE(i,v)               test_custFRAM[(i)].value = (ALTIA_FLOAT)(v)


/******************************************************************************
 TIMER TYPE
 *****************************************************************************/

/* Constant */
#define ALTIA_TIMER_STARTCMD(i)                 (ALTIA_BYTE)test_timersROM[(i)].startCmd
#define ALTIA_TIMER_STOPCMD(i)                  (ALTIA_BYTE)test_timersROM[(i)].stopCmd
#define ALTIA_TIMER_STARTVAL(i)                 (AltiaEventType)test_timersROM[(i)].startVal
#define ALTIA_TIMER_STOPVAL(i)                  (AltiaEventType)test_timersROM[(i)].stopVal
#define ALTIA_TIMER_MSECS(i)                    (ALTIA_UINT32)((ALTIA_INT32)test_timersROM[(i)].msecs)
#define ALTIA_TIMER_FIRSTSTATE(i)               (ALTIA_INDEX)test_timersROM[(i)].firstState
#define ALTIA_TIMER_COUNT(i)                    (ALTIA_SHORT)test_timersROM[(i)].count
#define ALTIA_TIMER_OBJIDX(i)                   (ALTIA_INDEX)test_timersROM[(i)].objIdx
#define ALTIA_TIMER_OBJTYPE(i)                  (ALTIA_BYTE)test_timersROM[(i)].objType

/* Variable */
#define ALTIA_TIMER_STOPPED(i)                  (ALTIA_BOOLEAN)test_timersRAM[(i)].stopped

/* Writers */
#define ALTIA_SET_TIMER_STOPPED(i,v)            test_timersRAM[(i)].stopped = (ALTIA_BOOLEAN)(v)


/******************************************************************************
 TEXT-IO OBJECT TYPE
 *****************************************************************************/


/* Not using builtins so object attributes are fixed */
#define TEXTIO_INPUT(i,m)                       ((ALTIA_TEXT_INPUTINDEX(i) >= 0) ? test_textioInputs[ALTIA_TEXT_INPUTINDEX(i)].m : 0)
#define TEXTIO_SET_INPUT(i,m,v)                 { if (ALTIA_TEXT_INPUTINDEX(i) >= 0) test_textioInputs[ALTIA_TEXT_INPUTINDEX(i)].m = (v); }

/* Constant */
#define ALTIA_TEXT_LABELINDEX(i)                (ALTIA_INDEX)test_textiosROM[(i)].labelIndex
#define ALTIA_TEXT_INPUTINDEX(i)                (ALTIA_INDEX)test_textiosROM[(i)].inputIndex

/* Variable */
#define ALTIA_TEXT_LENGTHMODE(i)                (ALTIA_SHORT)test_textiosRAM[(i)].lengthMode
#define ALTIA_TEXT_DECPTS(i)                    (ALTIA_BYTE)test_textiosRAM[(i)].decPts
#define ALTIA_TEXT_APPEND(i)                    (ALTIA_BOOLEAN)test_textiosRAM[(i)].append
#define ALTIA_TEXT_BASE(i)                      (ALTIA_BYTE)test_textiosRAM[(i)].base
#define ALTIA_TEXT_MAX_COUNT(i)                 (ALTIA_SHORT)test_textiosRAM[(i)].max_count
#define ALTIA_TEXT_MAX_PIXEL_COUNT(i)           (ALTIA_SHORT)test_textiosRAM[(i)].max_pixel_count
#define ALTIA_TEXT_JUSTIFY(i)                   (ALTIA_BYTE)test_textiosRAM[(i)].justify
#define ALTIA_TEXT_ROUTING(i)                   (ALTIA_BOOLEAN)test_textiosRAM[(i)].routing
#define ALTIA_TEXT_CLEARNEXT(i)                 (ALTIA_BOOLEAN)test_textiosRAM[(i)].clearNext

/* Variable (inputs) */
#define ALTIA_TEXT_COLOR(i)                     (ALTIA_COLOR)TEXTIO_INPUT(i,color)
#define ALTIA_TEXT_SELECTON(i)                  (ALTIA_BOOLEAN)TEXTIO_INPUT(i,selectOn)
#define ALTIA_TEXT_SHORTCUTON(i)                (ALTIA_BOOLEAN)TEXTIO_INPUT(i,shortcutOn)
#define ALTIA_TEXT_JUMPON(i)                    (ALTIA_BOOLEAN)TEXTIO_INPUT(i,jumpOn)
#define ALTIA_TEXT_CLIP(i)                      (ALTIA_BOOLEAN)TEXTIO_INPUT(i,clip)
#define ALTIA_TEXT_CURSOR(i)                    (ALTIA_SHORT)TEXTIO_INPUT(i,cursor)
#define ALTIA_TEXT_SCROLL_ON(i)                 (ALTIA_BOOLEAN)TEXTIO_INPUT(i,scroll_on)
#define ALTIA_TEXT_ACTUAL_SCROLL_ON(i)          (ALTIA_BOOLEAN)TEXTIO_INPUT(i,actual_scroll_on)
#define ALTIA_TEXT_START(i)                     (ALTIA_SHORT)TEXTIO_INPUT(i,start)
#define ALTIA_TEXT_SELECTSTART(i)               (ALTIA_SHORT)TEXTIO_INPUT(i,selectStart)
#define ALTIA_TEXT_SELECTEND(i)                 (ALTIA_SHORT)TEXTIO_INPUT(i,selectEnd)

/* Variable -- uninitialized */
#define ALTIA_TEXT_DISPSTR(i)                   ((ALTIA_CHAR *)test_textiosBSS[(i)].dispStr)
#define ALTIA_TEXT_DISPSIZE(i)                  (ALTIA_SHORT)test_textiosBSS[(i)].dispSize
#define ALTIA_TEXT_CUSTS(i)                     (Altia_CustomSValue_type *)test_textiosBSS[(i)].custS


#define ALTIA_TEXT_HLCOLOR(i)                   ((ALTIA_TEXT_INPUTINDEX(i) >= 0) ? test_textioInputsBSS[ALTIA_TEXT_INPUTINDEX(i)].hlcolor : NULL)

/* Writers */
#define ALTIA_SET_TEXT_DISPSTR(i,v)             test_textiosBSS[(i)].dispStr = (ALTIA_CHAR *)(v)
#define ALTIA_SET_TEXT_DISPSIZE(i,v)            test_textiosBSS[(i)].dispSize = (ALTIA_SHORT)(v)
#define ALTIA_SET_TEXT_CUSTS(i,v)               test_textiosBSS[(i)].custS = (Altia_CustomSValue_type *)(v)
#define ALTIA_SET_TEXT_LENGTHMODE(i,v)          test_textiosRAM[(i)].lengthMode = (ALTIA_SHORT)(v)
#define ALTIA_SET_TEXT_DECPTS(i,v)              test_textiosRAM[(i)].decPts = (ALTIA_BYTE)(v)
#define ALTIA_SET_TEXT_APPEND(i,v)              test_textiosRAM[(i)].append = (ALTIA_BOOLEAN)(v)
#define ALTIA_SET_TEXT_BASE(i,v)                test_textiosRAM[(i)].base = (ALTIA_BYTE)(v)
#define ALTIA_SET_TEXT_MAX_COUNT(i,v)           test_textiosRAM[(i)].max_count = (ALTIA_SHORT)(v)
#define ALTIA_SET_TEXT_MAX_PIXEL_COUNT(i,v)     test_textiosRAM[(i)].max_pixel_count = (ALTIA_SHORT)(v)
#define ALTIA_SET_TEXT_JUSTIFY(i,v)             test_textiosRAM[(i)].justify = (ALTIA_BYTE)(v)
#define ALTIA_SET_TEXT_ROUTING(i,v)             test_textiosRAM[(i)].routing = (ALTIA_BOOLEAN)(v)
#define ALTIA_SET_TEXT_CLEARNEXT(i,v)           test_textiosRAM[(i)].clearNext = (ALTIA_BOOLEAN)(v)

/* Writers (inputs) */
#define ALTIA_SET_TEXT_COLOR(i,v)               TEXTIO_SET_INPUT(i,color,(ALTIA_COLOR)(v))
#define ALTIA_SET_TEXT_SELECTON(i,v)            TEXTIO_SET_INPUT(i,selectOn,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_SHORTCUTON(i,v)          TEXTIO_SET_INPUT(i,shortcutOn,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_JUMPON(i,v)              TEXTIO_SET_INPUT(i,jumpOn,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_CLIP(i,v)                TEXTIO_SET_INPUT(i,clip,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_CURSOR(i,v)              TEXTIO_SET_INPUT(i,cursor,(ALTIA_SHORT)(v))
#define ALTIA_SET_TEXT_SCROLL_ON(i,v)           TEXTIO_SET_INPUT(i,scroll_on,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_ACTUAL_SCROLL_ON(i,v)    TEXTIO_SET_INPUT(i,actual_scroll_on,(ALTIA_BOOLEAN)(v))
#define ALTIA_SET_TEXT_START(i,v)               TEXTIO_SET_INPUT(i,start,(ALTIA_SHORT)(v))
#define ALTIA_SET_TEXT_SELECTSTART(i,v)         TEXTIO_SET_INPUT(i,selectStart,(ALTIA_SHORT)(v))
#define ALTIA_SET_TEXT_SELECTEND(i,v)           TEXTIO_SET_INPUT(i,selectEnd,(ALTIA_SHORT)(v))

/******************************************************************************
 ALPHA MASK TYPE
 *****************************************************************************/

/* Constant */
#define ALTIA_AMASK_COLORCNT(i)                  (ALTIA_INT)test_alphaMaskROM[(i)].colorCnt
#define ALTIA_AMASK_FIRSTCOLOR(i)                (ALTIA_INDEX)test_alphaMaskROM[(i)].firstColor
#define ALTIA_AMASK_ALPHACNT(i)                  (ALTIA_INT)test_alphaMaskROM[(i)].alphaCnt
#define ALTIA_AMASK_FIRSTALPHA(i)                (ALTIA_INDEX)test_alphaMaskROM[(i)].firstAlpha

/* Variable */
#define ALTIA_AMASK_WIDTH(i)                     test_alphaMaskRAM[(i)].width
#define ALTIA_AMASK_HEIGHT(i)                    test_alphaMaskRAM[(i)].height
#define ALTIA_AMASK_MODE(i)                      test_alphaMaskRAM[(i)].mode
#define ALTIA_AMASK_FLAGS(i)                     test_alphaMaskBSS[(i)]

/* Writers */
#define ALTIA_SET_AMASK_WIDTH(i,v)               test_alphaMaskRAM[(i)].width = (ALTIA_USHORT)(v)
#define ALTIA_SET_AMASK_HEIGHT(i,v)              test_alphaMaskRAM[(i)].height = (ALTIA_USHORT)(v)
#define ALTIA_SET_AMASK_MODE(i,v)                test_alphaMaskRAM[(i)].mode = (ALTIA_UBYTE)(v)
#define ALTIA_SET_AMASK_FLAG(i,v)                test_alphaMaskBSS[(i)] |= (Altia_AlphaMaskBSS_type)(v)
#define ALTIA_CLR_AMASK_FLAG(i,v)                test_alphaMaskBSS[(i)] &= ~(Altia_AlphaMaskBSS_type)(v)


#endif /* ALTIADATA_H */
