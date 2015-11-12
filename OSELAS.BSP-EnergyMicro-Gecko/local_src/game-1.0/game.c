#include <stdio.h>
#include <stdlib.h>
#include "game.h"


FILE* device;
bool active;
uint8_t last_input;


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
	device = fopen("/dev/gamepad", "rb");
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
	
	if (fcntl(fileno(device), F_SETOWN, getpid()) == -1) {
		printf("Error: setting pid as owner.\n");
		return EXIT_FAILURE;
	}
	
	long oflags = fcntl(fileno(device), F_GETFL);
	if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1)
	{
		printf("Error setting FASYNC flag.\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
	}

void tearDown_gamepad()
{
	fclose(device);
}

void signal_handler(int signaloutput)
{
	printf("Signal nr.: %d\n", signaloutput);
	/*int input = map_input(fgetc(device));
	switch (input)
	{
		case 1:
			left();
			break;
		case 2:
			up();
			break;
		case 3:
			right();
			break;
		case 4:
			down();
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

	last_input = input;*/
	
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
