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
            if (offset >= MAX_ROWS * MAX_COLS * 2) {
                offset = scroll_ln(offset);
            }
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
    uint16_t FAT_buffer[256];
    uint16_t directory_cluster = *current_directory;
    int processing_long_name = 0;
    read_sector(buffer, *current_directory, 1);
    read_sector(FAT_buffer, get_fat(), 1);
    while(1){
        for (int i = 0; i < 16; i++){
            if ((buffer[i*16] & 0xFF) == 0x00E5) continue;
            if (buffer[i*16] == 0x0000) break;
            if((buffer[i*16 + 5] << 16) >> 24 == 0x0F){
                processing_long_name = 1;
                for(int j = 0; j < 5; j++){
                    print_word_string(buffer[i*16+j] >> 8 );
                }
                for(int j = 0; j < 6; j++){
                    print_word_string(buffer[i*16+j+7]);
                }
                for(int j = 0; j < 2; j++){
                    print_word_string(buffer[i*16+j+14]);
                }
                continue;
            };
            if(processing_long_name){
                processing_long_name = 0;
                print_string("\n");
                continue;
            }
            for(int j = 0; j < 4; j++){
                print_word_string(buffer[i*16 + j]);
            }
            print_string("\n");
        }
        uint32_t FAT_number = (FAT_buffer[(directory_cluster - get_root_directory() + 2)*2] | (FAT_buffer[(directory_cluster - get_root_directory() + 2)*2 + 1] << 16));
        if (FAT_number >= 0x0FFFFFF8 && FAT_number <= 0x0FFFFFFF){
            break;
        }
        else{
            directory_cluster = FAT_number + get_root_directory() - 2;
            read_sector(buffer, directory_cluster, 1);
        }
    }
}

void cat(const char* args, uint16_t *current_directory){
    //get first argument
    char* space_pos = strchr(args, ' ');
    int argument_length = space_pos ? space_pos - args : strlen(args);
    char inputed_argument[argument_length+1];
    for (int i = 0; i < argument_length; i++)
    {
        inputed_argument[i] = args[i];
    }
    inputed_argument[argument_length] = '\0';
    uint16_t buffer[256];
    read_sector(buffer, *current_directory, 1);
    for (int i = 0; i < 16; i++){
        if (buffer[i*16] == 0x0000) break; // look for attribute of file instead of name
        if ((buffer[i*16] & 0xFF) == 0x00E5) continue;

        char entry[14] = {0};
        if((buffer[i*16 + 5] >> 8) == 0x0F){
            if((buffer[i*16 + 21] >> 8) == 0x10) continue;
            for(int j = 0; j < 5; j++){
                entry[j] = (char)(buffer[i*16+j] >> 8 & 0xFF);
            }
            for(int j = 0; j < 6; j++){
                entry[j+5] = (char)(buffer[i*16+j+7]& 0xFF);
            }
            for(int j = 0; j < 2; j++){
                entry[j+11] = (char)(buffer[i*16+j+14]& 0xFF);
            }
            if(strcmp(entry, inputed_argument) == 0){
                unsigned int amount = (buffer[i*16+30] + (buffer[i*16+31] << 16) + 511)/512;
                uint16_t file[256];
                for(int k = 0; k < amount; k++){
                    read_sector(file, get_root_directory() + buffer[i*16+29] - 2 + k, 1);
                    for(int j = 0; j < 256; j++){
                        print_word_string(file[j]);
                    }
                }
                return;
            }
            i++;
            continue;
        };
        if((buffer[i*16 + 5] >> 8) == 0x10) continue;
        for(int j = 0; j < 4; j++){
            for(int j = 0; j < 4; j++){
                if((char)(buffer[i*16+j*2] & 0xFF) == ' ') break;
                entry[j*2] = (char)(buffer[i*16+j*2] & 0xFF);         // Lower byte to first character
                if((char)((buffer[i*16+j*2] >> 8) & 0xFF) == ' ') break;
                entry[j*2+1] = (char)((buffer[i*16+j*2] >> 8) & 0xFF);  // Upper byte to second character
            }
            if(strcmp(entry, inputed_argument) == 0){
                uint16_t file[256];
                read_sector(file, get_root_directory() + buffer[i*16+13] - 2, 1);
                for(int j = 0; j < 256; j++){
                    print_word_string(file[j]);
                }
                return;
            }     
        }
    }
    print_string("File %s not found", inputed_argument);
}

void cd(const char* args, uint16_t *current_directory){
    //get first argument
    char* space_pos = strchr(args, ' ');
    int argument_length = space_pos ? space_pos - args : strlen(args);
    char inputed_argument[argument_length+1];
    for (int i = 0; i < argument_length; i++)
    {
        inputed_argument[i] = args[i];
    }
    inputed_argument[argument_length] = '\0';
    uint16_t buffer[256];
    read_sector(buffer, *current_directory, 1);
    for (int i = 0; i < 16; i++){                    
        char entry[14] = {0};
        if (buffer[i*16] == 0x0000) break;
        if ((buffer[i*16] & 0xFF) == 0x00E5) continue;
        //looking for longname directory entries
        if((buffer[i*16 + 5] << 16) >> 24 == 0x0F){
            if((buffer[i*16 + 21] >> 8)== 0x10){
                if((buffer[i*16 + 5] << 16) >> 24 == 0x0F){
                    for(int j = 0; j < 5; j++){
                        entry[j] = (char)(buffer[i*16+j] >> 8 & 0xFF);
                    }
                    for(int j = 0; j < 6; j++){
                        entry[j+5] = (char)(buffer[i*16+j+7]& 0xFF);
                    }
                    for(int j = 0; j < 2; j++){
                        entry[j+11] = (char)(buffer[i*16+j+14]& 0xFF);
                    }
                }
                if(strcmp(entry, inputed_argument) == 0){
                    if (buffer[i*16+29] == 0) {
                        *current_directory = get_root_directory();
                    }
                    else *current_directory = get_root_directory() + buffer[i*16+29] - 2;
                    print_hex(*current_directory);
                    return;
                }
            i++;
            continue;
            }
        }
        //looking for normal directory entries
        if((buffer[i*16 + 5] >> 8) == 0x10){
            for(int j = 0; j < 4; j++){
                if((char)(buffer[i*16+j*2] & 0xFF) == ' ') break;
                entry[j*2] = (char)(buffer[i*16+j*2] & 0xFF);         // Lower byte to first character
                if((char)((buffer[i*16+j*2] >> 8) & 0xFF) == ' ') break;
                entry[j*2+1] = (char)((buffer[i*16+j*2] >> 8) & 0xFF);  // Upper byte to second character
            }
            if(strcmp(entry, inputed_argument) == 0){
                if (buffer[i*16+13] == 0) {
                    *current_directory = get_root_directory();
                }
                else *current_directory = get_root_directory() + buffer[i*16+13] - 2;
                return;
            }     
        }
    }
    print_string("Directory: %s not found", inputed_argument);
}


typedef struct {
    const char *name;
    void (*func)();
} CommandMap;


CommandMap command_table[] = {
    {"echo", echo},
    {"ls", ls},
    {"cd", cd},
    {"cat", cat}
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
            if (ascii_char == 'p'){
                    set_cursor(6);
                    print_hex(get_fat());
            }
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