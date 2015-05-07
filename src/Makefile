##############################################################################################
#	makefile
#	 by Juan Yescas
#
#	A makefile script for generation of BearOs raspberry pi kernel images.
##############################################################################################

# The toolchain to use. arm-none-eabi works, but there does exist 
# arm-bcm2708-linux-gnueabi.
ARMGNU	    = arm-none-eabi

# The intermediate directory for compiled object files.
BUILD	    = build/

# The directory that contains the device C files.
DEVICES	    = devices/

# The directory that contains the miscellanea C files.
MISC	    = misc/

# The directory that contains the threads C files.
THREADS	    = threads/

# The directory that contains the C libraries.
LIB	    = lib/

# The directory that contains the C kernel libraries.
LIB_KERNEL  = lib/kernel/

##############################################################################################
# GCC library
# GCC provides a low-level runtime library, libgcc.a or libgcc_s.so.1 on some platforms.
# GCC generates calls to routines in this library automatically, whenever it needs to perform some
# operation that is too complicated to emit inline code for.
#
# Most of the routines in libgcc handle arithmetic operations that the target processor cannot
# perform directly. This includes integer multiply and divide on some machines, and all
# floating-point and fixed-point operations on other machines. libgcc also includes routines for
# exception handling, and a handful of miscellaneous operations. 
#
# https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
#
# https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html#Integer-library-routines
#
##############################################################################################
LIB_GCC	    = libgcc/libgcc.a

# The directory in which source files are stored.
ASM_SOURCE  = arm_asm/

# SD card
SD_CARD	    = /Volumes/RECOVERY

# The name of the output file to generate.
TARGET	    = kernel.img

# The name of the assembler listing file to generate.
LIST	    = kernel.list

# The name of the map file to generate.
MAP	    = kernel.map

# The name of the linker script to use.
LINKER	    = kernel.ld

# C FLAGS
# -nostdinc		No include the standard libraries.
# -I$(LIB)		Include the standard libraries.
CFLAGS	    = -nostdinc
CFLAGS	    += -mcpu=arm1176jzf-s
CFLAGS	    += -Wall

# Include directories
INCL	    = -I$(DEVICES) -I$(LIB) -I$(LIB_KERNEL) -I$(THREADS) -I$(MISC)

# The names of all object files that must be generated. Deduced from the 
# assembly code files in source.
ASM_SRC	    = $(wildcard $(ASM_SOURCE)*.s)
OBJECTS	    := $(patsubst $(ASM_SOURCE)%.s, $(BUILD)%.o, $(ASM_SRC))

# Rule to make everything.
all: $(TARGET) $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all

# Rule to copy the image onto the flash drive.
install : rebuild
	cp $(TARGET) $(SD_CARD)
	#umount $(SD_CARD)

# Rule to make the listing file.
$(LIST) : $(BUILD)output.elf
	@echo
	@echo "=============== Dumping ELF to $@ ==============="
	$(ARMGNU)-objdump -d $(BUILD)output.elf > $(LIST)

# Rule to make the image file.
$(TARGET) : $(BUILD)output.elf
	@echo
	@echo "=============== Generating Kernel Image $@ ==============="
	$(ARMGNU)-objcopy $(BUILD)output.elf -O binary $(TARGET) 

# C Objects that have to be compiled.
DEVICES_SRC = $(wildcard $(DEVICES)*.c)
DEVICES_OBJ = $(patsubst $(DEVICES)%.c, $(BUILD)%.o, $(DEVICES_SRC))
LIB_SRC	    = $(wildcard $(LIB)*.c)
LIB_OBJ	    = $(patsubst $(LIB)%.c, $(BUILD)%.o, $(LIB_SRC))
KERNEL_SRC  = $(wildcard $(LIB_KERNEL)*.c)
KERNEL_OBJ  = $(patsubst $(LIB_KERNEL)%.c, $(BUILD)%.o, $(KERNEL_SRC))
THREADS_SRC = $(wildcard $(THREADS)*.c)
THREADS_OBJ = $(patsubst $(THREADS)%.c, $(BUILD)%.o, $(THREADS_SRC))
MISC_SRC    = $(wildcard $(MISC)*.c)
MISC_OBJ    = $(patsubst $(MISC)%.c, $(BUILD)%.o, $(MISC_SRC))

C_OBJECTS   = $(DEVICES_OBJ) \
	      $(LIB_OBJ) \
	      $(KERNEL_OBJ) \
	      $(THREADS_OBJ)

# Rule to make the elf file.
$(BUILD)output.elf : $(BUILD) $(OBJECTS) $(C_OBJECTS) $(LINKER)
	@echo
	@echo "=============== Generating $@ ==============="
	$(ARMGNU)-ld --no-undefined $(OBJECTS) $(C_OBJECTS) \
	-Map $(MAP) -o $(BUILD)output.elf -T $(LINKER) \
	 $(LIB_GCC)
# 	-verbose 

# Rule to make the object files.
$(OBJECTS): $(BUILD)%.o : $(ASM_SOURCE)%.s
	@echo
	@echo "=============== Assembling $@ ==============="
	$(ARMGNU)-as -I $(ASM_SOURCE) $< -o $@
	
$(DEVICES_OBJ) : $(BUILD)%.o : $(DEVICES)%.c
	@echo
	@echo "=============== Generating $@ ==============="
	$(ARMGNU)-gcc $(INCL) $(CFLAGS) -c $< -o $@
	
$(LIB_OBJ) : $(BUILD)%.o : $(LIB)%.c
	@echo
	@echo "=============== Generating $@ ==============="
	$(ARMGNU)-gcc $(INCL) $(CFLAGS) -c $< -o $@
	
$(KERNEL_OBJ) : $(BUILD)%.o : $(LIB_KERNEL)%.c
	@echo
	@echo "=============== Generating $@ ==============="
	$(ARMGNU)-gcc $(INCL) $(CFLAGS) -c $< -o $@
	
$(THREADS_OBJ) : $(BUILD)%.o : $(THREADS)%.c
	@echo
	@echo "=============== Generating $@ ==============="
	$(ARMGNU)-gcc $(INCL) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir $@

# Rule to clean files.
clean : 
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
