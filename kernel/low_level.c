#include <stdint.h>
#include "low_level.h"

unsigned char inb ( unsigned short port ) {
// A handy C wrapper function that reads a byte from the specified port
// "=a" ( result ) means : put AL register in variable RESULT when finished
// "d" ( port ) means : load EDX with port
unsigned char result ;
__asm__ ("in %%dx, %%al" : "=a" ( result ) : "d" ( port ));
return result ;
}
void outb ( unsigned short port , unsigned char data ) {
// "a" ( data ) means : load EAX with data
// "d" ( port ) means : load EDX with port
__asm__("out %%al, %%dx" : :"a" ( data ), "d" ( port ));
}

uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile ("inw %1, %0" : "=a" (value) : "dN" (port));
    return value;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" :: "a" (value), "dN" (port));
}

void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int strcmp(const char *str1, const char *str2) {
    int i = 0;
    while (str1[i] == str2[i]){
        if (str1[i] == '\0' || str2[i] == '\0') break;
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0') return 0;
    return 1;
}

char * strchr(char * str, char c) {
    int i = 0; 
    while(str[i] != '\0') {
        if (str[i] == c){
            return &str[i];
        }
        i++;
    }
    return &str[i];
}

int strlen(char * str) {
    int i = 0; 
    while(str[i] != '\0') {
        i++;
    }
    return i;
}