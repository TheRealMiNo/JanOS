#include <stdint.h>
#include "low_level.h"
#include "../drivers/screen.h"
#include "../drivers/serialkeyboard.h"


void main() {
    clear_screen();
    print_string("$");
    terminal();
}
