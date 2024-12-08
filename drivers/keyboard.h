#pragma once

#include <stdint.h>

// PCI I/O ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
uint32_t get_bar_address(uint8_t input_bc, uint8_t input_sc, uint8_t input_pf, char *search);
void init_xhci(uint32_t bar_address);

typedef struct {
    uint32_t MessageAddressLow;
    uint32_t MessageAddressHigh;
    uint32_t MessageData;
    uint32_t VectorControl;  // Bit 0: Mask bit, should be cleared to enable
} MSI_X_Vector;