#pragma once

#include "stdint.h"

// PCI I/O ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC
uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
uint32_t get_bar_address();
void reset_controller(uint32_t bar_address);