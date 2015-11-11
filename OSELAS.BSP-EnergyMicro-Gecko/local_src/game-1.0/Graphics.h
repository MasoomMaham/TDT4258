
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <asm/types.h>

#define Black     #0000
#define LightGrey #A6A6A6
#define Red       #FF0700
#define Yellow    #FFFF00
#define Green     #00DC00
#define Blue      #003FC4
#define Purple    #8C003C
#define Orange    #FFA100
#define White	  #FFFFFF

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
void memoryMapDriver();
void updateBufferDriver();
void tearDown_frameBuffer();

void draw_game();
void draw_Brick(int pen_position_x, int pen_position_y, int width, int height);
void draw_Ball(int , int ,int radius);
void draw_Player(int pen_position_x, pen_position_y, int width, int height);



















