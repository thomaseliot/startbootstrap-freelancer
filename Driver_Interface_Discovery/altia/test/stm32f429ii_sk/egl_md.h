/**
 * @brief   This file contains declarations for the items that change most often
 * for different deployments of the this target driver.
 *
 * @copyright (c) 2015 Altia Inc.
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

#ifndef _EGL_MD_H_
#define _EGL_MD_H_

#include <stddef.h>     // for size_t


#ifndef __altiaWindowDefined__
#define __altiaWindowDefined__
typedef void* ALTIA_WINDOW;
#endif  // #ifndef __altiaWindowDefined__


#define ALTIA_DOUBLE_BUFFERING 1
/*******************************************************************************
 * Set ALTIA_DOUBLE_BUFFERING to a 1 if you want the graphics to be drawn to an
 * offscreen pixmap and the blt'ed to the graphics hardware once drawing is
 * complete.  This prevents flashing and possible tearing of the graphics image
 * since the graphics are drawn all at once.  This requires more memory as an
 * offscreen pixmap the size of the display is required. Set
 * ALTIA_DOUBLE_BUFFERING to a 0 if you want the graphics drawing to be drawn
 * directly to the screen.
 ******************************************************************************/

#define ALTIA_ALPHA_BLENDING 1
/*******************************************************************************
 * Set ALTIA_ALPHA_BLENDING to a 1 if you want AltiaGL to perform alpha
 * blending where necessary.  Setting this to 1 will also include the required
 * frame buffer code needed to do the blending. NOTE:  A frame buffer is
 * required for alpha blending.  If you are using a scanline driver you must
 * use double buffering.
 ******************************************************************************/

#define USE_FB
/*******************************************************************************
 * If you leave this define then the library will include references to the
 * frame buffer code.  The frame buffer code is used both to draw to a memory
 * mapped display frame buffer (double buffering) and if the user wants to
 * display text, rasters, or stencils. AltiaGL will override the users choice
 * and defines USE_FB if the design file that code was generated for includes
 * any of these types of objects.  This must be defined for this frame buffer
 * based driver.
 ******************************************************************************/

#define EGL_USE_INIT_REFRESH 0
/*******************************************************************************
 * On some targets the initial refresh of the display is handled by a refresh
 * event after the window has been created.  This means that we don't have to
 * do the initial drawing of the window since the refresh event will come in
 * and that will cause the initial draw.  On those targets set
 * EGL_USE_INIT_REFRESH to 1.  On targets that don't have refresh events,
 * altia will do the initial draw in the init code.  In that case
 * EGL_USE_INIT_REFRESH should be set to 0.
 ******************************************************************************/

#define ALTIA_SEMAPHORES_ENABLED 0
/*******************************************************************************
 * If you are using multiple threads to access the the Altia DeepScreen code,
 * then you want to set the above to 1.  This will protect Altia's
 * non-reentrant code. Set it to 0 if you are not using threads--the code will
 * execute faster.
 ******************************************************************************/

#define ALTIA_SAVE_FONT_BITMAP
/*******************************************************************************
 * If this define is set each monochrome bitmap created when a text char is
 * drawn is saved for when the char is drawn again. It then gets destroyed when
 * the font is destroyed.  This makes text drawing much faster but requires
 * more memory. If this is not set then the monochrome bitmap is created when
 * the text is drawn and is not saved for later use.
 ******************************************************************************/

#define EGL_USE_CURSOR_HANDLING 0
/*******************************************************************************
 * If this define is set AltiaGL will render a mouse cursor to the display.
 * This is useful for Linux target that contain a standard mouse device.
 ******************************************************************************/

#define USE_MOUSE_SIMULATION_KEYS 0
/*******************************************************************************
 * If this define is set the driver will interpret the up, down, right, and
 * left cursor keys as mouse movement.  The enter key (up and down) will be
 * treated as a left mouse button (up and down) event.  This is very useful for
 * systems without a touch screen/mouse device.  In order for this to make
 * sense EGL_USE_CURSOR_HANDLING should be set to a 1 and LINUX_INPUT_DEVICES
 * should reflect a keyboard device.
 *
 * NOTE: defining this will override any stimulus behavior assigned to the up,
 * down, right, left, and enter keys within your Altia Design faceplate.
 ******************************************************************************/

#endif  // #ifndef _EGL_MD_H_
