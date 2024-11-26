#pragma once

#include <stdint.h>

void port_byte_out(unsigned short port, unsigned char data);
unsigned char port_byte_in(unsigned short port);
void outl(unsigned short port, unsigned int data);
uint32_t inl(unsigned short port);

int strncmp(const char *s1, const char *s2, int n);