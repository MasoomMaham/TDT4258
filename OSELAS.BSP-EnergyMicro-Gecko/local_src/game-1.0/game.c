#include <stdio.h>
#include <stdlib.h>
#include "game.h"


FILE* device;
bool active;
uint8_t last_input;

int player = 1;
int ball = 2;
int brick = 3;

int moveSpeed = 10;
int maxMove = (screen_width/moveSpeed);
int moveRight = (maxMove/2);
int moveLeft = (maxMove/2);

int playerWidthStart = ((screen_width/2) - (PLAYER_WIDTH/2));
int playerWidthEnd = ((screen_width/2) + (PLAYER_WIDTH/2));
int playerHeightStart = screen_height - (screenBottom_margin + PLAYER_HEIGHT);

int originBallSpeed_dx = 10;
int ballSpeed_dx;
int ballSpeed_dy = -10;
int ballRowLocation = screen_height - (PLAYER_HEIGHT + (BALL_RADIUS));
int ballCenter = (screen_width/2);
bool ballReleased = false;
bool ballOut = false;

bool positiveDirection;


int initialize()
{
	if(initialize_gamepad() == EXIT_FAILURE)
	{
		printf("ERROR: Could not initiate the gamepad!");
		return EXIT_FAILURE;
	}

	if (memoryMapDriver() == EXIT_FAILURE)
	{
		printf("Error: unable to init framebuffer.\n");
		return EXIT_FAILURE;
	}
	

	return EXIT_SUCCESS;
}

void deinitialize()
{
	tearDown_gamepad();
	disconnect_frameBuffer();
}

int initialize_gamepad()
{
	device = open("/dev/driver_name", O_RDONLY | O_NONBLOCK);
	if (!device) 
	{
		printf("Unable to open driver device, try loading the module again\n");
		return EXIT_FAILURE;
	}

	if (signal(SIGIO, &signal_handler) == SIG_ERR)
	{
		printf("ERROR: Failed while register a signal handler.\n");
		return EXIT_FAILURE;
	}
	
	if ((fileno(device), F_SETOWN, getpid()) == -1) {
		printf("Error: setting pid as owner.\n");
		return EXIT_FAILURE;
	}
	
	long oflags = fcntl(filefcntlno(device), F_GETFL);
	if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1)
	{
		printf("Error setting FASYNC flag.\n");
		return EXIT_FAILURE;
	}
		
		new_game();
		return EXIT_SUCCESS;
	}

void tearDown_gamepad()
{
	fclose(device);
}

void signal_handler(int signaloutput)
{
	printf("Signal nr.: %d\n", signaloutput);
	int input = map_input(fgetc(device));
	switch (input)
	{
		case 1:
			movePlayerLeft();
			break;
		case 3:
			movePlayerRight();
			break;
		case 5:
			releaseBall();
			break;
		case 6:
			if (last_input == 6)
			{
				new_game();
			}
			break;
		case 8:
			if (last_input == 8)
			{
				active = false;
			}
			break;
	}

	last_input = input;
}

void new_game()
{
	fillBackground();
	draw_game();
}

void movePlayerRight()
{
	if(moveRight < 0)
	{
		moveRight = 0;
		printf("Can't move outside the screen! Right move 0");
	}
	
	else if(moveRight > 0 && moveLeft >= 0)
	{
		fillPreviousAnimation(player);
		draw_movedPlayer(moveSpeed);
		if(!ballReleased)
		{
			fillPreviousAnimation(ball);
			draw_movedBall(ballRowLocation, moveSpeed)
		}
	
		moveRight -= 1;
		moveLeft += 1;
	}
}

void movePlayerLeft()
{
	if(moveLeft < 0)
	{
		moveLeft = 0;
		printf("Can't move outside the screen! Left move 0");
	}
	
	else if(moveLeft > 0 && moveRight >= 0)
	{
		fillPreviousAnimation(player);
		draw_movedPlayer(-moveSpeed);
		if(!ballReleased)
		{
			fillPreviousAnimation(ball);
			draw_movedBall(ballRowLocation, -moveSpeed);
		}
	
		moveLeft -= 1;
		moveRight += 1;
	}
}

