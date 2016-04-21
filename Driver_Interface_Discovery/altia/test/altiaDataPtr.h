/* $Revision: 1.9 $    $Date: 2009-09-30 23:33:52 $
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


#include "altiaControl.h"



typedef struct
{

ALTIA_INT _window_width;
ALTIA_INT _window_height;
ALTIA_INT _window_xoffset;
ALTIA_INT _window_yoffset;
ALTIA_COLOR _window_background;

ALTIA_INT _dobjs_count;
Altia_DynamicObject_type *_dobjs;

ALTIA_INT _sobjs_count;
Altia_StaticObject_type *_sobjs;
Altia_StaticObjectC_type *_sobjsc;

ALTIA_INT _states_count;
Altia_StateEntry_type *_states;

ALTIA_INT _funcs_count;
Altia_FunctionName_type *_funcs;

ALTIA_INT _funcIndex_count;
Altia_FunctionIndex_type *_funcIndex;
AltiaEventType *_curValue;

ALTIA_INT _funcFIndexs_count;
Altia_FunctionFIndex_type *_funcFIndexs;

ALTIA_INT _basetrans_count;
Altia_BaseTrans_type *_basetrans;

ALTIA_INT _offtrans_count;
Altia_OffsetTrans_type *_offtrans;

Altia_NonaffineHdr_type _nonaffineHdr;
Altia_Nonaffine_type *_nonaffine;

ALTIA_INT _grouptrans_count;
Altia_Transform_type *_grouptrans;

ALTIA_INT _children_count;
Altia_ChildEntry_type *_children;

ALTIA_INT _sequence_count;
Altia_Sequence_type *_sequence;

ALTIA_INT _colorMap_count;
Altia_ColorEntry_type *_colorMap;

ALTIA_INT _patterns_count;
Altia_Pattern_type *_patterns;

ALTIA_INT _brushes_count;
Altia_Brush_type *_brushes;

ALTIA_INT _fonts_count;
Altia_Font_type *_fonts;

ALTIA_INT _ellipses_count;
Altia_Ellipse_type *_ellipses;

ALTIA_INT _lines_count;
Altia_NCoord_type *_lines;

ALTIA_INT _rects_count;
Altia_Rect_type *_rects;

ALTIA_INT _poly_count;
Altia_NCoord_type *_poly;

ALTIA_INT _fpoly_count;
Altia_NCoord_type *_fpoly;

ALTIA_INT _splines_count;
Altia_NCoord_type *_splines;

ALTIA_INT _csplines_count;
Altia_NCoord_type *_csplines;

ALTIA_INT _fsplines_count;
Altia_NCoord_type *_fsplines;

ALTIA_INT _labels_count;
Altia_Label_type *_labels;

ALTIA_INT _stencils_count;
Altia_Stencil_type *_stencils;
Altia_StencilID_type *_stencilid;

ALTIA_INT _rasters_count;
Altia_Raster_type *_rasters;

ALTIA_INT _decks_count;
Altia_Deck_type *_decks;

ALTIA_INT _clips_count;
Altia_Clip_type *_clips;

ALTIA_INT _lplots_count;
Altia_LinePlot_type *_lplots;

ALTIA_INT _fpplots_count;
Altia_FillPolyPlot_type *_fpplots;

ALTIA_INT _dpoly_count;
Altia_DynamicPoly_type *_dpoly;

ALTIA_INT _dline_count;
Altia_DynamicLine_type *_dline;

ALTIA_INT _ticks_count;
Altia_Tick_type *_ticks;

ALTIA_INT _sscharts_count;
Altia_StripChart_type *_sscharts;

ALTIA_INT _coords_count;
Altia_Coord_type *_coords;

ALTIA_INT _extents_count;
ALTIA_INT _extents_max;
Altia_Extent_type *_extents;

ALTIA_INT _custI_count;
Altia_CustomIValue_type *_custI;

ALTIA_INT _custF_count;
Altia_CustomFValue_type *_custF;

ALTIA_INT _custS_count;
Altia_CustomSValue_type *_custS;

ALTIA_INT _textios_count;
Altia_Textio_type *_textios;

ALTIA_INT _rect3d_count;
Altia_Rect3d_type *_rect3d;

ALTIA_INT _mltexts_count;
Altia_MLineText_type *_mltexts;

ALTIA_INT _rectInput_count;
Altia_RectInput_type *_rectInput;

ALTIA_INT _polarInput_count;
Altia_PolarInput_type *_polarInput;

ALTIA_INT _sensorInput_count;
Altia_SensorInput_type *_sensorInput;

ALTIA_INT _istateObj_count;
Altia_IStateObj_type *_istateObj;

ALTIA_INT _inputSeq_count;
Altia_InputSequence_type *_inputSeq;

ALTIA_INT _links_count;
Altia_LinkConnection_type *_links;

ALTIA_INT _connects_count;
Altia_Connection_type *_connects;

ALTIA_INT _timerState_count;
Altia_TimerState_type *_timerState;

ALTIA_INT _timers_count;
Altia_Timer_type *_timers;

ALTIA_INT _preInput_count;
Altia_InputSequence_type  *_preInput;

ALTIA_INT _preSeq_count;
Altia_Sequence_type *_preSeq;

ALTIA_INT _pies_count;
Altia_Pie_type *_pies;

ALTIA_INT _pieSlices_count;
Altia_PieSlice_type  *_pieSlices;

ALTIA_INT _soundobjs_count;
Altia_SoundObj_type *_soundobjs;

ALTIA_INT _imageobjs_count;
Altia_ImageObj_type *_imageobjs;

ALTIA_INT _snapshots_count;
Altia_Snapshot_type *_snapshots;

ALTIA_INT _drawArea_count;
Altia_DrawingArea_type *_drawArea;

ALTIA_INT _opengl_count;
Altia_OpenGLObj_type *_opengl;

ALTIA_INT _tdscenes_count;
Altia_TDScene_type *_tdscenes;

ALTIA_INT _specifiers_count;
Altia_Specifier_type *_specifiers;

ALTIA_INT _properties_count;
Altia_Property_type *_properties;



} AltiaData_type;

extern AltiaData_type *_altiaDataPtr;

extern test_Control_type *_altiaControlPtr;

