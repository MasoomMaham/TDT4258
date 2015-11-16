#include "DrawGraphic.h"

int openFrameBuffer;
int pixelsOnScreen;
int totalBytesUsedByScreen;
uint16_t *screen;
struct fb_var_screeninfo screenInfo;
struct fb_copyarea screenArea;

 int playerWidthStart = ((screen_width/2) - (PLAYER_WIDTH/2));
 int playerWidthEnd = ((screen_width/2) + (PLAYER_WIDTH/2)); 
 int playerHeightStart = screen_height - (screenBottom_margin + PLAYER_HEIGHT);

 int ballCenter = (screen_width/2);
 int ballRowLocation = screen_height - (PLAYER_HEIGHT + (BALL_RADIUS));  

int framebuffer()
{
	openFrameBuffer = open("/dev/fb0", O_RDWR);
	if(!openFrameBuffer)
	{
		printf("Could not open the file fb0 or use the framebuffer\n");
		return EXIT_FAILURE;
	}
	
	return 0;
}

int memoryMapDriver()
{
	framebuffer();
	if(ioctl(openFrameBuffer, FBIOGET_VSCREENINFO, &screenInfo) == -1)
	{
		printf("ERROR: Could not retrive the screen info\n");
		return EXIT_FAILURE;
	}

	//printf("Screeninfo: %d x %d, %dbpp\n", screenInfo.xres, screenInfo.yres, screenInfo.bits_per_pixel);
	
	screenArea.dx = 0;
	screenArea.dy = 0;
	screenArea.width = screenInfo.xres;
	screenArea.height = screenInfo.yres;

	pixelsOnScreen = screenArea.width * screenArea.height;
	totalBytesUsedByScreen = pixelsOnScreen * screenInfo.bits_per_pixel / 8;	

	screen = (uint16_t)mmap(NULL, totalBytesUsedByScreen, PROT_READ|PROT_WRITE, MAP_SHARED, openFrameBuffer, 0);
	if((uintptr_t)screen == (uintptr_t)MAP_FAILED)
	{
		printf("Memorymap the framebuffer most likely failed\n");
		return EXIT_FAILURE;
	}
	
	fill_Background();

	return EXIT_SUCCESS;
}

void updateBufferDriver()
{
	ioctl(openFrameBuffer, 0x4680, &screenArea);
}

void fill_Background()
{
	int totalPixels;
	for (totalPixels = 0; totalPixels < pixelsOnScreen; totalPixels++)
	{ 
		screen[totalPixels] = BACKGROUND_COLOR1;
	}
}

void disconnect_frameBuffer()
{
	munmap(screen, totalBytesUsedByScreen);
	close(openFrameBuffer); 
}

void draw_game()
{
	int row;
	int col;
	for(row = 0; row < screen_height; row++)
	{
		if(row < screenHeight_margin || (row > (screen_sectors_y_axis + screenHeight_margin) && row < (screen_height - (PLAYER_HEIGHT + (BALL_RADIUS*2)))))
		{
			continue;
		}
		
		for(col = 0; col < screen_width; col++)
		{
			if(row > screenHeight_margin && col > (screenWidth_margin/2) && row < (screen_sectors_y_axis + screenHeight_margin) && col < (screen_width - screenWidth_margin/2)) 
			{
				draw_Brick(col, row, BRICK_WIDTH, BRICK_HEIGHT);
				col += BRICK_WIDTH + brickWidth_margin;
				if(col == screen_width - (screenWidth_margin/2))
				{
					row += BRICK_HEIGHT + brickHeight_margin;
				} 		
			}
			
			else if(row > (screen_height - (screenBottom_margin + PLAYER_HEIGHT + BALL_RADIUS)) && col > (screen_width/2) && col < (screen_width/2) && row < (screen_height - (screenBottom_margin + PLAYER_HEIGHT + BALL_RADIUS))) 
			{
				row = screen_height - (PLAYER_HEIGHT + (BALL_RADIUS));
				col = (screen_width/2);
				draw_Ball(row, col, BALL_RADIUS);	
			}
			
			else if(row > (screen_height - (screenBottom_margin + PLAYER_HEIGHT)) && col > ((screen_width/2) - (PLAYER_WIDTH/2)) && col < ((screen_width/2) + (PLAYER_WIDTH/2)) && row < (screen_height - screenBottom_margin))
			{
				int startPos_x = ((screen_width/2) - (PLAYER_WIDTH/2));
				int startPos_y = screen_height - (screenBottom_margin + PLAYER_HEIGHT);
				draw_Player(startPos_x , startPos_y, PLAYER_WIDTH, PLAYER_HEIGHT);
				row += PLAYER_HEIGHT;
				col += PLAYER_WIDTH; 
			} 
		}
	}
}


