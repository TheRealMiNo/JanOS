#include <stdint.h>
#include "low_level.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serialkeyboard.h"
#include "../drivers/ide_controller.h"

void draw_pixle(int x, int y, int color){
    uint8_t *framebuffer = *(uint32_t *)(0x9000 + 40);
    framebuffer[x*3+y*1280*3] = color & 0b11111111;
    framebuffer[x*3+1+y*1280*3] = color >> 8 & 0b11111111;
    framebuffer[x*3+2+y*1280*3] = color >> 16 & 0b11111111;
}

void draw_circle(int x, int y, int color){
    uint8_t *framebuffer = *(uint32_t *)(0x9000 + 40);
    int radius = 100;
    for (int j = 0; j < 1024; j++) {
        for (int i = 0; i < 1280; i++){
            if(((i-x)*(i-x) + (j - y)*(j - y)) < radius*radius){
                draw_pixle(i, j, color);
            }
        }
    }
}

void draw_rectangle(int x_high, int x_low, int y_high, int y_low, int color) {
    for (int y = y_high; y < y_low; y++) {
        for (int x = x_high; x < x_low; x++) {
            draw_pixle(x, y, color);
        }
    }
}

void main() {
    uint8_t *framebuffer = *(uint32_t *)(0x9000 + 40);
    for (int j = 0; j < 1024; j++) {
        for (int i = 0; i < 1280; i++){
            draw_pixle(i, j, 0x000FF0);
        }
    }
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            draw_rectangle(40+400*i, 430+400*i, 20+300*j, 310+300*j, 0xFF00FF);
        }
    }
    int player = 0;
    char key = '\0';
    int place = 0;
    while(1){
        while(1){
            place = -1;
            key = keyboard_handler();
            if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' ||key == '7' || key == '8' || key == '9'){
                break;
            }
        }
        place = key - '1';
        if (player){
            draw_rectangle(50+400*(place%3), 420+400*(place%3), 30+300*(place/3), 300+300*(place/3), 0xFF0000);
        }
        else {
            draw_circle(250+400*(place%3), 180+300*(place/3), 0x0000FF);
        }
        player = !player;
        
    }
}