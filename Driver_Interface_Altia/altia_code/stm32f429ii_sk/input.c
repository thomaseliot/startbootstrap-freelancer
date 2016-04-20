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

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "egl_Wrapper.h"
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"

#include "input.h"
#include "os_Wrapper.h"
#include "altiaDebug.h"

/*
 * BSP includes
 */
#include "LCD.h"
#include "AnalogTouchDriver.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef ALTIA_INPUT_DEBUG
#define ALTIA_INPUT_DEBUG           0
#endif

#if ALTIA_INPUT_DEBUG
    #define INPUT_DEBUG(...)        DS_DBG_PRINT(__VA_ARGS__)
#else
    #define INPUT_DEBUG(...)
#endif


#define INVERT_Y_TOUCH_STIM(yStimulusValue)     \
    ( \
        (((EGL_UINT16) (sg_touchInput.screenHeight - 1) - (EGL_UINT16) (yStimulusValue)) < 0) ? \
            (0) : \
            ((EGL_UINT16) (sg_touchInput.screenHeight - 1) - (EGL_UINT16) (yStimulusValue)) \
    )


/******************************************************************************
 * Function prototypes
 ******************************************************************************/



/******************************************************************************
 * Global variables
 ******************************************************************************/


/******************************************************************************
 * Private types
 ******************************************************************************/
typedef enum
{
    INPUT_TOUCH_STATE_NONE = 0,
    INPUT_TOUCH_STATE_TOUCH,
    INPUT_TOUCH_STATE_DRAG,
    INPUT_TOUCH_STATE_RELEASE,
    INPUT_TOUCH_STATE_MAX
} EInputTouchState_t;


typedef struct
{
    EGL_UINT8 enableFlag;
    EGL_UINT16 screenWidth;
    EGL_UINT16 screenHeight;
    EInputTouchState_t state;
    EGL_UINT16 x;
    EGL_UINT16 y;
    uint16_t lcdLastX;
    uint16_t lcdLastY;
    bool lcdLastPen;
} SInputTouchInfo_t;


/******************************************************************************
 * Private variables
 ******************************************************************************/



/******************************************************************************
 * Private functions
 ******************************************************************************/



/******************************************************************************
 * APIs
 ******************************************************************************/
void driver_InputConfig(EGL_UINT16 screenWidth, EGL_UINT16 screenHeight)
{

}


void driver_InputEnable(int enableFlag)
{

}


EGL_STATUS driver_InputEvent(EGL_INPUT_EVENT * pEvent, EGL_TIMEOUT timeout)
{
    int retVal = EGL_STATUS_Q_EMPTY;



    return (retVal);
}

