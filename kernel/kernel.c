#include <stdint.h>
#include "low_level.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serialkeyboard.h"



void main() {
    clear_screen();
    get_bar_address(0x01, 0x01, 0x80, "IDE");
    uint16_t buffer2[256];
    for (int i = 0; i < 256; i++) {
        buffer2[i] = 0xFFFF;
    }
    write_sector(buffer2, 0);
    uint16_t buffer1[256];
    read_sector(buffer1, 0);
}
