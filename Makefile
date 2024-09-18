# Makefile for building a simple OS image

# Variables
NASM = nasm
GCC = gcc
LD = ld
DD = dd
RM = rm -f
MKDIR = mkdir -p

# Paths
BOOT_DIR = boot
KERNEL_DIR = kernel
OUTPUT_DIR = output

# Files
BOOT_SECT_ASM = $(BOOT_DIR)/boot_sect.asm
BOOT_SECT_IMG = $(BOOT_DIR)/boot_sect.img
KERNEL_ENTRY_ASM = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_ENTRY_O = $(KERNEL_DIR)/kernel_entry.o
KERNEL_C = $(KERNEL_DIR)/kernel.c
KERNEL_O = $(KERNEL_DIR)/kernel.o
LOW_LEVEL_C = $(KERNEL_DIR)/low_level.c
LOW_LEVEL_O = $(KERNEL_DIR)/low_level.o
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin
OS_IMG = $(OUTPUT_DIR)/os.img

# Targets and rules
all: $(OS_IMG)

$(BOOT_SECT_IMG): $(BOOT_SECT_ASM)
	$(NASM) $< -f bin -o $@ -O0

$(KERNEL_ENTRY_O): $(KERNEL_ENTRY_ASM)
	$(NASM) $< -f elf32 -o $@ -O0

$(KERNEL_O): $(KERNEL_C)
	$(GCC) -m32 -ffreestanding -fno-pic -c $< -o $@ -g0

$(LOW_LEVEL_O): $(LOW_LEVEL_C)
	$(GCC) -m32 -ffreestanding -fno-pic -c $< -o $@ -g0

$(KERNEL_BIN): $(KERNEL_ENTRY_O) $(KERNEL_O) $(LOW_LEVEL_O)
	$(LD) -m elf_i386 -T linker.ld -o $@ $(KERNEL_ENTRY_O) $(KERNEL_O) $(LOW_LEVEL_O) --oformat binary

$(OS_IMG): $(BOOT_SECT_IMG) $(KERNEL_BIN)
	$(MKDIR) $(OUTPUT_DIR)
	$(DD) if=/dev/zero of=$@ bs=512 count=128
	$(DD) if=$(BOOT_SECT_IMG) of=$@ bs=512 count=1 conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

clean:
	$(RM) $(BOOT_SECT_IMG) $(KERNEL_ENTRY_O) $(KERNEL_O) $(LOW_LEVEL_O) $(KERNEL_BIN) $(OS_IMG)

.PHONY: all clean
