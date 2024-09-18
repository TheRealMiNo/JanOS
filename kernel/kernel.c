#include <stdint.h>
#include "../drivers/screen.c"
#include "../drivers/keyboard.c"

void main() {
    clear_screen();
    enumerate_pci_devices();
}