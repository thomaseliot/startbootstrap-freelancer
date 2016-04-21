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

ifndef _TOOLCHAIN_IAR_7_DEEPSCREEN_MK_
_TOOLCHAIN_IAR_7_DEEPSCREEN_MK_ := 1


################################################################################
# Tool chain basic config
################################################################################
TOOLCHAIN_HOST_OS := win32
ifneq ($(strip $(BUILD_TOOLS_HOST_OS)),$(strip $(TOOLCHAIN_HOST_OS)))
$(info ================================================================================)
$(info Configuration error:     BUILD_TOOLS_HOST_OS <$(BUILD_TOOLS_HOST_OS)> and TOOLCHAIN_HOST_OS <$(TOOLCHAIN_HOST_OS)> do not match!)
$(info ----                     TOOLCHAIN_MAKEFILE <$(TOOLCHAIN_MAKEFILE)> only supports the following host OS(es):  $(TOOLCHAIN_HOST_OS))
$(info ================================================================================)
$(error Exiting build...)
endif  # ifneq ($(strip $(BUILD_TOOLS_HOST_OS)),$(strip $(TOOLCHAIN_HOST_OS)))


################################################################################
# Paths
################################################################################
ifeq ($(strip $(TOOLCHAIN_BASE_PATH)),)
TOOLCHAIN_BASE_PATH := C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.0\arm
$(info -    TOOLCHAIN_BASE_PATH:    $(TOOLCHAIN_BASE_PATH)  (default))

else
$(info -    TOOLCHAIN_BASE_PATH:    $(TOOLCHAIN_BASE_PATH))

endif  # ifeq ($(strip $(TOOLCHAIN_BASE_PATH)),)


$(info ================================================================================)


TOOLCHAIN_PATH		:= $(TOOLCHAIN_BASE_PATH)
TOOLCHAIN_EXE_PATH	:= $(TOOLCHAIN_PATH)/bin
TOOLCHAIN_INC_PATH	:= $(TOOLCHAIN_PATH)/inc
TOOLCHAIN_LIB_PATH	:= $(TOOLCHAIN_PATH)/lib


################################################################################
# Platform tool chain and build output configuration
################################################################################
#
# Tool chain binaries
#
ASM	:= "$(TOOLCHAIN_EXE_PATH)/iasmarm.exe"
CC	:= "$(TOOLCHAIN_EXE_PATH)/iccarm.exe"
CXX	:= "$(TOOLCHAIN_EXE_PATH)/iccarm.exe"
AR	:= "$(TOOLCHAIN_EXE_PATH)/iarchive.exe"
LD	:= "$(TOOLCHAIN_EXE_PATH)/ilinkarm.exe"


#
# Tool chain expected values for various build artifacts
# Typical values include:
#
#	ASM_SUFFIX	:= [.s | .src]
#	OBJ_SUFFIX	:= [.o | .obj]
#	LIB_PREFIX	:= [lib | <nothing at all>]
#	LIB_SUFFIX	:= [.a | .lib]
#	DLL_SUFFIX	:= [.so | .dll]
#	EXE_SUFFIX	:= [.exe | .axf | .out | .mot | .hex | <nothing at all>]
#
ASM_SUFFIX	:= .s
C_SUFFIX	:= .c
CXX_SUFFIX	:= .cpp
OBJ_SUFFIX	:= .o
LIB_PREFIX	:= lib
LIB_SUFFIX	:= .a
DLL_SUFFIX	:=
EXE_SUFFIX	:= .out


################################################################################
#-------------------
# Assembler options
#-------------------
#
#	--cpu								Generate code for the specified ARM processor 
#										variant
#	--endian little						Force little endian mode  (should be on 
#										by default)
#	--fpu								Select the type of floating-point unit
#	-S									Quiet mode / disable assembler version info
#	-s									Case sensitive user symbols
#	-w									Disable some warnings
#		25								Warning[25]: Label 'XXX' is defined pubweak in a section implicitly declared root
#
#-----------
# Reference
#-----------
# These came from Segger IAR EA LPC1788 demo board BSP and IAR assembler documentation.
#
################################################################################
#
# Debug vs release build options
#
ifeq ($(strip $(BUILD_TYPE)),debug)
ASMDEBUG += -r
ASMDEFINES += DEBUG
else
ASMDEBUG +=
endif  # ifeq ($(strip $(BUILD_TYPE)),debug)


ASMDEFINES += \
	TOOLCHAIN_IAR


ASMFLAGS += \
	-S \
	-s \
	-w-25 \
	$(ASMDEBUG)


