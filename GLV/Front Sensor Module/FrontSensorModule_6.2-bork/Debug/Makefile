################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../Source/ \
../Source/include/ \
../Source/portable/ \
../Source/portable/GCC/ \
../Source/portable/GCC/ATMega64/ \
../Source/portable/MemMang/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../main.c \
../node_tasks.c \
../Source/list.c \
../Source/portable/GCC/ATMega64/port.c \
../Source/portable/MemMang/heap_1.c \
../Source/queue.c \
../Source/tasks.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
main.o \
node_tasks.o \
Source/list.o \
Source/portable/GCC/ATMega64/port.o \
Source/portable/MemMang/heap_1.o \
Source/queue.o \
Source/tasks.o

OBJS_AS_ARGS +=  \
main.o \
node_tasks.o \
Source/list.o \
Source/portable/GCC/ATMega64/port.o \
Source/portable/MemMang/heap_1.o \
Source/queue.o \
Source/tasks.o

C_DEPS +=  \
main.d \
node_tasks.d \
Source/list.d \
Source/portable/GCC/ATMega64/port.d \
Source/portable/MemMang/heap_1.d \
Source/queue.d \
Source/tasks.d

C_DEPS_AS_ARGS +=  \
main.d \
node_tasks.d \
Source/list.d \
Source/portable/GCC/ATMega64/port.d \
Source/portable/MemMang/heap_1.d \
Source/queue.d \
Source/tasks.d

OUTPUT_FILE_PATH +=FrontSensorModule.elf

OUTPUT_FILE_PATH_AS_ARGS +=FrontSensorModule.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"../Source" -I"../Source/include" -I"../Source/portable/GCC/ATMega64" -I".." -I"../Source/portable/MemMang"  -O1 -fpack-struct -fshort-enums -g3 -Wall -Wextra -mmcu=atmega64c1 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

Source/%.o: ../Source/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"../Source" -I"../Source/include" -I"../Source/portable/GCC/ATMega64" -I".." -I"../Source/portable/MemMang"  -O1 -fpack-struct -fshort-enums -g3 -Wall -Wextra -mmcu=atmega64c1 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

Source/portable/GCC/ATMega64/%.o: ../Source/portable/GCC/ATMega64/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"../Source" -I"../Source/include" -I"../Source/portable/GCC/ATMega64" -I".." -I"../Source/portable/MemMang"  -O1 -fpack-struct -fshort-enums -g3 -Wall -Wextra -mmcu=atmega64c1 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

Source/portable/MemMang/%.o: ../Source/portable/MemMang/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"../Source" -I"../Source/include" -I"../Source/portable/GCC/ATMega64" -I".." -I"../Source/portable/MemMang"  -O1 -fpack-struct -fshort-enums -g3 -Wall -Wextra -mmcu=atmega64c1 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="FrontSensorModule.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -mmcu=atmega64c1  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "FrontSensorModule.elf" "FrontSensorModule.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "FrontSensorModule.elf" "FrontSensorModule.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "FrontSensorModule.elf" > "FrontSensorModule.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "FrontSensorModule.elf" "FrontSensorModule.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-size.exe" "FrontSensorModule.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "FrontSensorModule.elf" "FrontSensorModule.a" "FrontSensorModule.hex" "FrontSensorModule.lss" "FrontSensorModule.eep" "FrontSensorModule.map" "FrontSensorModule.srec" "FrontSensorModule.usersignatures"
	