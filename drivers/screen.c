#include "screen.h"
#include "../kernel/low_level.h"
#include <stdarg.h>

void set_cursor(int offset) {
    offset /= 2;
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset >> 8));
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset & 0xff));
}

int get_cursor() {
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    int offset = port_byte_in(VGA_DATA_REGISTER) << 8;
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    offset += port_byte_in(VGA_DATA_REGISTER);
    return offset * 2;
}

int get_row_from_offset(int offset) {
    return offset / (2 * MAX_COLS);
}

int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

void memory_copy(char *source, char *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

int move_offset_to_new_line(int offset) {
    return get_offset(0, get_row_from_offset(offset) + 1);
}

void set_char_at_video_memory(char character, int offset) {
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
    vidmem[offset] = character;
    vidmem[offset + 1] = WHITE_ON_BLACK;
}

int scroll_ln(int offset) {
    memory_copy(
            (char *) (get_offset(0, 1) + VIDEO_ADDRESS),
            (char *) (get_offset(0, 0) + VIDEO_ADDRESS),
            MAX_COLS * (MAX_ROWS - 1) * 2
    );

    for (int col = 0; col < MAX_COLS; col++) {
        set_char_at_video_memory(' ', get_offset(col, MAX_ROWS - 1));
    }

    return offset - 2 * MAX_COLS;
}

void print_string(char *string, ...) {
    va_list args;
    va_start(args, string);

    int offset = get_cursor();
    int i = 0;

    while (string[i] != 0) {
        if (offset >= MAX_ROWS * MAX_COLS * 2) {
            offset = scroll_ln(offset);
        }
        if (string[i] == '\n') {
            offset = move_offset_to_new_line(offset);
        }
        else if (string[i] == '\b'){
            set_cursor(offset-1);
            offset = get_cursor();
            set_char_at_video_memory(' ', offset);
        }
        else if (string[i] == '%' && string[i+1] == 's'){
            char *next_string = va_arg(args, char *);
            set_cursor(offset);
            print_string(next_string);
            offset = get_cursor();
            i++;
        }
        else {
            set_char_at_video_memory(string[i], offset);
            offset += 2;
        }
        i++;
    }
    set_cursor(offset);
    va_end(args);
}

void print_hex(uint32_t number) {
    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[9];
    buffer[8] = '\0';

    for (int i = 7; i >= 0; --i) {
        buffer[i] = hex_digits[number & 0xF];
        number >>= 4;
    }

    // Print the hex representation
    print_string(buffer);
}

void print_address_info(char *label, uint32_t address) {
    print_string(label);
    print_string(": ");
    print_hex(address);
    print_string("\n");
}

void clear_screen() {
    for (int i = 0; i < MAX_COLS * MAX_ROWS; ++i) {
        set_char_at_video_memory(' ', i * 2);
    }
    set_cursor(get_offset(0, 0));
}

void check_input(int offset) {
    int line = offset / 160;
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS + line * 160;
    char str[81];
    for (int i = 0; i < 160; i += 2){
        str[i/2] = vidmem[i];
    }
    str[80] = '\0';

    //strip command
    char* space_pos = strchr(str, ' ');
    //-1 to strip away the $
    int command_length = space_pos ? space_pos - str - 1: strlen(str);
    char command[command_length];
    for (int i = 0; i < command_length; i++)
    {
        command[i] = str[i+1];
    }

    if (!strncmp(command, "echo", command_length)) {
        print_string("\n%s\n$", str + 6);
    }
    else if (!strncmp(command, "ls", command_length)){
        print_string("\nno filesystem implemented\n$");
    }
    else {
        print_string("\ncommand not found\n$", str);
    }
}

void terminal(){
    int offset = get_cursor();
    char ascii_char;
    while (1){
        ascii_char = keyboard_handler();

        if (ascii_char) {
            //Delete character
            if (ascii_char == '\b'){
                if (offset % 160 == 2) continue;
                offset -= 2;
                set_char_at_video_memory(' ', offset);
                set_cursor(offset);
                continue;
            }
            //enter
            if (ascii_char == '\n'){
                check_input(offset);
                offset = move_offset_to_new_line(offset);
                offset = move_offset_to_new_line(offset) + 2;
                set_cursor(offset);
                continue;
            }
            set_char_at_video_memory(ascii_char, offset);
            offset += 2;
            set_cursor(offset);
        }
    }
}