/* $Revision$    $Date$
 * Copyright (c) 2012 Altia Inc.
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

#ifndef ALTIA_ANIMATION_ID_TABLE
#define ALTIA_ANIMATION_ID_TABLE

#define ALT_ANIM(x) test_ ## x
typedef enum
{
ALT_ANIM(GUI_ctrl_block_label_base_mode),
ALT_ANIM(GUI_ctrl_block_label_character),
ALT_ANIM(GUI_ctrl_block_label_clear),ALT_ANIM(GUI_ctrl_block_label_decimal_pts),
ALT_ANIM(GUI_ctrl_block_label_float),ALT_ANIM(GUI_ctrl_block_label_integer),
ALT_ANIM(GUI_ctrl_block_label_justify_mode),
ALT_ANIM(GUI_ctrl_block_label_set_style),
ALT_ANIM(GUI_ctrl_block_label_slide_append_on),
ALT_ANIM(GUI_ctrl_block_label_text),ALT_ANIM(dash_home_card),
ALT_ANIM(dash_home_change_card),ALT_ANIM(dash_home_dec_bounds_test),
ALT_ANIM(dash_home_dec_card),ALT_ANIM(dash_home_dec_change_val),
ALT_ANIM(dash_home_deckButtonInput),ALT_ANIM(dash_home_decrement_event),
ALT_ANIM(dash_home_inc_bounds_test),ALT_ANIM(dash_home_inc_card),
ALT_ANIM(dash_home_inc_change_val),ALT_ANIM(dash_home_increment_event),
ALT_ANIM(dash_home_jump_to_card),ALT_ANIM(dash_home_loop),
ALT_ANIM(dash_home_looptype),ALT_ANIM(dash_home_max_card),
ALT_ANIM(dash_home_min_card),ALT_ANIM(dash_home_once),
ALT_ANIM(dash_home_once_animation_done),
ALT_ANIM(dash_home_pingpong),ALT_ANIM(dash_home_pointerLoop),
ALT_ANIM(dash_home_pointingToLayer),ALT_ANIM(dash_home_pongDirection),
ALT_ANIM(dash_home_slider_or_knob_flag),
ALT_ANIM(dash_home_startTimer),ALT_ANIM(dash_home_stop_animation),
ALT_ANIM(dash_home_timerToggler),ALT_ANIM(dash_home_timer_event),
ALT_ANIM(dash_home_toggle_timer),ALT_ANIM(status_bar_card),
ALT_ANIM(status_bar_change_card),ALT_ANIM(status_bar_dec_bounds_test),
ALT_ANIM(status_bar_dec_card),ALT_ANIM(status_bar_dec_change_val),
ALT_ANIM(status_bar_deckButtonInput),ALT_ANIM(status_bar_decrement_event),
ALT_ANIM(status_bar_inc_bounds_test),ALT_ANIM(status_bar_inc_card),
ALT_ANIM(status_bar_inc_change_val),ALT_ANIM(status_bar_increment_event),
ALT_ANIM(status_bar_jump_to_card),ALT_ANIM(status_bar_loop),
ALT_ANIM(status_bar_looptype),ALT_ANIM(status_bar_max_card),
ALT_ANIM(status_bar_min_card),ALT_ANIM(status_bar_once),
ALT_ANIM(status_bar_once_animation_done),
ALT_ANIM(status_bar_pingpong),ALT_ANIM(status_bar_pointerLoop),
ALT_ANIM(status_bar_pointingToLayer),ALT_ANIM(status_bar_pongDirection),
ALT_ANIM(status_bar_slider_or_knob_flag),
ALT_ANIM(status_bar_startTimer),ALT_ANIM(status_bar_stop_animation),
ALT_ANIM(status_bar_timerToggler),ALT_ANIM(status_bar_timer_event),
ALT_ANIM(status_bar_toggle_timer)
} ALTIA_ANIMATION_ID;
#endif /* !ALTIA_ANIMATION_ID_TABLE */


#ifndef ALTIA_FONT_ID_TABLE
#define ALTIA_FONT_ID_TABLE

typedef enum
{
 _msdpi_Arial_medium_r_normal____80____________ 
} ALTIA_FONT_ID;
#endif /* !ALTIA_FONT_ID_TABLE */

