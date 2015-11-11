#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "DrawGraphic.h"

//Prototypes:
int initialize();
void deinitialize();
int initialize_gamepad();
int initialize_framebuffer();
void tearDown_gamepad();
void signal_handler(int signaloutput);
int signal(void SIGIO,int &signal_handler);
int main();
