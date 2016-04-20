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

ifndef _RULES_DEEPSCREEN_MK_
_RULES_DEEPSCREEN_MK_ := 1


################################################################################
# Do some preliminary make rules setup work
################################################################################
#
# :NOTE: Single library for full DeepScreen targets
# The de-facto convention for full DeepScreen target builds is 2 libraries: one
# for the API, one for the design data, engine, etc.  The "library_deepscreen" rule
# is used to accomplish this type of build.
# If a single, combined library is desired, the "library" rule is used.  This is
# the expectation for miniGL targets.  For full DeepScreen targets, the API source
# file list (C_SOURCE_FILE_LIST_DS_API) must be added to the standard source file
# list so its source files get built and included in the resulting single library 
# build artifact.  The C_SOURCE_FILE_LIST_DS_API source list is cleared out after
# it's added to the standard source file list (C_SOURCE_FILE_LIST) so that it can't 
# be acted on in accidentally in other rules.
# 
ifeq ($(filter-out program_deepscreen_1_lib library, $(strip $(BUILD_OUTPUT_ARTIFACT))),)
C_SOURCE_FILE_LIST := $(C_SOURCE_FILE_LIST_DS_API) $(C_SOURCE_FILE_LIST)
C_SOURCE_FILE_LIST_DS_API :=
endif  # ifeq ($(filter-out program library, $(strip $(BUILD_OUTPUT_ARTIFACT))),)

