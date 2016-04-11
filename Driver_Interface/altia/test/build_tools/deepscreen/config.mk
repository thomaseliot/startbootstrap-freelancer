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

ifndef _CONFIG_DEEPSCREEN_MK_
_CONFIG_DEEPSCREEN_MK_ := 1


################################################################################
# Preliminary build checks
################################################################################
#
# Build output artifacts can only be a single word
#
BUILD_OUTPUT_ARTIFACT_SUPPORTED := program_deepscreen program_deepscreen_1_lib library_deepscreen library

ifeq ($(strip $(BUILD_OUTPUT_ARTIFACT)),)
$(info ================================================================================)
$(info Configuration error:     BUILD_OUTPUT_ARTIFACT not set!)
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_OUTPUT_ARTIFACT_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

else ifeq ($(filter-out $(BUILD_OUTPUT_ARTIFACT_SUPPORTED), $(strip $(BUILD_OUTPUT_ARTIFACT))),)
$(info -    BUILD_OUTPUT_ARTIFACT:  $(BUILD_OUTPUT_ARTIFACT))

else
$(info ================================================================================)
$(info Configuration error:     BUILD_OUTPUT_ARTIFACT set to invalid value:  $(BUILD_OUTPUT_ARTIFACT))
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_OUTPUT_ARTIFACT_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

endif  # ifeq ($(strip $(BUILD_OUTPUT_ARTIFACT)),)


#
# Build types can only be a single word
#
BUILD_TYPE_SUPPORTED := debug release

ifeq ($(strip $(BUILD_TYPE)),)
BUILD_TYPE = release
$(info -    BUILD_TYPE:             $(BUILD_TYPE)  (default))

else ifeq ($(filter-out $(BUILD_TYPE_SUPPORTED), $(strip $(BUILD_TYPE))),)
$(info -    BUILD_TYPE:             $(BUILD_TYPE))

else
$(info ================================================================================)
$(info Configuration error:     BUILD_TYPE set to invalid value:  $(BUILD_TYPE))
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_TYPE_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

endif  # ifeq ($(strip $(BUILD_TYPE)),)


#
# If the "editor" rule was supplied, ALTIAHOME better be defined, else the build
# system has no way of locating the appropriate DeepScreen editor API library
#
ifeq ($(strip $(MAKECMDGOALS)),editor)
ifneq ($(strip $(ALTIAHOME)),)
$(info -    ALTIAHOME:              $(ALTIAHOME))

else
$(info ================================================================================)
$(info Configuration error:     ALTIAHOME not set!)
$(info ----                     This environment variable is required for building with)
$(info ----                     the DeepScreen editor API libraries)
$(info ================================================================================)
$(error Exiting build...)

endif  # ifneq ($(strip $(ALTIAHOME)),)
endif  # ifeq ($(strip $(MAKECMDGOALS)),editor)


################################################################################
# Set up the tool chain specific makefile now
# Verify file existence; if it doesn't exist, error out
################################################################################
ifeq ($(strip $(TOOLCHAIN_VERSION)),)
TOOLCHAIN_MAKEFILE := $(BUILD_TOOLS_PATH)/$(BUILD_TOOLS_PROFILE)/toolchain_$(TOOLCHAIN_NAME).mk

else
TOOLCHAIN_MAKEFILE := $(BUILD_TOOLS_PATH)/$(BUILD_TOOLS_PROFILE)/toolchain_$(TOOLCHAIN_NAME)_$(TOOLCHAIN_VERSION).mk

endif  # ifeq ($(strip $(TOOLCHAIN_VERSION)),)

$(if $(call func_file_exists,$(TOOLCHAIN_MAKEFILE)),,$(error Configuration error:  Unable to find tool chain makefile: (TOOLCHAIN_MAKEFILE):  $(TOOLCHAIN_MAKEFILE)))


################################################################################
# Declare new build vars, functions, build defaults, etc.
# Override build defaults as needed
################################################################################
C_SOURCE_FILE_LIST_DS_API :=


ifeq ($(strip $(SRC1BASE)),)
SRC1BASE = test
$(info -    SRC1BASE:               $(SRC1BASE)  (default))

else
$(info -    SRC1BASE:               $(SRC1BASE))

endif  # ifeq ($(strip $(SRC1BASE)),)


$(info -    TARGETDEFS:             $(TARGETDEFS))
$(info -    EXTRASRCS:              $(EXTRASRCS))
$(info -    EXTRALIBS:              $(EXTRALIBS))


#
# :NOTE:
# This is one of the few exceptions where delayed expansion is used to set a variable.
# Delayed expansion is used here to simplify execution since the various prefix 
# and suffix variables haven't been defined yet; the prefix / suffix variables
# aren't defined until the tool chain makefile is included.  Assuming the standard 
# build tools templates for the master makefiles are being used of course...
#
BUILD_OUTPUT_LIB_NAME			= $(LIB_PREFIX)altia$(LIB_SUFFIX)


BUILD_OUTPUT_LIB_NAME_DS_API	= $(LIB_PREFIX)altiaAPIlib$(LIB_SUFFIX)




BUILD_OUTPUT_LIB_NAME_DS_WIN	= $(LIB_PREFIX)altiaWinLib$(LIB_SUFFIX)

BUILD_OUTPUT_PROGRAM_NAME		= $(SRC1BASE)$(EXE_SUFFIX)


#
# Declare some functions to handle altiaUserMain.c file copies
#
func_copy_altia_user_main_default = \
	$(CP) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(DS_CODE_GEN_BASE_PATH)/altiaSimpleMain.c),$(DS_CODE_GEN_BASE_PATH)/altiaSimpleMain.c) \
		altiaUserMain.c


func_copy_altia_user_main_user_supplied = \
	$(CP) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(SRC1BASE).c),$(SRC1BASE).c) \
		altiaUserMain.c \
	&& \
	$(ECHO) ================================================================================ \
	&& \
	$(ECHO) ---- Compiling $(SRC1BASE).c as altiaUserMain.c \
	&& \
	$(ECHO) ---- If there are syntax errors, edit $(SRC1BASE).c to fix them \
	&& \
	$(ECHO) ================================================================================


endif  # ifndef _CONFIG_DEEPSCREEN_MK_