################################################################################
#------------------
# Compiler options
#------------------
#	--cpu								Generate code for the specified ARM processor 
#										variant
#	--thumb								Set the cpu mode to use Thumb instructions
#	--interwork							Generate interworking code to enable inter-compatibility
#										b/w ARM / Thumb compiled code
#	--endian=little						Force little endian mode  (should be on 
#										by default)
#	--fpu								Select the type of floating-point unit
#	--relaxed_fp						Relax rules for floating-point expression
#										optimization
#	--use_c++_inline					Use C++ inline semantics in C99
#	--macro_positions_in_diagnostics	Obtain positions inside macros in diagnostic 
#										messages
#	-e									Enable language extensions
#	-l									Create a list file  (add if needed)
#										"cN" options:  C source file, no diagnostics
#										Example:  -lcN $(ALTIA_LIST_FILE_PATH)
#	--error_limit						Specify the allowed number of errors before 
#										compilation stops
#	--silent							Turn off compiler logo, stats reporting 
#	--no_unaligned_access				Make sure code, structs, etc. are aligned 
#										on 4 byte boundaries.  Ensures faster execution.
#	--char_is_signed					Default "char" type to be signed
#	--no_tbaa							Disable type based alias analysis
#	--diag_suppress						Disable some warnings that are invalid for 
#										the generated target code
#		Pa050							non-native end of line sequences detected
#		Pa082							order of volatile accesses is undefined in this statement
#		Pa130							value is not used in expanded macro
#		Pe068							integer conversion resulted in a change of sign
#		Pe111							unreachable statement
#		Pe177							declared, but unreferenced variable or function
#		Pe188							enumerated type mixed with another type
#		Pe191							type qualifier is meaningless on cast type
#		Pe223							function "XXX" declared implicitly
#		Pe301							typedef name has already been declared (with same type)
#		Pe550							variable set, but never used
#
# Build type related options:
#	--debug								Include related debugging info
#	-Ol									Optimization level:  low
#	-Ohs								Optimization level:  high, favoring speed
#
#-----------
# Reference
#-----------
# These came from several sources:  list file output from the original Vybrid IAR 
# port, STM32 miniGL port, and IAR compiler documentation.
#
################################################################################
#
# Debug vs release build options
#
ifeq ($(strip $(BUILD_TYPE)),debug)
CDEBUG		+= --debug -Ol
CXXDEBUG	+= $(CDEBUG)
else
CDEBUG		+= -Ohs
CXXDEBUG	+= $(CDEBUG)
endif  # ifeq ($(strip $(BUILD_TYPE)),debug)


CDEFINES := \
	TOOLCHAIN_IAR \
	$(CDEFINES)


C_INCLUDE_PATH_LIST +=


CFLAGS += \
	--use_c++_inline \
	--macro_positions_in_diagnostics \
	-e \
	--no_unaligned_access \
	--char_is_signed \
	--no_tbaa \
	--error_limit=100 \
	--silent \
	--diag_suppress=Pa050,Pa082,Pa130,Pe068,Pe111,Pe177,Pe188,Pe191,Pe223,Pe301,Pe550 \
	$(CDEBUG)


CXXDEFINES += \
	$(CDEFINES)


CXXFLAGS += \
	--use_c++_inline \
	--macro_positions_in_diagnostics \
	-e \
	--no_unaligned_access \
	--char_is_signed \
	--no_tbaa \
	--error_limit=100 \
	--silent \
	--diag_suppress=Pa050,Pa082,Pa130,Pe068,Pe111,Pe177,Pe188,Pe191,Pe223,Pe301,Pe550 \
	$(CXXDEBUG)


################################################################################
#-------------------
# Librarian options
#-------------------
#	--create						Create archive
#
#-----------
# Reference
#-----------
# IAR toolchain documentation
#
################################################################################
ARFLAGS += \
	--create


################################################################################
#----------------
# Linker options
#----------------
#	--cpu								Generate code for the specified ARM processor 
#										variant
#	--map								Generate map file for the final executable 
#	--inline							Auto-optimize functions that can fit into 
#										the space of a function call
#	--config							Linker configuration file ie section map
#	--output							Final build artifact / binary / executable 
#
#-----------
# Reference
#-----------
# IAR toolchain documentation
#
################################################################################
#
# Debug vs release build options
#
ifeq ($(strip $(BUILD_TYPE)),debug)
LDDEBUG +=
else
LDDEBUG +=
endif  # ifeq ($(strip $(BUILD_TYPE)),debug)


LDDEFINES +=


ifeq ($(filter-out program_deepscreen library_deepscreen, $(strip $(BUILD_OUTPUT_ARTIFACT))),)
LD_PROGRAM_LIBS_LIST += \
	$(BUILD_OUTPUT_LIB_NAME_DS_API) \
	$(BUILD_OUTPUT_LIB_NAME_DS_WIN)

else
LD_PROGRAM_LIBS_LIST += \
	$(BUILD_OUTPUT_LIB_NAME)

