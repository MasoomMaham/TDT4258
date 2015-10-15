#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

int playMusic(int i);	  //Defines the function playMusic() because C needs to know it exists before it is called.
void interrupthandler();  //Defines the function interruptHandler() with the same argument as above.
int i = 0;		  //Defines an Integer i to hold track on where in the sine wave we are and make the DAC play continouosly

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  	
	if(i == 7) 	  		//The if-statement allows the Handler to increment before reset. Change this and you get 
	{				//different sounds.
		i = 0;			//Set i to zero to reset the sound.
	}	

	*DAC0_CH0DATA = playMusic(i);	//*Some_Register means write to it. The code let our synthesizer write a continous stream 
  	*DAC0_CH1DATA =	playMusic(i);	// to the DAC channel 0 and 1, which means we use stereo.	
	*TIMER1_IFC = 1;		//Clearing the interrupt generated from a button pressed. 
	i++;				//Incrementing i by 1 to let the interrupt handler be able to play a smooth sine wave. 
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
	
	
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{

	interrupthandler(); 		//When a button with a even value (button SW2, SW4, etc) get pressed it calles the 
	*GPIO_IFC = 1;			//interrupthandler() function to check button pressed and perform action.
					//Clearing the button interrupt signal to allow new button press.


    /* TODO handle button pressed event, remember to clear pending interrupt */
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	interrupthandler();		//Same argument as for the GPIO_EVEN_IRQHandler() but with odd value(button SW1, SW3, etc)
	*GPIO_IFC = 1;			//Clearing the button interrupt signal to allow new button press.

    /* TODO handle button pressed event, remember to clear pending interrupt */
}

void interrupthandler(){
	uint8_t button_pressed = ~*GPIO_PC_DIN;	//Need to save an invert of the button pressed to activate the correct LED
						//to a variable of the type uint8_t, which saves only 8 bits because button and 						  LED's use 8 bits.  
	if(button_pressed == 0x10)		//if-statement checks to see if button SW5 is pressed.
	{
		//*GPIO_PA_DOUT = 0xFFFFFFFF;	
		TIMER1_IRQHandler();		//if SW5 is pressed, call the TIMER1_IRQHandler()-funksjon.
	}
	
	else if(button_pressed == 0x20)		//if-statement checks to see if button SW6 is pressed.
	{
		*GPIO_PA_DOUT = 0x0000000;	//if SW6 is pressed, turn on all the LEDs.
		
	}
	
	else if(button_pressed == 0x40)		//if-statement checks to see if button SW7 is pressed.
	{
		*GPIO_PA_DOUT = 0xFFFFFFFF;	//if SW7 is pressed, turn off all the LEDs.
		
	}
	
	else if(button_pressed == 0x80)		//if-statement checks to see if button SW8 is pressed.
	{
		*GPIO_PA_DOUT = 0xFFFFFFFF;	//if SW8 is pressed, turn off all the LEDs.
		
	}

	/* else if(button_pressed == 0x40)
	{
		int timesPressed = 1;
		for(lightNumber = 0, timesPressed < 2, lightNumber++)
		{
			if(lightNumber == 0)
			{
				*GPIO_PA_DOUT = 0x0000001;
			}
		}
	}*/	

}
