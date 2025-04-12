#include "serialkeyboard.h"
#include "../kernel/low_level.h"
#include "screen.h"

unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',  '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,  ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',  '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0,  ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Flag to track Shift state
int shift_pressed = 0;

char keyboard_handler() {
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    if (status & 0x01) { // Output buffer full
        unsigned char scan_code = inb(KEYBOARD_DATA_PORT);

        // Handle Shift key press/release
        if (scan_code == 0x2A || scan_code == 0x36) { // Shift pressed
            shift_pressed = 1;
            return '\0';
        }
        if (scan_code == 0xAA || scan_code == 0xB6) { // Shift released
            shift_pressed = 0;
            return '\0';
        }

        // Check if the scan code is within range
        if (scan_code < 128) {
            unsigned char ascii_char = shift_pressed ? shift_map[scan_code] : keyboard_map[scan_code];

            // If the character is valid, return it
            if (ascii_char != 0) {
                return ascii_char;
            }
        }
    }
    return '\0';
}
