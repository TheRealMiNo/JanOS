#include <stdint.h>

unsigned char port_byte_in ( unsigned short port ) {
// A handy C wrapper function that reads a byte from the specified port
// "=a" ( result ) means : put AL register in variable RESULT when finished
// "d" ( port ) means : load EDX with port
unsigned char result ;
__asm__ ("in %%dx, %%al" : "=a" ( result ) : "d" ( port ));
return result ;
}
void port_byte_out ( unsigned short port , unsigned char data ) {
// "a" ( data ) means : load EAX with data
// "d" ( port ) means : load EDX with port
__asm__("out %%al, %%dx" : :"a" ( data ), "d" ( port ));
}
unsigned short port_word_in ( unsigned short port ) {
unsigned short result ;
__asm__("in %%dx, %%ax" : "=a" ( result ) : "d" ( port ));
return result ;
}
void port_word_out ( unsigned short port , unsigned short data ) {
__asm__("out %%ax, %%dx" : :"a" ( data ), "d" ( port ));
}

static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
