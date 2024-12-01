#pragma once
#include <stdint.h>

void read_sector(uint16_t *buffer, uint32_t lba);
void write_sector(uint16_t *buffer, uint32_t lba);