void draw_line(int startPosition_x, int startPosition_y, int endPosition_x, int endPosition_y, bool brick)
{
	uint16_t fillColor;

	if(brick)
	{
		int randomColor = rand() % 6;
		fillColor = colors[randomColor]; 
	}
	
	else
	{
		fillColor = LightGrey;
	}

	bool finish = false;
	while(!finish)
	{
		fillPixel(startPosition_x, startPosition_y, fillColor);
		if(startPosition_x == endPosition_x)
		{
			finish = true;
		}
		
		if(startPosition_y == endPosition_y)
		{
			finish = true;
		}
	}
}

void draw_Brick(int pen_position_x, int pen_position_y, int width, int height)
{
	int movePenDistance = 0;
	int penDrawLength_x = pen_position_x + width;
	int penDrawLength_y = pen_position_y + height;
	
	for(movePenDistance = pen_position_y; movePenDistance < penDrawLength_y; movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance, true);
	}	
}

void fillPixel(int startPosition_x , int startPosition_y, uint16_t fillColor)
{
	screen[startPosition_y * screen_width + startPosition_x] = fillColor; 
}

void fillPreviousAnimation(int fillArea)
{
	if(fillArea == 1)
	{
		screen[playerHeightStart * PLAYER_WIDTH + playerWidthStart] = BACKGROUND_COLOR1;
	}
	
	else if(fillArea == 2)
	{
		screen[ballRowLocation * BALL_RADIUS + ballCenter] = BACKGROUND_COLOR1;		
	}
	
	else
	{
		screen[ballRowLocation * BALL_RADIUS + ballCenter] = BACKGROUND_COLOR1;
	}
}

void draw_Ball(int row, int col, int radius)
{
	int dy;
	int dx;
	int diameter = radius * radius;
	for(dy = -radius; dy <= radius; dy++)
	{
		for(dx = -radius; dx <= radius; dx++)
		{
			if((dx*dx + dy*dy) <= diameter)
			{
				fillPixel(row + diameter, col + diameter, White); 
			}
		}
	} 
}


void draw_Player(int pen_position_x, int pen_position_y, int width, int height)
{	
	int movePenDistance = 0;
	int penDrawLength_x = pen_position_x + width;
	int penDrawLength_y = pen_position_y + height;
	
	for(movePenDistance = pen_position_y; movePenDistance < penDrawLength_y; movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance, false);
	}
}	

void draw_movedPlayer(int pen_position_x)
{
	int movePenDistance = 0;
	int penDrawLength_x = 0;
	if(pen_position_x >= 0)
	{
		penDrawLength_x = playerWidthStart + pen_position_x;
		playerWidthStart = penDrawLength_x;
	}
	
	else if(pen_position_x < 0)
	{
		penDrawLength_x = playerWidthStart - pen_position_x;
		playerWidthStart = penDrawLength_x;
	}
	
	int penDrawLength_y = screen_height - screenBottom_margin - PLAYER_HEIGHT;
	for(movePenDistance = penDrawLength_y; movePenDistance < (penDrawLength_y + PLAYER_HEIGHT); movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance, false);
	}	
}

void draw_movedBall(int row, int col)
{	

	int dy;
	int dx;
	int diameter = BALL_RADIUS * BALL_RADIUS;	
	if(col < 0)
	{
		ballCenter -= col;
	}
		
	else
	{
		ballCenter += col;		
	}

	for(dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
	{
		for(dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
		{
			if((dx*dx + dy*dy) <= diameter)
			{
				fillPixel((screen_height - (PLAYER_HEIGHT + (BALL_RADIUS))) + diameter, ballCenter + diameter, White); 
			}
		}
	} 
}

void ballMovementAfterRelease(int row, int col)
{
	ballRowLocation = row;
	ballCenter = col;
	int dy;
	int dx;
	int diameter = BALL_RADIUS * BALL_RADIUS;
	for(dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
	{
		for(dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
		{
			if((dx*dx + dy*dy) <= diameter)
			{
				fillPixel(ballRowLocation + diameter, ballCenter + diameter, White);
			}
		}
	} 	 
}

