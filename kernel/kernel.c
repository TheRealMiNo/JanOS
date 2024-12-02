#include <stdint.h>
#include "low_level.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serialkeyboard.h"
#include "../drivers/ide_controller.h"


void main() {
    clear_screen();
    uint16_t buffer[256];
    print_string("\n\n");
    print_hex(get_root_directory());
    print_string("\n\n");
    read_sector(buffer, get_root_directory());
    for (int i = 0; i < 256; i++) {
        print_word(buffer[i]);
    }
}