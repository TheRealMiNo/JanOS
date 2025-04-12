#include <stdint.h>
#include "../kernel/low_level.h"
#include "ide_controller.h"

void read_sector(uint16_t *buffer, uint32_t lba, unsigned int sector_count ) {
    // Select Master drive with LBA mode
    outb(0x1F6, 0xE0);

    // Wait for BSY to clear
    while (inb(0x1F7) & 0x80) {
        print_string("Waiting for BSY to clear...\n");
    }

    // Set sector count and LBA address
    outb(0x1F2, sector_count);
    outb(0x1F3, lba & 0xFF);         // LBA low byte
    outb(0x1F4, (lba >> 8) & 0xFF);  // LBA mid byte
    outb(0x1F5, (lba >> 16) & 0xFF); // LBA high byte

    // Send READ command
    outb(0x1F7, 0x20);

    for (int s = 0; s < sector_count; s++) {
        // Wait for DRQ to set or check for errors
        while (!(inb(0x1F7) & 0x08)) {
            uint8_t status = inb(0x1F7);
            if (status & 0x01) { // ERR bit
                uint8_t error = inb(0x1F1);
                print_string("Error occurred: ");
                print_word(error);
                return;
            }
            if (status & 0x20) { // DF bit
                print_string("Device fault\n");
                return;
            }
        }

        // Read 256 words (512 bytes)
        for (int i = 0; i < 256; i++) {
            buffer[s*256 + i] = inw(0x1F0);
        }
    }
}

uint32_t get_root_directory() {
    uint16_t buffer[256];
    read_sector(buffer, 0x800, 1); // Read the boot sector

    if (buffer[22] == 0x0002) { // Confirm FAT32 signature
        uint16_t BPB_RsvdSecCnt = buffer[0x0E / 2];
        uint8_t BPB_NumFATs = buffer[0x10 / 2] & 0xFF;
        uint32_t BPB_FATSz32 = buffer[0x24 / 2] | (buffer[0x26 / 2] << 16);

        uint32_t root_sector = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz32);
        return root_sector + 0x800; // Add partition offset
    }

    print_string("error: in get_root_directory\n");
    return 0x0000;
}

uint32_t get_fat() {
    uint16_t buffer[256];
    read_sector(buffer, 0x800, 1); // Read the boot sector

    if (buffer[22] == 0x0002) { // Confirm FAT32 signature
        return buffer[7] + 0x800; // Add partition offset
    }

    print_string("error: in get_fat\n");
    return 0x0000;
}


void write_sector(uint16_t *buffer, uint32_t lba) {
    // Select Master drive with LBA mode
    outb(0x1F6, 0xE0);

    // Wait for BSY to clear
    while (inb(0x1F7) & 0x80) {
        print_string("Waiting for BSY to clear...\n");
    }

    // Set sector count and LBA address
    outb(0x1F2, 0x01);              // Sector count: 1
    outb(0x1F3, lba & 0xFF);         // LBA low byte
    outb(0x1F4, (lba >> 8) & 0xFF);  // LBA mid byte
    outb(0x1F5, (lba >> 16) & 0xFF); // LBA high byte

    // Send READ command
    outb(0x1F7, 0x30);

    // Wait for DRQ to set or check for errors
    while (!(inb(0x1F7) & 0x08)) {
        uint8_t status = inb(0x1F7);
        if (status & 0x01) { // ERR bit
            uint8_t error = inb(0x1F1);
            print_string("Error occurred: ");
            print_word(error);
            return;
        }
        if (status & 0x20) { // DF bit
            print_string("Device fault\n");
            return;
        }
    }

    // Write 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, buffer[i]);
    }
}