void playBallInRandomDirection()
{
	int randomSpeed_dx = rand() % 11;
	int randomDirection = rand() % 2;
	
	if(randomDirection == 0)
	{
		ballSpeed_dx = -randomSpeed_dx;
		positiveDirection = false;
	}
	
	else
	{
		ballSpeed_dx = randomSpeed_dx;
		positiveDirection = true;
	}
}

void releaseBall()
{
	if(ballReleased == false)
	{
		ballReleased = true;
		ballInPlay();
	}
	
	else
	{
		printf("The ball is allready released");
	}
}

void ballInPlay()
{ 
	playBallInRandomDirection();
	while(!ballOut)
	{
		fillPreviousAnimation(ball);
		if(detectHitWall())
		{	
			ballMovementAfterRelease(ballRowLocation, ballCenter);
			
			if(ballSpeed_dx != originBallSpeed_dx)
			{
				if(ballSpeed_dx < 0)
				{
					ballSpeed_dx = -originBallSpeed_dx; 
				}
				
				else
				{
					ballSpeed_dx = originBallSpeed_dx;
				} 
			} 			
		}
		
		else if(detectHitBrick())
		{
			ballMovementAfterRelease(ballRowLocation, ballCenter);
			if(ballSpeed_dx != originBallSpeed_dx)
			{
				if(ballSpeed_dx < 0)
				{
					ballSpeed_dx = -originBallSpeed_dx; 
				}
				
				else
				{
					ballSpeed_dx = originBallSpeed_dx;
				} 
			} 		
		}
		
		else if(detectHitPlayer())
		{
			ballMovementAfterRelease(ballRowLocation, ballCenter);
		}
		
		else if(detectGameOver())
		{
			new_game();
		}

		else
		{	
			ballRowLocation += ballSpeed_dy;
			ballCenter += ballSpeed_dx;
			ballMovementAfterRelease(ballRowLocation, ballCenter);
		}
	}
}

bool detectHitWall()
{
	if(ballRowLocation - ballSpeed < BALL_RADIUS)
	{
		if(ballRowLocation == BALL_RADIUS)
		{
			ballSpeed_dy = -ballSpeed_dy;
			return true;
		}
		
		else
		{
			ballRowLocation = BALL_RADIUS;
			ballSpeed_dy = -ballSpeed_dy;
			return true;
		}
	}
	
	else if(ballCenter - ballSpeed < BALL_RADIUS)
	{
		
		if(ballCenter == BALL_RADIUS)
		{
			ballSpeed_dx = -ballSpeed_dx;
			return true;
		}
		
		else
		{
			ballCenter = BALL_RADIUS;
			ballSpeed_dx = -ballSpeed_dx;
			return true;
		}
	}
	
	else if(ballCenter + ballSpeed > screen_width - BALL_RADIUS)
	{
		if(ballCenter == screen_width - BALL_RADIUS)
		{
			ballSpeed_dx = -ballSpeed_dx;
			return true;
		}
		
		else
		{
			ballCenter = screen_width - BALL_RADIUS;
			ballSpeed_dx = -ballSpeed_dx;
			return true;
		}
	}
	
	else
	{
		return false;
	}	
}

