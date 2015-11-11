
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <asm/types.h>

typedef unsigned int uint16_t;
typedef unsigned int uint8_t;

#define Black     0x0000
#define LightGrey 0xA6A6A6
#define Red       0xFF0700
#define Yellow    0xFFFF00
#define Green     0x00DC00
#define Blue      0x003FC4
#define Purple    0x8C003C
#define Orange    0xFFA100
#define White	  0xFFFFFF

#define BACKGROUND_COLOR1 Black

#define screen_width 320
#define screen_height 240

#define screenWidth_margin 50
#define screenHeight_margin 30
#define brickWidth_margin 1
#define brickHeight_margin 1
#define screenBottom_margin 5

#define screen_sectors_x_axis ((screen_width - screenWidth_margin)/27)
#define screen_sectors_y_axis ((screen_height - screenHeight_margin)/3)

#define BRICK_WIDTH 9
#define BRICK_HEIGHT 6

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 3

#define BALL_RADIUS 3

static uint16_t colors[6] = {Red, Yellow, Green, Blue, Purple, Orange};

//Prototypes:
int framebuffer();
int memoryMapDriver();
void updateBufferDriver();
void disconnect_frameBuffer();

void draw_game();
void draw_Brick(int, int, int, int);
void draw_Ball(int , int ,int);
void draw_Player(int, int, int, int);
void draw_movedPlayer(int);
void draw_movedBall(int, int, bool);
void ballMovementAfterRelease(int, int);
void fillPixel(int, int, uint16_t);



