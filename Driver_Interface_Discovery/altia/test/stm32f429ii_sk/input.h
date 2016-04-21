/**
 * @brief   Target user input related functionality
 *
 * @copyright (c) 2015 Altia, Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license.  Permission to sell or
 * distribute this source code is denied.
 *
 * Altia makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _INPUT_H_
#define _INPUT_H_

/******************************************************************************
 * Includes
 ******************************************************************************/


/******************************************************************************
 * Definitions
 ******************************************************************************/


/******************************************************************************
 * Types
 ******************************************************************************/


/******************************************************************************
 * Configuration
 ******************************************************************************/


/******************************************************************************
 * APIs
 ******************************************************************************/
/**
 * @brief  Configure the input event subsystem.
 * @description  This function is driver specific.  It passes in the LCD touch screen
 * height and width so that the LCD touch coordinates provided by the BSP can be
 * converted to the appropriate values for the Altia engine.
 * @param  screenWidth  LCD screen width (in pixels)
 * @param  screenHeight  LCD screen height (in pixels)
 * @return  n/a
 */
extern void driver_InputConfig(EGL_UINT16 screenWidth, EGL_UINT16 screenHeight);


/**
 * @brief  Enable input event subsystem.
 * @description  This function is driver specific.  It enables / disables the input
 * event subsystem.
 * @param  enableFlag  0 == disable, !0 == enable
 * @return  n/a
 */
extern void driver_InputEnable(int enableFlag);


/**
 * @brief  Get an input event.
 * @description  This function is driver/os specific.  It takes a message and turns
 * it into an EGL_EVENT so we it can be sent on to ALTIA_GL and a stimulus event
 * generated if required.  Currently, this target port only supports touch screen
 * events.
 * @param  pEvent  pointer to input event structure to stuff
 * @param  timeout  millisecond count to wait for an input event if one is not detected
 * @return  EGL_STATUS_Q_EMPTY == no input event, EGL_STATUS_Q_DATA == pEvent stuffed with input event data
 */
extern EGL_STATUS driver_InputEvent(EGL_INPUT_EVENT * pEvent, EGL_TIMEOUT timeout);


#endif  // #ifndef _INPUT_H_