ASM_SOURCE_OBJECT_LIST			:= $(patsubst %$(ASM_SUFFIX),$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX),$(ASM_SOURCE_FILE_LIST))
ASM_SOURCE_FILE_LIST_DIRS 		:= $(foreach asmSourceDir,$(sort $(dir $(ASM_SOURCE_FILE_LIST))),$(if $(call func_file_exists,"$(BUILD_OUTPUT_DIR)$(asmSourceDir)"),,$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(asmSourceDir)"),"$(BUILD_OUTPUT_DIR)$(asmSourceDir)")))

C_SOURCE_OBJECT_LIST			:= $(patsubst %$(C_SUFFIX),$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX),$(C_SOURCE_FILE_LIST))
C_SOURCE_OBJECT_LIST			+= $(patsubst %$(C_SUFFIX),$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX),$(EXTRASRCS))
C_SOURCE_FILE_LIST_DIRS 		:= $(foreach cSourceDir,$(sort $(dir $(C_SOURCE_FILE_LIST))),$(if $(call func_file_exists,"$(BUILD_OUTPUT_DIR)$(cSourceDir)"),,$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(cSourceDir)"),"$(BUILD_OUTPUT_DIR)$(cSourceDir)")))

C_SOURCE_OBJECT_LIST_DS_API		:= $(patsubst %$(C_SUFFIX),$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX),$(C_SOURCE_FILE_LIST_DS_API))
C_SOURCE_FILE_LIST_DIRS_DS_API	:= $(foreach cSourceDir,$(sort $(dir $(C_SOURCE_FILE_LIST_DS_API))),$(if $(call func_file_exists,"$(BUILD_OUTPUT_DIR)$(cSourceDir)"),,$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(cSourceDir)"),"$(BUILD_OUTPUT_DIR)$(cSourceDir)")))

CXX_SOURCE_OBJECT_LIST			:= $(patsubst %$(CXX_SUFFIX),$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX),$(CXX_SOURCE_FILE_LIST))
CXX_SOURCE_FILE_LIST_DIRS 		:= $(foreach cxxSourceDir,$(sort $(dir $(CXX_SOURCE_FILE_LIST))),$(if $(call func_file_exists,"$(BUILD_OUTPUT_DIR)$(cxxSourceDir)"),,$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(cxxSourceDir)"),"$(BUILD_OUTPUT_DIR)$(cxxSourceDir)")))


################################################################################
# Make rules
#
# :NOTE:
# createBuildOutputDir is added as pre-requisite to many other rules to avoid 
# bogus error / warning messages from the shell.
#
# :NOTE:
# Most, if not all, of the rules related to object file creation are using "static
# pattern" rules.  To get these to work as desired, ie only rebuilding an object 
# file when the corresponding source file or relevant header files are changed, 
# "order-only prerequisites" are used for build output directory creation rules.
#
# These are needed because any time a directory is created using the build output 
# directory creation rules, it's timestamp is updated, even if it already exists.
# Without making these rules "order-only prerequisites", make sees the directory 
# timestamp as updated which indicates a change, and it will then  rebuild all the 
# targets ie all the source files, even though none of them may have actually changed.
# Thus, all build output directory creation rules are specified as order-only 
# prerequisites for the object file creation rules.
#
# Also note, that the corresponding source file suffix designator must be supplied 
# for each static pattern object file creation rule.  This is required so that make 
# doesn't try and be smart and use the desired built-in implicit rule's build cmd 
# under the covers, but instead, actually uses the build cmd macros defined in the 
# tool chain makefile.  It also forces make to detect when the source file (C, C++,
# assembly, etc.) has changed, and its corresponding object file must be rebuilt.
################################################################################
.PHONY: \
	default \
	standalone \
	editor \
	createBuildOutputDir \
	createAsmSourceBuildOutputDirs \
	createCSourceBuildOutputDirs \
	createCSourceBuildOutputDirs_ds_api \
	createCxxSourceBuildOutputDirs \
	createDocsDir \
	copyAltiaUserMain \
	program \
	library_deepscreen \
	library \
	clean \
	cleanProgram \
	cleanLibrary_ds_api \
	cleanLibrary_ds_win \
	cleanLibrary \
	cleanObjects \
	docs


default: $(BUILD_OUTPUT_ARTIFACT)


standalone: default


editor: program_deepscreen
	@-$(ECHO) Done  --  $(@)


createBuildOutputDir:
	@-$(if $(strip $(BUILD_OUTPUT_DIR)),$(if $(call func_file_exists,$(BUILD_OUTPUT_DIR)),,$(ECHO) Creating build output dir:  $(BUILD_OUTPUT_DIR)),)
	@-$(if $(strip $(BUILD_OUTPUT_DIR)),$(if $(call func_file_exists,$(BUILD_OUTPUT_DIR)),,$(MKDIR) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(BUILD_OUTPUT_DIR)),$(BUILD_OUTPUT_DIR))),)


createAsmSourceBuildOutputDirs: createBuildOutputDir
	@-$(if $(strip $(ASM_SOURCE_FILE_LIST_DIRS)),$(ECHO) Creating build output subdirs in:  $(BUILD_OUTPUT_DIR),)
	@-$(if $(strip $(ASM_SOURCE_FILE_LIST_DIRS)),$(MKDIR) $(ASM_SOURCE_FILE_LIST_DIRS),)


createCSourceBuildOutputDirs: createBuildOutputDir
	@-$(if $(strip $(C_SOURCE_FILE_LIST_DIRS)),$(ECHO) Creating build output subdirs in:  $(BUILD_OUTPUT_DIR),)
	@-$(if $(strip $(C_SOURCE_FILE_LIST_DIRS)),$(MKDIR) $(C_SOURCE_FILE_LIST_DIRS),)


createCSourceBuildOutputDirs_ds_api: createBuildOutputDir
	@-$(if $(strip $(C_SOURCE_FILE_LIST_DIRS_DS_API)),$(ECHO) Creating build output subdirs in:  $(BUILD_OUTPUT_DIR),)
	@-$(if $(strip $(C_SOURCE_FILE_LIST_DIRS_DS_API)),$(MKDIR) $(C_SOURCE_FILE_LIST_DIRS_DS_API),)


createCxxSourceBuildOutputDirs: createBuildOutputDir
	@-$(if $(strip $(CXX_SOURCE_FILE_LIST_DIRS)),$(ECHO) Creating build output subdirs in:  $(BUILD_OUTPUT_DIR),)
	@-$(if $(strip $(CXX_SOURCE_FILE_LIST_DIRS)),$(MKDIR) $(CXX_SOURCE_FILE_LIST_DIRS),)


copyAltiaUserMain:
	@-$(if $(call func_file_exists,$(SRC1BASE).c),$(call func_copy_altia_user_main_user_supplied),$(call func_copy_altia_user_main_default))
	@-$(call func_make_file_writeable,altiaUserMain.c)


program_deepscreen: createBuildOutputDir cleanProgram $(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX) library_deepscreen  
	$(BUILD_CMD_PROGRAM_DEEPSCREEN)
	@-$(ECHO) Done  --  $(@)


program_deepscreen_1_lib: createBuildOutputDir cleanProgram $(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX) library  
	$(BUILD_CMD_PROGRAM_DEEPSCREEN)
	@-$(ECHO) Done  --  $(@)


program: createBuildOutputDir cleanProgram $(ASM_SOURCE_OBJECT_LIST) $(C_SOURCE_OBJECT_LIST) $(CXX_SOURCE_OBJECT_LIST) 
	$(BUILD_CMD_PROGRAM)
	@-$(ECHO) Done  --  $(@)


library_deepscreen: $(BUILD_OUTPUT_LIB_NAME_DS_API) $(BUILD_OUTPUT_LIB_NAME_DS_WIN)
	@-$(ECHO) Done  --  $(@)


$(BUILD_OUTPUT_LIB_NAME_DS_API): createBuildOutputDir cleanLibrary_ds_api $(C_SOURCE_OBJECT_LIST_DS_API)
	$(BUILD_CMD_LIBRARY_DS_API)
	@-$(ECHO) Done  --  $(@)


$(BUILD_OUTPUT_LIB_NAME_DS_WIN): createBuildOutputDir cleanLibrary_ds_win $(ASM_SOURCE_OBJECT_LIST) $(C_SOURCE_OBJECT_LIST) $(CXX_SOURCE_OBJECT_LIST)
	$(BUILD_CMD_LIBRARY_DS_WIN)
	@-$(ECHO) Done  --  $(@)


library: createBuildOutputDir cleanLibrary $(ASM_SOURCE_OBJECT_LIST) $(C_SOURCE_OBJECT_LIST) $(CXX_SOURCE_OBJECT_LIST)
	$(BUILD_CMD_LIBRARY)
	@-$(ECHO) Done  --  $(@)


$(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX): copyAltiaUserMain altiaUserMain$(C_SUFFIX) 
	$(BUILD_CMD_C_SOURCE_FILE_ALTIA_USER_MAIN)


$(ASM_SOURCE_OBJECT_LIST): $(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX): %$(ASM_SUFFIX) | createAsmSourceBuildOutputDirs
	$(BUILD_CMD_ASM_SOURCE_FILE)


$(C_SOURCE_OBJECT_LIST): $(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX): %$(C_SUFFIX) $(C_HEADER_FILE_LIST) | createCSourceBuildOutputDirs
	$(BUILD_CMD_C_SOURCE_FILE)


$(C_SOURCE_OBJECT_LIST_DS_API): $(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX): %$(C_SUFFIX) $(C_HEADER_FILE_LIST) | createCSourceBuildOutputDirs
	$(BUILD_CMD_C_SOURCE_FILE)


$(CXX_SOURCE_OBJECT_LIST): $(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX): %$(CXX_SUFFIX) $(CXX_HEADER_FILE_LIST) | createCxxSourceBuildOutputDirs
	$(BUILD_CMD_CXX_SOURCE_FILE)


cleanProgram: | createBuildOutputDir
	@-$(ECHO) Cleaning program in:  $(BUILD_OUTPUT_DIR)
	@-$(RM) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_PROGRAM_NAME)"),"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_PROGRAM_NAME)")


