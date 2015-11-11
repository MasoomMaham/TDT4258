#include "Graphics.h"

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

bool ballStickedToPlayer = false;

int framebuffer()
{
	openFrameBuffer = open("/dev/fb0", O_RDWR);
	if(!openFrameBuffer)
	{
		printk(KERN_INFO "Could not open the file fb0 or use the framebuffer");
		return EXIT_FAILURE;
	}
	
	return openFrameBuffer;
}

void memoryMapDriver()
{
	if(ioctl(openFrameBuffer, FBIOGET_VSCREENINFO, &screenInfo) == -1)
	{
		printk(KERN_INFO "ERROR: Could not retrive the screen info\n");
		return EXIT_FAILURE:
	}

	printf("Screeninfo: %d x %d, %dbpp\n", screenInfo.xres, screenInfo.yres, screenInfo.bits_per_pixel);
	
	screen.dx = 0;
	screen.dy = 0;
	screen.width = screenInfo.xres;
	screen.height = screenInfo.yres;

	pixelsOnScreen = screen.width * screen.height;
	totalBytesUsedByScreen = pixelsOnScreen * screenInfo.bits_per_pixel / 8;	

	uint16_t *screen = (uint16_t)mmap(NULL, totalBytesUsedByScreen, PROT_READ|PROT_WRITE, MAP_SHARED, openFrameBuffer, 0);
	if((int)screen == MAP_FAILED)
	{
		printk(KERN_INFO "Memorymap the framebuffer most likely failed\n");
		return EXIT_FAILURE;
	}

	for (int totalPixels = 0; totalPixels < pixelsOnScreen; totalPixels++)
	{ 
		screen[totalPixels] = BACKGROUND_COLOR1;
	}

	return EXIT_SUCCESS;
}

void updateBufferDriver()
{
	ioctl(openFrameBuffer, 0x4680, &screenArea)
}

void disconnect_frameBuffer()
{
	munmap(screen, totalBytesUsedByScreen);
	close(openFrameBuffer); 
}

void draw_game();
{
	for(int row = 0; row < screen_height; row++)
	{
		if(row < screenHeight_margin || row > (screen_sectors_y_axis + screenHeight_margin) && row < (screen_height - (PLAYER_HEIGHT + (BALL_RADIUS*2))))
		{
			continue;
		}
		
		for(int col = 0; col < screen_width; col++)
		{
			if(row > screenHeight_margin && col > (screenWidth_margin/2) && row < (screen_sectors_y_axis + screenHeight_margin) && col < (screen_width - screenWidth_margin/2) 
			{
				draw_Brick(col, row, BRICK_WIDTH, BRICK_HEIGHT);
				col += BRICK_WIDTH + brickWidth_margin;
				if(col == screen_width - (screenWidth_margin/2))
				{
					row += BRICK_HEIGHT + brickHeight_margin;
				} 		
			}
			
			if(row > (screen_height - (screenBottom_margin + PLAYER_HEIGHT + BALL_RADIUS)) && col > (screen_width/2) && col < (screen_width/2) && row < (screen_height - (screenBottom_margin + PLAYER_HEIGHT + BALL_RADIUS))) 
			{
				row = screen_height - (PLAYER_HEIGHT + (BALL_RADIUS));
				col = (screen_width/2)
				draw_Ball(row, col, radius);	
			}
			
			if(row > (screen_height - (screenBottom_margin + PLAYER_HEIGHT)) && col > ((screen_width/2) - (PLAYER_WIDTH/2)) && col < ((screen_width/2) + (PLAYER_WIDTH/2))) && row < (screen_height - screenBottom_margin))
			{
				startPos_x = ((screen_width/2) - (PLAYER_WIDTH/2))
				startPos_y = screen_height - (screenBottom_margin + PLAYER_HEIGHT)
				draw_Player(startPos_x , startPos_y, PLAYER_WIDTH, PLAYER_HEIGHT);
				row += PLAYER_HEIGHT;
				col += PLAYER_WIDTH; 
			} 
		}
	}
}

