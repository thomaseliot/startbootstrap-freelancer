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

ifndef _CONFIG_MK_
_CONFIG_MK_ := 1


################################################################################
# Useful internal build system functions
#
# :NOTE: space_char
# This macro must defined here for proper error message printing.  It also gets 
# re-defined in utils.mk.  Since this macro is simply expanded, it's not a
# big deal if it gets re-defined.  
################################################################################
space_char := $(empty) $(empty)


################################################################################
# :NOTE:
# The following are expected to be set in the master makefile:
#	- BUILD_TOOLS_PATH
#	- BUILD_TOOLS_PROFILE
#	- BUILD_TOOLS_HOST_OS
#
# Once these are verified, include the utilities makefile to make life easier
################################################################################
ifeq ($(strip $(BUILD_TOOLS_PATH)),)
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_PATH not set!)
$(info ----                     This environment variable must be set, typically by the master makefile.)
$(info ================================================================================)
$(error Exiting build...)
endif  # ifeq ($(strip $(BUILD_TOOLS_PATH)),)


$(info ================================================================================)
$(info Configuration info:)


#
# Build tool profiles can only be a single word
#
BUILD_TOOLS_PROFILE_SUPPORTED := generic deepscreen

ifeq ($(strip $(BUILD_TOOLS_PROFILE)),)
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_PROFILE not set!)
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_TOOLS_PROFILE_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

else ifeq ($(filter-out $(BUILD_TOOLS_PROFILE_SUPPORTED), $(strip $(BUILD_TOOLS_PROFILE))),)
$(info -    BUILD_TOOLS_PROFILE:    $(BUILD_TOOLS_PROFILE))

else
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_PROFILE set to invalid value:  $(BUILD_TOOLS_PROFILE))
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_TOOLS_PROFILE_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

endif  # ifeq ($(strip $(BUILD_TOOLS_PROFILE)),)


#
# Build tools host OS can only be a single word
#
BUILD_TOOLS_HOST_OS_SUPPORTED := win32 unix

ifeq ($(strip $(BUILD_TOOLS_HOST_OS)),)
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_HOST_OS not set!)
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_TOOLS_HOST_OS_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

else ifeq ($(filter-out $(BUILD_TOOLS_HOST_OS_SUPPORTED), $(strip $(BUILD_TOOLS_HOST_OS))),)
$(info -    BUILD_TOOLS_HOST_OS:    $(BUILD_TOOLS_HOST_OS))

else
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_HOST_OS set to invalid value:  $(BUILD_TOOLS_HOST_OS))
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(BUILD_TOOLS_HOST_OS_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

endif  # ifeq ($(strip $(BUILD_TOOLS_HOST_OS)),)


################################################################################
# Once the above vars are verified, include utils.mk to make life easier
################################################################################
include $(BUILD_TOOLS_PATH)/utils.mk


################################################################################
# More preliminary build checks
################################################################################
#
# Tool chain names can only be a single word
#
TOOLCHAIN_NAME_SUPPORTED := msvs iar gcc ltib ds5 ghs yocto

ifeq ($(strip $(TOOLCHAIN_NAME)),)
$(info ================================================================================)
$(info Configuration error:     TOOLCHAIN_NAME not set!)
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(TOOLCHAIN_NAME_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

else ifeq ($(filter-out $(TOOLCHAIN_NAME_SUPPORTED), $(strip $(TOOLCHAIN_NAME))),)
$(info -    TOOLCHAIN_NAME:         $(TOOLCHAIN_NAME))
$(info -    TOOLCHAIN_VERSION:      $(TOOLCHAIN_VERSION))

else
$(info ================================================================================)
$(info Configuration error:     TOOLCHAIN_NAME set to invalid value:  $(TOOLCHAIN_NAME))
$(info ----                     Valid parameters include:  [$(subst $(space_char), | ,$(TOOLCHAIN_NAME_SUPPORTED))])
$(info ================================================================================)
$(error Exiting build...)

endif  # ifeq ($(strip $(TOOLCHAIN_NAME)),)


################################################################################
# Set up some initial build related paths
#
# :NOTE:
# BUILD_OUTPUT_DIR must either be left empty or specified as a directory WITH the
# trailing "/" character.  If the trailing "/" character is not supplied, then builds
# will not work correctly due the make variable substitutions that occur wrt this
# variable. 
################################################################################
BUILD_BIN_PATH		:= $(BUILD_TOOLS_PATH)/bin
BUILD_OUTPUT_DIR	:=
BUILD_DOCS_DIR		:= docs


################################################################################
# Declare some build defaults
# These are typically overridden by the master makefile or platform / build profile
# specific config
################################################################################
ASM_SOURCE_FILE_LIST				:=
ASM_INCLUDE_PATH_LIST				:=

C_SOURCE_FILE_LIST					:=
C_HEADER_FILE_LIST 					:=
C_INCLUDE_PATH_LIST					:=

CXX_SOURCE_FILE_LIST				:=
CXX_HEADER_FILE_LIST 				:=
CXX_INCLUDE_PATH_LIST				:=

ASM									:=
CC									:=
CXX									:=
AR									:=
LD									:=

ASM_SUFFIX							:=
C_SUFFIX							:=
CXX_SUFFIX							:=
OBJ_SUFFIX							:=
LIB_PREFIX							:=
LIB_SUFFIX							:=
EXE_SUFFIX							:=

ASMDEBUG							:=
ASMDEFINES							:=
ASMFLAGS							:=

CDEBUG								:=
CDEFINES							:=
CFLAGS								:=

CXXDEBUG							:=
CXXDEFINES							:=
CXXFLAGS							:=

ARDEBUG								:=
ARFLAGS								:=

LDDEBUG								:=
LDDEFINES							:=
LDFLAGS								:=

LD_PROGRAM_LIBS_LIST				:=
LD_PROGRAM_LIBS_INCLUDE_PATH_LIST 	:=

BUILD_OUTPUT_LIB_NAME				:=
BUILD_OUTPUT_PROGRAM_NAME			:=


################################################################################
# Set up the build profile specific makefiles now
# Verify file existence; if it doesn't exist, error out
################################################################################

CONFIG_MAKEFILE		:= $(BUILD_TOOLS_PATH)/$(BUILD_TOOLS_PROFILE)/config.mk
CHECKS_MAKEFILE		:= $(BUILD_TOOLS_PATH)/$(BUILD_TOOLS_PROFILE)/checks.mk
RULES_MAKEFILE		:= $(BUILD_TOOLS_PATH)/$(BUILD_TOOLS_PROFILE)/rules.mk

$(if $(call func_file_exists,$(CONFIG_MAKEFILE)),,$(error Configuration error:  Unable to find config makefile: (CONFIG_MAKEFILE):  $(CONFIG_MAKEFILE)))
$(if $(call func_file_exists,$(CHECKS_MAKEFILE)),,$(error Configuration error:  Unable to find checks makefile: (CHECKS_MAKEFILE):  $(CHECKS_MAKEFILE)))
$(if $(call func_file_exists,$(RULES_MAKEFILE)),,$(error Configuration error:  Unable to find rules makefile: (RULES_MAKEFILE):  $(RULES_MAKEFILE)))


################################################################################
# Include the build profile specific makefile 
################################################################################
include $(CONFIG_MAKEFILE)


endif  # ifndef _CONFIG_MK_