endif  # ifeq ($(filter-out program_deepscreen library_deepscreen, $(strip $(BUILD_OUTPUT_ARTIFACT))),)


LD_PROGRAM_LIBS_LIST += $(EXTRALIBS)


LD_PROGRAM_LIBS_INCLUDE_PATH_LIST += \
	$(if $(strip $(BUILD_OUTPUT_DIR)),$(BUILD_OUTPUT_DIR),.)


LDFLAGS += \
	--map="$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_PROGRAM_NAME).map" \
	--inline \
	$(LDDEBUG)


################################################################################
# Tool chain specifc build commands
#
#--------
# :NOTE:
#--------
# These must be ported per tool chain since there are typically numerous small, 
# but important differences across tool chains.  For instance the build output
# file flag.  This is "-o" for GCC and many other compilers; the MS VS compiler
# uses "-Fo".  Go figure...
#
#-----------
# Reference
#-----------
# For further information:
#	- Refer to gmake documentation wrt define command:
#		- https://www.gnu.org/software/make/manual/make.html#Canned-Recipes
#
################################################################################
define BUILD_CMD_ASM_SOURCE_FILE
	$(ASM) \
	$(ASMFLAGS) \
	$(patsubst %,-D%,$(ASMDEFINES)) \
	$(@:$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX)=%$(ASM_SUFFIX)) \
	-o $(@)
endef


define BUILD_CMD_C_SOURCE_FILE
	$(CC) \
	$(CFLAGS) \
	$(patsubst %,-I"%",$(C_INCLUDE_PATH_LIST)) \
	$(patsubst %,-D%,$(CDEFINES)) \
	$(TARGETDEFS) \
	$(@:$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX)=%$(C_SUFFIX)) \
	-o $(@)
endef


define BUILD_CMD_C_SOURCE_FILE_ALTIA_USER_MAIN
	$(CC) \
	$(CFLAGS) \
	$(patsubst %,-I"%",$(C_INCLUDE_PATH_LIST)) \
	$(patsubst %,-D%,$(CDEFINES)) \
	$(TARGETDEFS) \
	-DDEEPSCREENUSERMAIN \
	$(@:$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX)=%$(C_SUFFIX)) \
	-o $(@)
endef


define BUILD_CMD_CXX_SOURCE_FILE
	$(CXX) \
	$(CXXFLAGS) \
	$(patsubst %,-I"%",$(CXX_INCLUDE_PATH_LIST)) \
	$(patsubst %,-D%,$(CXXDEFINES)) \
	$(TARGETDEFS) \
	$(@:$(BUILD_OUTPUT_DIR)%$(OBJ_SUFFIX)=%$(CXX_SUFFIX)) \
	-o $(@)
endef


define BUILD_CMD_LIBRARY
	$(AR) \
	$(ARFLAGS) \
	"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME)" \
	$(ASM_SOURCE_OBJECT_LIST) \
	$(C_SOURCE_OBJECT_LIST) \
	$(CXX_SOURCE_OBJECT_LIST)
endef


define BUILD_CMD_LIBRARY_DS_WIN
	$(AR) \
	$(ARFLAGS) \
	"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_WIN)" \
	$(ASM_SOURCE_OBJECT_LIST) \
	$(C_SOURCE_OBJECT_LIST) \
	$(CXX_SOURCE_OBJECT_LIST)
endef


define BUILD_CMD_LIBRARY_DS_API
	$(AR) \
	$(ARFLAGS) \
	"$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_LIB_NAME_DS_API)" \
	$(C_SOURCE_OBJECT_LIST_DS_API)
endef


define BUILD_CMD_PROGRAM
	$(LD) \
	$(LDFLAGS) \
	$(LDDEFINES) \
	$(patsubst %,-libpath:"%",$(LD_PROGRAM_LIBS_INCLUDE_PATH_LIST)) \
	--output "$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_PROGRAM_NAME)" \
	$(ASM_SOURCE_OBJECT_LIST) \
	$(C_SOURCE_OBJECT_LIST) \
	$(CXX_SOURCE_OBJECT_LIST) \
	$(LD_PROGRAM_LIBS_LIST)
endef


define BUILD_CMD_PROGRAM_DEEPSCREEN
	$(LD) \
	$(LDFLAGS) \
	$(LDDEFINES) \
	$(patsubst %,-libpath:"%",$(LD_PROGRAM_LIBS_INCLUDE_PATH_LIST)) \
	--output "$(BUILD_OUTPUT_DIR)$(BUILD_OUTPUT_PROGRAM_NAME)" \
	$(BUILD_OUTPUT_DIR)altiaUserMain$(OBJ_SUFFIX) \
	$(LD_PROGRAM_LIBS_LIST)
endef


endif  # ifndef _TOOLCHAIN_IAR_7_DEEPSCREEN_MK_