void draw_Brick(int pen_position_x, int pen_position_y, int width, int height);
{
	int movePenDistance = 0;
	int penDrawLength_x = pen_position_x + width;
	int penDrawLength_y = pen_position_y + height;
	
	for(movePenDistance = pen_position_y; movePenDistance < penDrawLength_y; movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance);
	}	
}

void draw_line(int startPosition_x, startPosition_y, endPosition_x, endPosition_y)
{
	int randomColor = rand() % 6;
	uint16_t fillColor = colors[randomColor]; 
	int x_distance = abs(endPosition_x - startPosition_x);
	int y_distance = -abs(endPosition_y - startPosition_y);
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

void fillPixel(int startPosition_x , int startPosition_y, uint16_t fillColor)
{
	screen[startPosition_y * screen_width + startPosition_x] = fillColor; 
}

void draw_Ball(int row, int col, int radius)
{
	int diameter = radius * radius;
	for(int dy = -radius; dy <= radius; dy++)
	{
		for(int dx = -radius; dx <= radius; dx++)
		{
			if((dx*dx + dy*dy) <= diameter)
			{
				fillPixel(row + diameter, col + diameter, White); 
			}
		}
	} 
}


void draw_Player(int pen_position_x, int pen_position_y, int width, int height);
{	
	int movePenDistance = 0;
	int penDrawLength_x = pen_position_x + width;
	int penDrawLength_y = pen_position_y + height;
	
	for(movePenDistance = pen_position_y; movePenDistance < penDrawLength_y; movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance);
	}
}	

void draw_movedPlayer(int pen_position_x)
{
	int movePenDistance = 0;
	if(pen_position_x >= 0)
	{
		int penDrawLength_x = playerWidthStart + pen_position_x + (PLAYER_WIDTH/2);
	}
	
	else if(pen_position_x < 0)
	{
		int penDrawLength_x = playerWidthStart - pen_position_x - (PLAYER_WIDTH/2);
	}
	
	int penDrawLength_y = screenHeight - screenBottom_margin - PlayerHeight;
	for(movePenDistance = pen_position_y; movePenDistance < penDrawLength_y; movePenDistance++)
	{
		draw_line(pen_position_x, movePenDistance, penDrawLength_x, movePenDistance);	
}

void draw_movedBall(int row, int col, bool ballReleased)
{	
	
	if(col < 0)
	{
		ballCenter -= col;
	}
		
	else
	{
		ballCenter += col;		
	}

	if(!ballReleased)
	{
		int diameter = BALL_RADIUS * BALL_RADIUS;
		for(int dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
		{
			for(int dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
			{
				if((dx*dx + dy*dy) <= diameter)
				{
					fillPixel((screen_height - (PLAYER_HEIGHT + (BALL_RADIUS))) + diameter, ballCenter + diameter, White); 
				}
			}
		} 
	}

	else
	{
		ballStickedToPlayer = ballReleased;
		ballRowLocation -= row;
		int diameter = BALL_RADIUS * BALL_RADIUS;
		for(int dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
		{
			for(int dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
			{
				if((dx*dx + dy*dy) <= diameter)
				{
					fillPixel((ballRowLocation + diameter, ballCenter + diameter, White); 
				}
			}
		} 
	}
}

void ballMovementAfterRelease(int row, int col)
{
	ballRowLocation -= row;
	ballCenter += col;
	int diameter = BALL_RADIUS * BALL_RADIUS;
	for(int dy = -BALL_RADIUS; dy <= BALL_RADIUS; dy++)
		{
			for(int dx = -BALL_RADIUS; dx <= BALL_RADIUS; dx++)
			{
				if((dx*dx + dy*dy) <= diameter)
				{
					fillPixel((ballRowLocation + diameter, ballCenter + diameter, White); 
				}
			}
		} 
	 
}

