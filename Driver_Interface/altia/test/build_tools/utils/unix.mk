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

ifndef _UTILS_UNIX_MK_
_UTILS_UNIX_MK_ := 1


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
func_file_exists = $(call func_common_file_exists,$(call func_remove_quotes,$1))
func_find_all_files_of_type = $(shell find $1 -iname $2)
func_make_file_writeable = $(shell chmod 666 $1)
func_get_cwd = $(shell pwd)


################################################################################
# File system utilities
#
# These are currently set up for their Unix-based equivalents
################################################################################
CMD_SHELL			:=
CD					:= cd
ECHO				:= echo
ECHO_BLANK_LINE		:= echo
LS					:= ls
MKDIR				:= mkdir -p
RM					:= rm -f
RM_R				:= rm -rf
RMDIR				:= rmdir
RMDIR_R				:= rmdir
CP					:= cp -f
CP_R				:= cp -rf
FIND				:= find . -iname
PWD					:= pwd


################################################################################
# Build system utilities
#
# For more info on tar, see here:
#	- http://linux.die.net/man/1/tar
################################################################################
MAKE		:= make
DOXYGEN		:= doxygen


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
DOS_PATH_SEP_CHAR_REQD :=


endif  # ifndef _UTILS_UNIX_MK_

