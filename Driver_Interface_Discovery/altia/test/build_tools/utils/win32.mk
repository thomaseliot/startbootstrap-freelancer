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

ifndef _UTILS_WIN32_MK_
_UTILS_WIN32_MK_ := 1


################################################################################
# Useful internal build system functions
#
# :NOTE: Tool existence verification
# Getting the file existence verification working was very difficult due to a mixture 
# of DOS path separator characters, spaces in the path / file names, usage of quotes
# to enable the compiler to be called and executed properly from within this makefile,
# and just general Unix/DOS compatibility issues in gmake when trying to do anything
# other than boilerplate type operations.
#
# This link was incredibly useful in getting this working:
#   http://oreilly.com/catalog/make3/book/ch07.pdf
#
################################################################################
func_file_exists = $(call func_common_file_exists,$(call func_sub_dos_2_unix_chars,$(call func_remove_quotes,$1)))
func_find_all_files_of_type = $(shell dir /b /s $1\$2)
func_make_file_writeable = $(shell attrib -r $1)
func_get_cwd = $(shell cd)


################################################################################
# File system utilities
#
# These are currently set up for their DOS-based equivalents
#
# :NOTE:
# Some of these commands rely on cmd.exe extensions to enable more Unix-like
# behavior.  Thus, this build system is only compatible with Windows XP and 
# later.
#
# In particular, the MKDIR command needs extensions enabled to enable easy
# creation of a full directory hierarchy similar the Unix "mkdir -p" command.
################################################################################
CMD_SHELL			:= cmd.exe /E:on /C
CD					:= $(CMD_SHELL) CD
ECHO				:= $(CMD_SHELL) ECHO
ECHO_BLANK_LINE		:= $(CMD_SHELL) ECHO.
LS					:= $(CMD_SHELL) DIR /B
MKDIR				:= $(CMD_SHELL) MKDIR
RM					:= $(CMD_SHELL) DEL /F /Q
RM_R				:= $(CMD_SHELL) DEL /F /Q /S
RMDIR				:= $(CMD_SHELL) RMDIR /Q
RMDIR_R				:= $(CMD_SHELL) RMDIR /S /Q
CP					:= $(CMD_SHELL) COPY /Y
CP_R				:= $(CMD_SHELL) XCOPY /Q /Y /S /E /I
FIND				:= $(CMD_SHELL) DIR /B /S
PWD					:= $(CMD_SHELL) CD


################################################################################
# Build system utilities
################################################################################
MAKE		:= $(BUILD_BIN_PATH)/gmake/make.exe
DOXYGEN		:= $(BUILD_BIN_PATH)/doxygen/doxygen.exe


################################################################################
# By default, all paths should be configured using Unix based path separator 
# characters ie '/' (aka forward slashes).  However, for the DOS-based commands
# to work properly in many of the make rules, the paths must sometimes be
# converted back to their DOS / Windows equivalents for things to work as 
# expected.  The DOS_PATH_SEP_CHAR_REQD macro is checked to see if this needs to
# happen or not.
#
# For Unix-ish platforms, this should be left unset for the "$(if )" functions
# referencing this macro to work correctly.
################################################################################
DOS_PATH_SEP_CHAR_REQD := 1


endif  # ifndef _UTILS_WIN32_MK_

