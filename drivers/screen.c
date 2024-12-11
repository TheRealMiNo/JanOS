#include "screen.h"
#include "../kernel/low_level.h"
#include <stdarg.h>

void set_cursor(int offset) {
    offset /= 2;
    outb(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    outb(VGA_DATA_REGISTER, (unsigned char) (offset >> 8));
    outb(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    outb(VGA_DATA_REGISTER, (unsigned char) (offset & 0xff));
}

int get_cursor() {
    outb(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    int offset = inb(VGA_DATA_REGISTER) << 8;
    outb(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    offset += inb(VGA_DATA_REGISTER);
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
        if (string[i] == '\n') {
            offset = move_offset_to_new_line(offset);
            if (offset >= MAX_ROWS * MAX_COLS * 2) {
                offset = scroll_ln(offset);
            }
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

void print_word(uint16_t number) {
    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[5];
    buffer[4] = '\0';

    for (int i = 3; i >= 0; --i) {
        buffer[i] = hex_digits[number & 0xF];
        number >>= 4;
    }

    // Print the hex representation
    print_string(buffer);
}

void print_word_string(uint16_t word) {
    char str[3];
    str[0] = (char)(word & 0xFF);         // Lower byte to first character
    str[1] = (char)((word >> 8) & 0xFF);  // Upper byte to second character
    str[2] = '\0'; // Null-terminate the string
    print_string(str);
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


void echo(const char* args) {
    print_string(args);
}

void ls(const char* args, uint16_t *current_directory) {
    uint16_t buffer[256];
    read_sector(buffer, *current_directory);
    for (int i = 0; i < 5; i++){
        if (buffer[16 + i*32] == 0x0000) continue;
        for(int j = 0; j < 4; j++){
            print_word_string(buffer[16 + i*32 + j]);
        }
        print_string("\n");
    }
}

void cd(const char* args, uint16_t *current_directory) {

    //get first argument
    char* space_pos = strchr(args, ' ');
    //-1 to strip away the $
    int argument_length = space_pos ? space_pos - args : strlen(args);
    char inputed_argument[argument_length+1];
    for (int i = 0; i < argument_length; i++)
    {
        inputed_argument[i] = args[i];
    }
    inputed_argument[argument_length] = '\0';


    uint16_t buffer[256];
    read_sector(buffer, *current_directory);
    for (int i = 0; i < 256; i+=32){
        if (buffer[21+i] >> 8 == 0x10){
            for (int j = 0; args[j] != '\0' || args[j+1] != '\0'; j+=2){
                char str[3];
                str[0] = (char)(buffer[j+16] & 0xFF);         // Lower byte to first character
                str[1] = (char)((buffer[j+16] >> 8) & 0xFF);  // Upper byte to second character
                str[2] = '\0';
                if (!strcmp(str, inputed_argument)){
                    print_string("they are the same\n");
                    *current_directory = get_root_directory() + buffer[93] - 2;
                    break;
                }
            }
        }
    }
}


typedef struct {
    const char *name;
    void (*func)();
} CommandMap;


CommandMap command_table[] = {
    {"echo", echo},
    {"ls", ls},
    {"cd", cd}
};

void check_input(int offset, uint16_t *current_directory) {
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
    char inputed_command[command_length+1];
    for (int i = 0; i < command_length; i++)
    {
        inputed_command[i] = str[i+1];
    }
    inputed_command[command_length] = '\0';

    //makes that the output of the command is written op a new line
    print_string("\n");

    //look up if command exists
    for (int i = 0; i < sizeof(command_table) / sizeof(command_table[0]); i++) {
        if (!strcmp(inputed_command, command_table[i].name)) {
            command_table[i].func(space_pos+1, current_directory);
            print_string("\n$");
            return;
        }
    }
    print_string("command: \"%s\" not found\n$", inputed_command);

}

void terminal(){
    uint16_t current_directory = get_root_directory();
    print_string("$");
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
                check_input(offset, &current_directory);
                offset = get_cursor();
                continue;
            }
            set_char_at_video_memory(ascii_char, offset);
            offset += 2;
            set_cursor(offset);
        }
    }
}