#include "../kernel/low_level.h"
#include "keyboard.h"
#include "screen.h"

// Read 32-bit value from PCI configuration space
uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1 << 31)             // Enable bit
                     | (bus << 16)           // Bus number
                     | (device << 11)        // Device number
                     | (function << 8)       // Function number
                     | (offset & 0xFC);      // Register offset (must be aligned to 4 bytes)

    outl(PCI_CONFIG_ADDRESS, address);  // Write the address to the PCI config address port
    return inl(PCI_CONFIG_DATA);        // Read from the PCI config data port
}

void pci_config_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

uint32_t get_bar_address() {
    // Enumerate PCI to find xHCI controller
    for (uint8_t bus = 0; bus < 255; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                uint32_t vendor_device_id = pci_config_read(bus, device, function, 0x00);
                uint16_t vendor_id = vendor_device_id & 0xFFFF;
                if (vendor_id == 0xFFFF) continue; // No device present
                
                uint32_t class_code_reg = pci_config_read(bus, device, function, 0x08);
                uint8_t base_class = (class_code_reg >> 24) & 0xFF;
                uint8_t sub_class = (class_code_reg >> 16) & 0xFF;
                uint8_t prog_if = (class_code_reg >> 8) & 0xFF;

                if (base_class == 0x0C && sub_class == 0x03 && prog_if == 0x30) {
                    print_string("Found xHCI controller\n");
                    uint32_t bar = pci_config_read(bus, device, function, 0x10);
                    
                    if (bar & 0x01) {
                        print_string("Found I/O port (not memory-mapped)\n");
                        continue;  // I/O port, not memory-mapped
                    }
                    return bar & ~0xF;  // Mask to get memory address
                }
            }
        }
    }
    return 0;  // No xHCI controller found
}

void init_xhci(uint32_t bar_address) {
    print_string("Initializing xHCI reset\n");

    // Cap Length Register
    uint8_t *cap_length_reg = (uint8_t *)bar_address;
    print_address_info("Cap Length Reg", (uint32_t)(*cap_length_reg));

    // find USB Command Register
    uint32_t *usb_command_reg = (uint32_t *)(bar_address + *cap_length_reg);
    print_address_info("USB Command Reg", (uint32_t)usb_command_reg);


    //reset controller
    *usb_command_reg |= 0b10;
    while (*usb_command_reg & 0b10) {
        print_string("Waiting for reset...");
    }
    
    // Now proceed with setting MaxSlotsEn
    print_string("Setting MaxSlotsEn...\n");
    *(usb_command_reg + 0b1110) |= 0b1000;
    print_string("MaxSlotsEn set to ");
    print_hex(*(usb_command_reg + 0b1110));
    print_string("\n");

    // Set DCBAAP (Device Context Base Address Array Pointer)
    print_string("Setting DCBAAP...\n");
    print_hex(*(usb_command_reg + 0b1100));
    print_string("\n");
    uint32_t *dcbaa = (uint32_t *)0x10000;
    *(usb_command_reg + 0b1100) = (uint32_t)dcbaa;
    print_string("DCBAAP set to: ");
    print_hex(*(usb_command_reg + 0b1100));
    print_string("\n");

    // Set (CRCR) Command Ring Control Register
    print_string("Setting CRCR...\n");
    print_hex(*(usb_command_reg + 0b110));
    print_string("\n");
    uint32_t *CRCR = (uint32_t *)0x20000;
    *(usb_command_reg + 0b110) = (uint32_t)CRCR;
    print_string("CRCR set to: ");
    print_hex(*(usb_command_reg + 0b110));
    print_string("\n");
}



