#include "serialkeyboard.h"
#include "../kernel/low_level.h"
#include "screen.h"

// Keyboard map to convert scan codes to ASCII characters
unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',  '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,  ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char keyboard_handler() {
    // Check if the keyboard data is available
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    if (status & 0x01) { // Output buffer full
        unsigned char scan_code = inb(KEYBOARD_DATA_PORT);

        // Check if the scan code is within range of our map
        if (scan_code < 128) {
            unsigned char ascii_char = keyboard_map[scan_code];

            // If the character is valid, print it
            if (ascii_char != 0) {
                return ascii_char;
            }
        }
    }
    return '\0';
}