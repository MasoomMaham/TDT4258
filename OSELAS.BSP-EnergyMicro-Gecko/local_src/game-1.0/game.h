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
int memoryMapDriver();
void tearDown_gamepad();
void signal_handler(int);

int main();
