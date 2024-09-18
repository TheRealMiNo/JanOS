#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

void port_byte_out(unsigned short port, unsigned char data);
unsigned char port_byte_in(unsigned short port);
void outl(unsigned short port, unsigned int data);
uint32_t inl(unsigned short port);

#endif