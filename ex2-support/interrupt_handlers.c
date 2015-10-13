#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

int playMusic();
void interrupthandler();

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
	*DAC0_CH0DATA = playMusic();
  	*DAC0_CH1DATA =	playMusic();
	*TIMER1_IFC = 1;
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
	
	
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{

	interrupthandler();
	*GPIO_IFC = 1;
    /* TODO handle button pressed event, remember to clear pending interrupt */
	
	
	
	
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

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	interrupthandler();
	*GPIO_IFC = 1;
    /* TODO handle button pressed event, remember to clear pending interrupt */
}

void interrupthandler(){
	uint8_t button_pressed = ~*GPIO_PC_DIN;
	if(button_pressed == 0x10)
	{
		*GPIO_PA_DOUT = 0xFFFFFFFF;
		//*GPIO_PC_DOUT = 0xFFFFFFFF;
	}
	
	else if(button_pressed == 0x20)
	{
		*GPIO_PA_DOUT = 0x0000000;
		
	}
	
	else if(button_pressed == 0x40)
	{
		*GPIO_PA_DOUT = 0xFFFFFFFF;
		
	}
	
	else if(button_pressed == 0x80)
	{
		*GPIO_PA_DOUT = 0xFFFFFFFF;
		
	}

}
