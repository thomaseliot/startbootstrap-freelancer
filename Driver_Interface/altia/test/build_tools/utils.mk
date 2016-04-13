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

ifndef _UTILS_MK_
_UTILS_MK_ := 1


################################################################################
# Useful internal build system functions
################################################################################
space_char := $(empty) $(empty)
func_space_to_question = $(subst $(space_char),?,$1)
func_wildcard_spaces = $(wildcard $(call func_space_to_question,$1))
func_remove_quotes = $(subst ",,$1)
func_sub_dos_2_unix_chars = $(subst \,/,$1)
func_sub_unix_2_dos_chars = $(subst /,\,$1)
func_common_file_exists = $(strip $(if $1,,$(warning $$1 has no value)) $(call func_wildcard_spaces,$1))


################################################################################
# Assume the specified platform utils makefile exists as it should have already
# been checked in config.mk
################################################################################
include $(BUILD_TOOLS_PATH)/utils/$(BUILD_TOOLS_HOST_OS).mk


endif  # ifndef _UTILS_MK_

