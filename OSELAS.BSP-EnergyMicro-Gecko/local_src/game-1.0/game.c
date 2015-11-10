#include <stdio.h>
#include <stdlib.h>

FILE* device;




int initialize()
{
	if(initialize_gamepad() == EXIT_FAILURE)
	{
		printf("ERROR: Could not initiate the gamepad!");
		return EXIT_FAILURE
	}

	if (initialize_framebuffer() == EXIT_FAILURE)
	{
		printf("Error: unable to init framebuffer.\n");
		return EXIT_FAILURE;
	}
	
	new_game();
	return EXIT_SUCCESS;
}

void deinit()
{
	tearDown_gamepad();
	tearDown_framebuffer();
}

int init_gamepad()
{
	device = fopen("/dev/gamepad", "rb");
	if (!device) 
	{
		printf("Unable to open driver device, try loading the module again\n");
		return EXIT_FAILURE;
	}

	if (signal(SingnalHandler, &sigio_handler) == SIGNAL_ERROR)
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
