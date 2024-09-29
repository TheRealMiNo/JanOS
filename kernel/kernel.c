#include <stdint.h>
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "low_level.h"

void main() {
    clear_screen();
    uint32_t bar_address = get_bar_address();
    init_xhci(bar_address);
}
