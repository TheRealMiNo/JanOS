#pragma once

#include <stdint.h>

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
void outl(unsigned short port, unsigned int data);
uint32_t inl(unsigned short port);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

int strcmp(const char *str1, const char *str2);
char * strchr(char * str, char c);
int strlen(char * str);