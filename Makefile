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
DRIVERS_DIR = drivers
OUTPUT_DIR = output

# Files
BOOT_SECT_ASM = $(BOOT_DIR)/boot_sect.asm
BOOT_SECT_IMG = $(BOOT_DIR)/boot_sect.img
KERNEL_ENTRY_ASM = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_ENTRY_O = $(KERNEL_DIR)/kernel_entry.o
KERNEL_FILES_C = ${wildcard $(KERNEL_DIR)/*.c} ${wildcard $(DRIVERS_DIR)/*.c}
KERNEL_FILES_O = $(KERNEL_FILES_C:c=o)
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin
OS_IMG = $(OUTPUT_DIR)/os.img

# Targets and rules
all: $(OS_IMG)

$(BOOT_SECT_IMG): $(BOOT_SECT_ASM)
	$(NASM) $< -f bin -o $@ -O0

$(KERNEL_ENTRY_O): $(KERNEL_ENTRY_ASM)
	$(NASM) $< -f elf32 -o $@ -O0

$(KERNEL_FILES_O): %.o: %.c
# $(GCC) -m32 -ffreestanding -nostartfiles -nostdlib -nolibc -fno-pic $(KERNEL_FILES_C) -o $@ -g0
	$(GCC) -m32 -ffreestanding -fno-pic -c $< -o $@ -g0

$(KERNEL_BIN): $(KERNEL_ENTRY_O) $(KERNEL_FILES_O)
	$(LD) -m elf_i386 -T linker.ld -o $@ $(KERNEL_FILES_O) $(KERNEL_ENTRY_O) --oformat binary

$(OS_IMG): $(BOOT_SECT_IMG) $(KERNEL_BIN)
	$(MKDIR) $(OUTPUT_DIR)
	$(DD) if=/dev/zero of=$@ bs=512 count=128
	$(DD) if=$(BOOT_SECT_IMG) of=$@ bs=512 count=1 conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

clean:
	$(RM) $(BOOT_SECT_IMG) $(KERNEL_ENTRY_O) $(KERNEL_FILES_O) $(KERNEL_BIN) $(OS_IMG)

.PHONY: all clean
