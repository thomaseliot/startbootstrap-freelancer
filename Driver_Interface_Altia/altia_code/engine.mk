################################################################################
#
# @copyright Copyright (c) 2014 Altia Inc.
#
# Permission to use, copy, and modify this software for any purpose is 
# hereby granted, provided that this copyright notice appears in all 
# copies and that the entity using, copying, or modifying this software 
# has purchased an Altia Deep Screen license. Permission to sell or 
# distribute this source code is denied.
#
# Altia makes no representations about the suitability of this software for any 
# purpose.  It is provided "as is" without express or implied warranty.
#
# ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
# IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
################################################################################

ifndef _ENGINE_MK_
_ENGINE_MK_ := 1


################################################################################
# Engine configuration
#
# :NOTE:
# The intent of this file is to capture pipeline specific info for the following:
#  - source listings
#  - header file include paths 
#  - graphics engine relevant C preprocessor definitions (note the generic format)
################################################################################
ENGINE_C_SOURCE_PATH			:= $(DS_ENGINE_PATH)
ENGINE_C_INCLUDE_PATH			:= $(DS_ENGINE_PATH)

ENGINE_COMMON_C_SOURCE_PATH		:= $(ENGINE_C_SOURCE_PATH)/common
ENGINE_COMMON_C_INCLUDE_PATH	:= $(ENGINE_C_INCLUDE_PATH)/common

ENGINE_SHAPING_C_SOURCE_PATH	:= $(ENGINE_C_SOURCE_PATH)/shaping
ENGINE_SHAPING_C_INCLUDE_PATH	:= $(ENGINE_C_INCLUDE_PATH)/shaping


C_SOURCE_FILE_LIST_DS_API := \
	$(ENGINE_C_SOURCE_PATH)/altiaAPI.c \
	$(ENGINE_C_SOURCE_PATH)/altiaAPICbk.c


C_SOURCE_FILE_LIST += \
	$(ENGINE_C_SOURCE_PATH)/altiaLibAlphaMask.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibClip.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibExtent.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibFont.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibFxmath.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibFxtrig.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibGeom.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibLPlot.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibMultiPlot.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibSSChart.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibTick.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibTrans.c \
	$(ENGINE_C_SOURCE_PATH)/altiaLibUtils.c \
	$(ENGINE_C_SOURCE_PATH)/animate.c \
	$(ENGINE_C_SOURCE_PATH)/builtins.c \
	$(ENGINE_C_SOURCE_PATH)/circbuffer.c \
	$(ENGINE_C_SOURCE_PATH)/control.c \
	$(ENGINE_C_SOURCE_PATH)/custAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/data.c \
	$(ENGINE_C_SOURCE_PATH)/displayAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/draw.c \
	$(ENGINE_C_SOURCE_PATH)/drawAreaAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/dsDrawEx.c \
	$(ENGINE_C_SOURCE_PATH)/dsPlotEx.c \
	$(ENGINE_C_SOURCE_PATH)/fpplotAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/gldata.c \
	$(ENGINE_C_SOURCE_PATH)/tdsceneAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/imageAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/languageAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/languageData.c \
	$(ENGINE_C_SOURCE_PATH)/mltextAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/multiplotAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/openglAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/pieAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/rect3dAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/skinAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/skinData.c \
	$(ENGINE_C_SOURCE_PATH)/snapshotAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/soundAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/sschartAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/stimulus.c \
	$(ENGINE_C_SOURCE_PATH)/tickmarkAnimate.c \
	$(ENGINE_C_SOURCE_PATH)/textioAnimate.c


#
# Legacy artifact of code generation
# Contains important image related C source files that need to be built
#
# :NOTE:  Default path for altimage.mk
# We are relying on gmake's convention to look for a specified makefile at the directory
# level make was invoked at.  Essentially, it's using the $(CURDIR) variable under
# the covers.  $(CURDIR) used to be explicitly specified here, but it caused problems
# when the path had spaces.  
#
include altimage.mk
C_SOURCE_FILE_LIST += \
	$(ALTIA_IMAGE_SOURCES)


C_SOURCE_FILE_LIST += \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaDraw.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaInput.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibBlurFilter.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibClipBoard.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibColor.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibCurve.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibDash.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibDIB.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibDraw.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibEllipse.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibError.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibExRaster.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibFlashPlayer.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibLabel.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibLine.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibPathObject.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibPattern.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibPolygon.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibRaster.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibScale.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibSem.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibSnapshot.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibSound.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibSpline.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibStencil.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibTDScene.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibWFont.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaLibWindow.c \
	$(ENGINE_COMMON_C_SOURCE_PATH)/altiaUtils.c






C_HEADER_FILE_LIST += \
	$(ENGINE_C_INCLUDE_PATH)/altiaBase.h \
	$(ENGINE_C_INCLUDE_PATH)/altiaData.h \
	$(ENGINE_C_INCLUDE_PATH)/altiaExtern.h \
	$(ENGINE_C_INCLUDE_PATH)/altiaImageAccess.h \
	$(ENGINE_C_INCLUDE_PATH)/altiaTypes.h


C_INCLUDE_PATH_LIST += \
	$(ENGINE_C_INCLUDE_PATH) \
	$(ENGINE_COMMON_C_INCLUDE_PATH)






#
# :NOTE:  DeepScreen default C preprocessor macros
# All targets should have the following C preprocessor macros by default!
#
# :NOTE:  ALTIA_ALIGN in CDEFINES
# For some targets, the ALTIA_ALIGN preprocessor definition will need to be tweaked.
# Refer to the build system user's guide for examples on how to use the gmake "filter-out"
# function to acccomplish this.
# Example:
#   ALTIA_ALIGN="__attribute__((aligned(64)))"
#
CDEFINES += \
	DEEPSCREEN \
	ALTIAGL \
	Altiafp \
	AltiaFunctionPrototyping \
	NeedNestedPrototypes \
	NeedFunctionPrototypes \
	AltiaUseAnimationIds=1 \
	ALTIA_ALIGN="" \
	ALTIA_CONST=const



ifeq ($(strip $(MAKECMDGOALS)),editor)
CDEFINES += \
	ALTIALIB
endif  # ifeq ($(strip $(MAKECMDGOALS)),editor)




CDEFINES += \
	ALTIAINTAPI





endif  # ifndef _ENGINE_MK_