bool detectHitBrick()
{
	if(ballRowLocation + ballSpeed_dy <= (screenHeight_margin + screen_sectors_y_axis + BALL_RADIUS) && ballRowLocation + ballSpeed_dy >=  (screenHeight_margin - BALL_RADIUS)&& ballCenter + ballSpeed_dx >= ((screenWidth_margin/2) - BALL_RADIUS) && ballCenter + ballSpeed_dx <= (screen_width - (screenWidth_margin/2) + BALL_RADIUS)
	{	
		int col;
		int row;	

		if((ballSpeed_dy < 0 && ballSpeed dx < 0) || (ballSpeed_dy > 0 && ballSpeed dx < 0))
		{
			for(col = ballCenter - BALL_RADIUS; col > (ballCenter - BALL_RADIUS - ballSpeed_dx); col--)
			{
				if(ballSpeed_dy < 0)
				{
					for(row = ballRowLocation - BALL_RADIUS; row < ((ballCenter - BALL_RADIUS) - col); row--)
					{
						if(row != BACKGROUND_COLOR1)
						{
							ballRowLoctaion = row + BALL_RADIUS;
							ballCenter = (col + BALL_RADIUS);
							ballSpeed_dy = -ballSpeed_dy
							return true;
						}
						
						else
						{
							return false;
						}
					}
				}
				
				else
				{
					for(row = ballRowLocation + BALL_RADIUS; row < ((ballCenter - BALL_RADIUS) - col); row++)
					{
						if(row != BACKGROUND_COLOR1)
						{
							ballRowLoctaion = (row - BALL_RADIUS);
							ballCenter = (col + BALL_RADIUS);
							ballSpeed_dy = -ballSpeed_dy;
							return true;
						}
						
						else
						{
							return false;
						}
					}
				}
			}
		} 
		
		else if((ballSpeed_dy < 0 && ballSpeed_dx > 0) || (ballSpeed_dy > 0 && ballSpeed_dx > 0))
		{
			for(col = ballCenter + BALL_RADIUS; col < (ballCenter + BALL_RADIUS + ballSpeed_dx; col++)
			{
				if(ballSpeed_dy < 0)
				{
					for(row = ballRowLocation - BALL_RADIUS; row < (col - (ballCenter + BALL_RADIUS)); row--)
					{
						if(row != BACKGROUND_COLOR1)
						{
							ballRowLoctaion = (row + BALL_RADIUS);
							ballCenter = (col - BALL_RADIUS);
							ballSpeed_dy = -ballSpeed_dy;
							return true;
						}
						
						else
						{
							return false;
						}
					}	
				}

				else
				{
					for(row = ballRowLocation + BALL_RADIUS; row < (col - (ballCenter + BALL_RADIUS)); row++)
					{
						if(row != BACKGROUND_COLOR1)
						{
							ballRowLoctaion = (row - BALL_RADIUS);
							ballCenter = (col - BALL_RADIUS);
							ballSpeed_dy = -ballSpeed_dy;
							return true;
						}
						
						else
						{
							return false;
						}
					}
				}			
			}
		}
	}

	
	else
	{
		return false;
	}
}

bool detectHitPlayer()
{
	if(ballRowLocation + BALL_RADIUS >= playerHeightStart && ballCenter < playerWidthEnd && ballCenter > playerStartEnd)
	{
		if(ballSpeed_dx < 0)
		{
			for(col = ballCenter - BALL_RADIUS; col > (ballCenter - BALL_RADIUS - ballSpeed_dx); col--)
			{
				for(row = ballRowLocation + BALL_RADIUS; row < ((ballCenter - BALL_RADIUS) - col); row++)
				{
					if(row == LightGray)
					{
						ballCenter = (col + BALL_RADIUS);
						ballRowLocation = (row - BALL_RADIUS);
						ballSpeed_dy = -ballSpeed_dy;
						return true;
					}
					
					else
					{
						return false;
					}
				}
			}
		}
		
		else
		{
			for(col = ballCenter + BALL_RADIUS; col < (ballCenter + BALL_RADIUS + ballSpeed_dx; col++)
			{
				for(row = ballRowLocation + BALL_RADIUS; row < (col - (ballCenter + BALL_RADIUS)); row++)
				{
					if(row == LightGray)
					{
						ballCenter = (col - BALL_RADIUS);
						ballRowLocation = (row - BALL_RADIUS);
						ballSpeed_dy = -ballSpeed_dy;
						return true;
					}

					else
					{
						return false;
					}
				}
			}
		}

	else
	{
		return false;
	} 
}

bool detectGameOver()
{
	if(ballRowLocation > (screen_height - screenBottom_margin))
	{
		return true;
	}
	
	else
	{
		return false;
	}
}


int main()
{
	if (initialize() == EXIT_FAILURE) 
	{
		printf("Error: unable to initialize.\n");
		return EXIT_FAILURE;
	}
	
	active = true;
	while (active)
	{
		draw_game();
	
		updateBufferDriver();
		pause();
	}
	
	deinitialize();
	return EXIT_SUCCESS;
}