cleanLibrary_ds_api: | createBuildOutputDir
	@-$(ECHO) Cleaning DeepScreen API library in:  $(BUILD_OUTPUT_DIR)
	@-$(RM) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_API)"),"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_API)")


cleanLibrary_ds_win: | createBuildOutputDir
	@-$(ECHO) Cleaning DeepScreen library in:  $(BUILD_OUTPUT_DIR)
	@-$(RM) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_WIN)"),"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_WIN)")


cleanLibrary: | createBuildOutputDir
	@-$(ECHO) Cleaning library in:  $(BUILD_OUTPUT_DIR)
	@-$(RM) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME)"),"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME)")


cleanObjects: | createBuildOutputDir
	@-$(ECHO) Cleaning object files in:  $(BUILD_OUTPUT_DIR)
	@-$(RM) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX)),$(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX)) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(ASM_SOURCE_OBJECT_LIST)),$(ASM_SOURCE_OBJECT_LIST)) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(C_SOURCE_OBJECT_LIST)),$(C_SOURCE_OBJECT_LIST)) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(C_SOURCE_OBJECT_LIST_DS_API)),$(C_SOURCE_OBJECT_LIST_DS_API)) \
		$(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(CXX_SOURCE_OBJECT_LIST)),$(CXX_SOURCE_OBJECT_LIST))


