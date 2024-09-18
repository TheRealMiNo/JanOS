#include "../drivers/screen.c"
#include <stdint.h>

// PCI I/O ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Read 32-bit value from PCI configuration space
uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1 << 31)             // Enable bit
                     | (bus << 16)           // Bus number
                     | (device << 11)        // Device number
                     | (function << 8)       // Function number
                     | (offset & 0xFC);      // Register offset (must be aligned to 4 bytes)

    // Write the address to the PCI configuration address port
    outl(PCI_CONFIG_ADDRESS, address);
    // Read from the PCI configuration data port
    return inl(PCI_CONFIG_DATA);
}
void enumerate_pci_devices() {
    for (uint8_t bus = 0; bus < 255; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                uint32_t vendor_device_id = pci_config_read(bus, device, function, 0x00);
                uint16_t vendor_id = vendor_device_id & 0xFFFF;
                if (vendor_id == 0xFFFF) {
                    // No device present
                    continue;
                }
                else {
                }

                // Read class code (at offset 0x08)
                uint32_t class_code_reg = pci_config_read(bus, device, function, 0x08);
                uint8_t base_class = (class_code_reg >> 24) & 0xFF;
                uint8_t sub_class  = (class_code_reg >> 16) & 0xFF;
                uint8_t prog_if    = (class_code_reg >> 8)  & 0xFF;

                // Check if it's a cHCI (XHCI) controller
                if (base_class == 0x0C && sub_class == 0x03 && (prog_if == 0x30 || prog_if == 0x20 || prog_if == 0x10 ||prog_if == 0x00)) {
                    // cHCI controller found
                    print_string("found xHCI controller\n");
                    return;
                }
            }
        }
    }
}

void main() {
    clear_screen();
    enumerate_pci_devices();
}