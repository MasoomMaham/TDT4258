#include "game.h"


FILE* device;
bool active;
uint8_t last_input;

int player = 1;
int ball = 2;
int brick = 3;

int moveSpeed = 10;
int moveRight = (maxMove/2);
int moveLeft = (maxMove/2);

extern int playerWidthStart;
extern int playerWidthEnd;
extern int playerHeightStart;

int originBallSpeed_dx = 10;
int ballSpeed_dx;
int ballSpeed_dy = -10;
extern int ballRowLocation;
extern int ballCenter;
bool ballReleased = false;
bool ballOut = false;

int hitDirection_dx;
int hitDirection_dy;

bool positiveDirection;


int initialize()
{
	if(initialize_gamepad() == EXIT_FAILURE)
	{
		printf("ERROR: Could not initiate the gamepad!");
		return EXIT_FAILURE;
	}

	if (memoryMapDriver() != EXIT_SUCCESS)
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
	
	if ((fileno(device), F_SETOWN, getpid()) == -1)
	{
		printf("Error: setting pid as owner.\n");
		return EXIT_FAILURE;
	}
		
		new_game();
		return EXIT_SUCCESS;
}

void tearDown_gamepad()
{
	fclose(device);
}

int button_input(int button)
{
	uint8_t input = ~button;
	if(input == 0x10)		//if-statement checks to see if button SW5 is pressed.
	{
		return 5;
	}
	
	else if(input == 0x20)		//if-statement checks to see if button SW6 is pressed.
	{
		return 6;
	}
	
	else if(input == 0x80)		//if-statement checks to see if button SW8 is pressed.
	{	
		return 8;
	}
	
	else if(input == 0x7)
	{
		return 3;
	}

	else if(input == 0x1)
	{
		return 1;
	}		
}

void signal_handler(int signaloutput)
{
	printf("Signal nr.: %d\n", signaloutput);
	int input = button_input(fgetc(device));
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
	fill_Background();
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
			draw_movedBall(ballRowLocation, moveSpeed);
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
			
			//removeBrick(hitDirection_dx, hitDirection_dy); 		
		}
		
		else if(detectHitPlayer())
		{
			ballMovementAfterRelease(ballRowLocation, ballCenter);
		}
		
		else if(detectGameOver())
		{
			ballOut = false;
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

/*void removeBrick(int hitDirection_dx, int hitDirection_dy)
{
}*/

bool detectHitWall()
{
	if(ballRowLocation - ballSpeed_dy < BALL_RADIUS)
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
	
	else if(ballCenter - ballSpeed_dx < BALL_RADIUS)
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
	
	else if(ballCenter + ballSpeed_dx > screen_width - BALL_RADIUS)
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
	if(ballRowLocation + ballSpeed_dy <= (screenHeight_margin + screen_sectors_y_axis + BALL_RADIUS) && ballRowLocation + ballSpeed_dy >=  (screenHeight_margin - BALL_RADIUS)&& ballCenter + ballSpeed_dx >= ((screenWidth_margin/2) - BALL_RADIUS) && ballCenter + ballSpeed_dx <= (screen_width - (screenWidth_margin/2) + BALL_RADIUS))
	{	
		int col = 0;
		int row = 0;	

		if((ballSpeed_dy < 0 && ballSpeed_dx < 0) || (ballSpeed_dy > 0 && ballSpeed_dx < 0))
		{
			for(col = ballCenter; col > (ballCenter + ballSpeed_dx); col--)
			{
				if(((col - 1) - BALL_RADIUS) != BACKGROUND_COLOR1)
				{
					hitDirection_dx = (col - BALL_RADIUS) - 1;
					hitDirection_dy = row;
					ballSpeed_dx = -ballSpeed_dx;
					return true;
				}
						{
				if(ballSpeed_dy < 0)
				{
					for(row = 0; row > (col - ballCenter); row--)
					{
						if(((row - 1) - BALL_RADIUS) != BACKGROUND_COLOR1)
						{	
							hitDirection_dy = (row - BALL_RADIUS) - 1;
							ballRowLocation += (row - 1);
							ballCenter = (col - 1);
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
					for(row = 0; row < (ballCenter - col); row++)
					{
						if(((row + 1) + BALL_RADIUS) != BACKGROUND_COLOR1)
						{	
							hitDirection_dy = (row + BALL_RADIUS) + 1;
							ballRowLocation += (row + 1);
							ballCenter = (col - 1);
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
		else if((ballSpeed_dy < 0 && ballSpeed_dx > 0) || (ballSpeed_dy > 0 && ballSpeed_dx > 0))
		{
			for(col = ballCenter; col < (ballCenter + ballSpeed_dx); col++)
			{
				if(((col + 1) + BALL_RADIUS) != BACKGROUND_COLOR1)
				{
					hitDirection_dx = (col + BALL_RADIUS) + 1;
					hitDirection_dy = row + ballRowLocation;
					ballSpeed_dx = -ballSpeed_dx;
					return true;
				}

				if(ballSpeed_dy < 0)
				{
					for(row = 0; row > (ballCenter - col); row--)
					{
						if(((row - 1) - BALL_RADIUS) != BACKGROUND_COLOR1)
						{	
							hitDirection_dy = (row - BALL_RADIUS) - 1;
							ballRowLocation += (row - 1);
							ballCenter = (col + 1);
							ballSpeed_dy = -ballSpeed_dy;
							return true;
						}
					}	
				}

				else
				{
					for(row = 0; row < (col - ballCenter); row++)
					{
						if(((row + 1) + BALL_RADIUS) != BACKGROUND_COLOR1)
						{	
							hitDirection_dy = (row + BALL_RADIUS) + 1;
							ballRowLocation += (row + 1);
							ballCenter = (col + 1);
							ballSpeed_dy = -ballSpeed_dy;
							return true;
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
	int col;
	int row;
	if(ballRowLocation + BALL_RADIUS >= playerHeightStart && ballCenter < playerWidthEnd && ballCenter > playerWidthStart)
	{
		if(ballSpeed_dx < 0)
		{
			for(col = ballCenter - BALL_RADIUS; col > (ballCenter - BALL_RADIUS - ballSpeed_dx); col--)
			{
				for(row = ballRowLocation + BALL_RADIUS; row < ((ballCenter - BALL_RADIUS) - col); row++)
				{
					if(row == LightGrey)
					{
						ballCenter = (col + BALL_RADIUS);
						ballRowLocation = (row - BALL_RADIUS);
						ballSpeed_dy = -ballSpeed_dy;
						return true;
					}
				}
			}
		}

		else
		{
			for(col = ballCenter + BALL_RADIUS; col < (ballCenter + BALL_RADIUS + ballSpeed_dx); col++)
			{
				for(row = ballRowLocation + BALL_RADIUS; row < (col - (ballCenter + BALL_RADIUS)); row++)
				{
					if(row == LightGrey)
					{
						ballCenter = (col - BALL_RADIUS);
						ballRowLocation = (row - BALL_RADIUS);
						ballSpeed_dy = -ballSpeed_dy;
						return true;
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