clean: createBuildOutputDir cleanProgram cleanLibrary cleanLibrary_ds_api cleanLibrary_ds_win cleanObjects
	@-$(ECHO) Done  --  $(@)


createDocsDir:
	@-$(if $(call func_file_exists,$(BUILD_DOCS_DIR)),,$(ECHO) Creating docs dir:  $(BUILD_DOCS_DIR))
	@-$(if $(call func_file_exists,$(BUILD_DOCS_DIR)),,$(MKDIR) $(if $(DOS_PATH_SEP_CHAR_REQD),$(call func_sub_unix_2_dos_chars,$(BUILD_DOCS_DIR)),$(BUILD_DOCS_DIR)))


docs: | createDocsDir
	$(DOXYGEN)


printBuildConfig:
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ================================================================================
	@-$(ECHO) Build Configuration
	@-$(ECHO) ================================================================================
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) BUILD_OUTPUT_DIR:    $(BUILD_OUTPUT_DIR)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ----------------
	@-$(ECHO) Tool chain info:
	@-$(ECHO) ----------------
	@-$(ECHO) TOOLCHAIN_HOST_OS:   $(strip $(TOOLCHAIN_HOST_OS))
	@-$(ECHO) TOOLCHAIN_PATH:      $(strip $(TOOLCHAIN_PATH))
	@-$(ECHO) TOOLCHAIN_EXE_PATH:  $(strip $(TOOLCHAIN_EXE_PATH))
	@-$(ECHO) TOOLCHAIN_INC_PATH:  $(strip $(TOOLCHAIN_INC_PATH))
	@-$(ECHO) TOOLCHAIN_LIB_PATH:  $(strip $(TOOLCHAIN_LIB_PATH))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) -------------------------------------------------
	@-$(ECHO) Tool chain binaries and associated build options:
	@-$(ECHO) -------------------------------------------------
	@-$(ECHO) ASM:                      $(strip $(ASM))
	@-$(ECHO) ASMFLAGS:                 $(strip $(ASMFLAGS))
	@-$(ECHO) ASMDEFINES:               $(strip $(ASMDEFINES))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) CC:                       $(strip $(CC))
	@-$(ECHO) CFLAGS:                   $(strip $(CFLAGS))
	@-$(ECHO) C_INCLUDE_PATH_LIST:      $(strip $(C_INCLUDE_PATH_LIST))
	@-$(ECHO) CDEFINES:                 $(strip $(CDEFINES))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) CXX:                      $(strip $(CXX))
	@-$(ECHO) CXXFLAGS:                 $(strip $(CXXFLAGS))
	@-$(ECHO) CXX_INCLUDE_PATH_LIST:    $(strip $(CXX_INCLUDE_PATH_LIST))
	@-$(ECHO) CXXDEFINES:               $(strip $(CXXDEFINES))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) AR:                       $(strip $(AR))
	@-$(ECHO) ARFLAGS:                  $(strip $(ARFLAGS))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) LD:                       $(strip $(LD))
	@-$(ECHO) LDFLAGS:                  $(strip $(LDFLAGS))
	@-$(ECHO) LD_PROGRAM_LIBS_INCLUDE_PATH_LIST:  $(strip $(LD_PROGRAM_LIBS_INCLUDE_PATH_LIST))
	@-$(ECHO) LD_PROGRAM_LIBS_LIST:     $(strip $(LD_PROGRAM_LIBS_LIST))
	@-$(ECHO) LDDEFINES:                $(strip $(LDDEFINES))
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ---------------------------------------------
	@-$(ECHO) Assembly source list ie ASM_SOURCE_FILE_LIST:
	@-$(ECHO) ---------------------------------------------
	@-$(ECHO) $(if $(strip $(ASM_SOURCE_FILE_LIST)),$(ASM_SOURCE_FILE_LIST),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ------------------------------------
	@-$(ECHO) C source list ie C_SOURCE_FILE_LIST:
	@-$(ECHO) ------------------------------------
	@-$(ECHO) $(if $(strip $(C_SOURCE_FILE_LIST)),$(C_SOURCE_FILE_LIST),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) C DS API source list ie C_SOURCE_FILE_LIST_DS_API:
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) $(if $(strip $(C_SOURCE_FILE_LIST_DS_API)),$(C_SOURCE_FILE_LIST_DS_API),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ------------------------------------
	@-$(ECHO) C header list ie C_HEADER_FILE_LIST:
	@-$(ECHO) ------------------------------------
	@-$(ECHO) $(if $(strip $(C_HEADER_FILE_LIST)),$(C_HEADER_FILE_LIST),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ----------------------------------------
	@-$(ECHO) C++ source list ie CXX_SOURCE_FILE_LIST:
	@-$(ECHO) ----------------------------------------
	@-$(ECHO) $(if $(strip $(CXX_SOURCE_FILE_LIST)),$(CXX_SOURCE_FILE_LIST),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ----------------------------------------
	@-$(ECHO) C++ header list ie CXX_HEADER_FILE_LIST:
	@-$(ECHO) ----------------------------------------
	@-$(ECHO) $(if $(strip $(CXX_HEADER_FILE_LIST)),$(CXX_HEADER_FILE_LIST),n/a)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ----------------------------------------------------
	@-$(ECHO) Assembler command line ie BUILD_CMD_ASM_SOURCE_FILE:
	@-$(ECHO) ----------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_ASM_SOURCE_FILE))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ---------------------------------------------------
	@-$(ECHO) C compiler command line ie BUILD_CMD_C_SOURCE_FILE:
	@-$(ECHO) ---------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_C_SOURCE_FILE))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) -------------------------------------------------------------------
	@-$(ECHO) C compiler command line ie BUILD_CMD_C_SOURCE_FILE_ALTIA_USER_MAIN:
	@-$(ECHO) -------------------------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_C_SOURCE_FILE_ALTIA_USER_MAIN))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) -------------------------------------------------------
	@-$(ECHO) C++ compiler command line ie BUILD_CMD_CXX_SOURCE_FILE:
	@-$(ECHO) -------------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_CXX_SOURCE_FILE))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) -------------------------------------------
	@-$(ECHO) Archiver command line ie BUILD_CMD_LIBRARY:
	@-$(ECHO) -------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_LIBRARY))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) Archiver command line ie BUILD_CMD_LIBRARY_DS_API:
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_LIBRARY_DS_API))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) Archiver command line ie BUILD_CMD_LIBRARY_DS_WIN:
	@-$(ECHO) --------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_LIBRARY_DS_WIN))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) -----------------------------------------
	@-$(ECHO) Linker command line ie BUILD_CMD_PROGRAM:
	@-$(ECHO) -----------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_PROGRAM))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ----------------------------------------------------
	@-$(ECHO) Linker command line ie BUILD_CMD_PROGRAM_DEEPSCREEN:
	@-$(ECHO) ----------------------------------------------------
	@-$(ECHO) '$(strip $(value BUILD_CMD_PROGRAM_DEEPSCREEN))'
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO_BLANK_LINE)
	@-$(ECHO) ================================================================================

endif  # ifndef _RULES_DEEPSCREEN_MK_

