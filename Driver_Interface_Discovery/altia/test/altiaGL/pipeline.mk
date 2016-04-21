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

ifndef _PIPELINE_MK_
_PIPELINE_MK_ := 1


################################################################################
# Pipeline configuration
#
# :NOTE:
# The intent of this file is to capture pipeline specific info for the following:
#  - source listings
#  - header file include paths 
#  - pipeline relevant C preprocessor definitions (note the generic format)
################################################################################
PIPELINE_C_SOURCE_PATH			:= $(DS_PIPELINE_PATH)
PIPELINE_C_INCLUDE_PATH			:= $(DS_PIPELINE_PATH)

PIPELINE_FB_C_SOURCE_PATH		:= $(PIPELINE_C_SOURCE_PATH)/fb
PIPELINE_FB_C_INCLUDE_PATH		:= $(PIPELINE_C_INCLUDE_PATH)/fb

PIPELINE_MI_C_SOURCE_PATH		:= $(PIPELINE_C_SOURCE_PATH)/mi
PIPELINE_MI_C_INCLUDE_PATH		:= $(PIPELINE_C_INCLUDE_PATH)/mi


C_SOURCE_FILE_LIST += \
	$(PIPELINE_C_SOURCE_PATH)/egl_amask.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_bitmap.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_changegc.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_common.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_cursor.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_data.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_event.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_font.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_layer.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_line.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_pixel.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_polygon.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_rect.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_region.c \
	$(PIPELINE_C_SOURCE_PATH)/egl_text.c \
	$(PIPELINE_C_SOURCE_PATH)/privates.c

C_SOURCE_FILE_LIST += \
	$(PIPELINE_FB_C_SOURCE_PATH)/fballpriv.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbalphablt.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbalphacopy.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbbits.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbblend.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbblt.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbbltone.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbcopy.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbfill.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbfillrect.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbfillsp.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbgc.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbgetsp.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbline.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbpixmap.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbpoint.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbseg.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbsetsp.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbsolid.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbstipple.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbtile.c \
	$(PIPELINE_FB_C_SOURCE_PATH)/fbutil.c

C_SOURCE_FILE_LIST += \
	$(PIPELINE_MI_C_SOURCE_PATH)/mibitblt.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/miexpose.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mifillrct.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/migc.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mipoly.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mipolygen.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mipolypnt.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mipolyrect.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mipolyutil.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/miregion.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/miscrinit.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mispans.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/miwideline.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mizerclip.c \
	$(PIPELINE_MI_C_SOURCE_PATH)/mizerline.c


C_HEADER_FILE_LIST += \
	$(PIPELINE_C_INCLUDE_PATH)/egl_Wrapper.h


################################################################################
# :NOTE:
# Instead of using the "+=" operator here, we want the Altia DeepScreen path to 
# altia.h to be the first in the include list, regardless of include order in the
# target master makefile.
################################################################################

C_INCLUDE_PATH_LIST := \
	$(PIPELINE_C_INCLUDE_PATH)/lib \
	$(C_INCLUDE_PATH_LIST)




C_INCLUDE_PATH_LIST += \
	$(PIPELINE_C_INCLUDE_PATH) \
	$(PIPELINE_FB_C_INCLUDE_PATH) \
	$(PIPELINE_MI_C_INCLUDE_PATH)


CDEFINES += \
	FB_NO_WINDOW_PIXMAPS \
	DONT_INLINE_REGION_OPS


endif  # ifndef _PIPELINE_MK_

