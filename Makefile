# Makefile for building a simple OS image

# Variables
NASM = nasm
GCC = gcc
LD = ld
DD = dd
RM = rm -f
MKDIR = mkdir -p
MKFS = mkfs.fat

# Paths
BOOT_DIR = boot
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
OUTPUT_DIR = output

# Files
MBR_ASM = $(BOOT_DIR)/mbr.asm
MBR_IMG = $(BOOT_DIR)/mbr.img
KERNEL_ENTRY_ASM = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_ENTRY_O = $(KERNEL_DIR)/kernel_entry.o
KERNEL_FILES_C = ${wildcard $(KERNEL_DIR)/*.c} ${wildcard $(DRIVERS_DIR)/*.c}
KERNEL_FILES_O = $(KERNEL_FILES_C:c=o)
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin
OS_IMG = $(OUTPUT_DIR)/os.img
FAT32_IMG = $(OUTPUT_DIR)/fat32.img

# Targets and rules
all: $(OS_IMG)

$(MBR_IMG): $(MBR_ASM)
	$(NASM) $< -f bin -o $@ -O0

$(KERNEL_ENTRY_O): $(KERNEL_ENTRY_ASM)
	$(NASM) $< -f elf32 -o $@ -O0

$(KERNEL_FILES_O): %.o: %.c
	$(GCC) -m32 -ffreestanding -fno-pic -c $< -o $@ -g0

$(KERNEL_BIN): $(KERNEL_ENTRY_O) $(KERNEL_FILES_O)
	$(LD) -m elf_i386 -T linker.ld -o $@ $(KERNEL_FILES_O) $(KERNEL_ENTRY_O) --oformat binary

# Create a FAT32 file system image
$(FAT32_IMG): 
	$(MKDIR) $(OUTPUT_DIR)
	# Create an empty image file of the size you want, here 50 MB (adjust as needed)
	dd if=/dev/zero of=$@ bs=1M count=50
	# Create a FAT32 file system on the image
	$(MKFS) -F 32 $@

$(OS_IMG): $(MBR_IMG) $(KERNEL_BIN) $(FAT32_IMG)
	$(MKDIR) $(OUTPUT_DIR)
	# Create the OS image with an initial 512-byte boot sector (MBR)
	$(DD) if=/dev/zero of=$@ bs=512 count=512
	$(DD) if=$(MBR_IMG) of=$@ bs=512 count=1 conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc
	# Write the FAT32 image starting at sector 2048 (i.e., offset 2048 * 512 bytes)
	$(DD) if=$(FAT32_IMG) of=$@ bs=512 seek=2048 conv=notrunc

clean:
	$(RM) $(MBR_IMG) $(KERNEL_ENTRY_O) $(KERNEL_FILES_O) $(KERNEL_BIN) $(OS_IMG)

.PHONY: all clean
