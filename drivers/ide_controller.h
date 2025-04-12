#pragma once
#include <stdint.h>

void read_sector(uint16_t *buffer, uint32_t lba, unsigned int sector_count);
void write_sector(uint16_t *buffer, uint32_t lba);