#include <stdint.h>
#include "../drivers/screen.c"
#include "../drivers/keyboard.c"
#include "low_level.h"

void main() {
    clear_screen();
    uint32_t bar_address = get_bar_address();
    reset_controller(bar_address);
}