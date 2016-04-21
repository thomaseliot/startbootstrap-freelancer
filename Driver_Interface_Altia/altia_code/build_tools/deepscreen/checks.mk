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

ifndef _CHECKS_DEEPSCREEN_MK_
_CHECKS_DEEPSCREEN_MK_ := 1


################################################################################
# Error, configuration, etc. checking
################################################################################
#
# Verify tool chain binary existence
#
ifneq ($(strip $(ASM)),)
$(if $(call func_file_exists,$(ASM)),,$(error Configuration error:  Unable to find tool: (ASM):  $(ASM)))
endif


ifneq ($(strip $(CC)),)
$(if $(call func_file_exists,$(CC)),,$(error Configuration error:  Unable to find tool: (CC):  $(CC)))
endif


ifneq ($(strip $(CXX)),)
$(if $(call func_file_exists,$(CXX)),,$(error Configuration error:  Unable to find tool: (CXX):  $(CXX)))
endif


ifneq ($(strip $(AR)),)
$(if $(call func_file_exists,$(AR)),,$(error Configuration error:  Unable to find tool: (AR):  $(AR)))
endif


ifneq ($(strip $(LD)),)
$(if $(call func_file_exists,$(LD)),,$(error Configuration error:  Unable to find tool: (LD):  $(LD)))
endif


#
# Verify individual file existence for each file the source and header file lists
#
ifneq ($(strip $(ASM_SOURCE_FILE_LIST)),)
$(foreach fileName,$(ASM_SOURCE_FILE_LIST),$(if $(call func_file_exists,$(fileName)),,$(error Configuration error:  Unable to find file in (ASM_SOURCE_FILE_LIST):  $(fileName))))
endif


ifneq ($(strip $(C_SOURCE_FILE_LIST)),)
$(foreach fileName,$(C_SOURCE_FILE_LIST),$(if $(call func_file_exists,$(fileName)),,$(error Configuration error:  Unable to find file in (C_SOURCE_FILE_LIST):  $(fileName))))
endif


ifneq ($(strip $(C_HEADER_FILE_LIST)),)
$(foreach fileName,$(C_HEADER_FILE_LIST),$(if $(call func_file_exists,$(fileName)),,$(error Configuration error:  Unable to find file in (C_HEADER_FILE_LIST):  $(fileName))))
endif


ifneq ($(strip $(CXX_SOURCE_FILE_LIST)),)
$(foreach fileName,$(CXX_SOURCE_FILE_LIST),$(if $(call func_file_exists,$(fileName)),,$(error Configuration error:  Unable to find file in (CXX_SOURCE_FILE_LIST):  $(fileName))))
endif


ifneq ($(strip $(CXX_HEADER_FILE_LIST)),)
$(foreach fileName,$(CXX_HEADER_FILE_LIST),$(if $(call func_file_exists,$(fileName)),,$(error Configuration error:  Unable to find file in (CXX_HEADER_FILE_LIST):  $(fileName))))
endif


#
# BUILD_OUTPUT_DIR must either be left empty or specified as a directory WITH the
# trailing "/" character.  If the trailing "/" character is not supplied, then builds
# will not work correctly due the make variable substitutions that occur wrt this
# variable. 
#
ifneq ($(strip $(BUILD_OUTPUT_DIR)),)
ifeq ($(patsubst %/,%,$(BUILD_OUTPUT_DIR)),$(BUILD_OUTPUT_DIR))
$(info ================================================================================)
$(info Configuration error:     BUILD_OUTPUT_DIR set to invalid value:  $(BUILD_OUTPUT_DIR))
$(info ----                     BUILD_OUTPUT_DIR must have a trailing "/"!)
$(info ================================================================================)
$(error Exiting build...)
endif  # ifeq ($(patsubst %/,%,$(BUILD_OUTPUT_DIR)),$(BUILD_OUTPUT_DIR))
endif  # ifneq ($(strip $(BUILD_OUTPUT_DIR)),)


endif  # ifndef _CHECKS_DEEPSCREEN_MK_

