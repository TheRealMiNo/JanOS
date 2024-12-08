#include <stdint.h>
#include "low_level.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serialkeyboard.h"
#include "../drivers/ide_controller.h"


void main() {
    clear_screen();
    terminal();
}