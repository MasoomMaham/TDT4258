#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* 
  TODO calculate the appropriate sample period for the sound wave(s) 
  you want to generate. The core clock (which the timer clock is derived
  from) runs at 14 MHz by default. Also remember that the timer counter
  registers are 16 bits.
*/
/* The period between sound samples, in clock cycles */
#define   SAMPLE_PERIOD   32768

/* Declaration of peripheral setup functions */
void setupTimer(uint32_t period);  //Declaration of function setupTimer because C needs to know the function before it's called
void setupDAC();		   //Declaration of function setupDAC() with same argument as above.
void setupNVIC();		   //Declaration of function setupNVIC() with same argument as above.
void setupGPIO();		   //Declaration of function setupGPIO() with same argument as above.

/* Your code will start executing here */
int main(void) 
{  
  /* Call the peripheral setup functions */
  setupGPIO();		     //Calling function setupGPIO() to make the microcontroller handle I/O requests from button and LED's
  setupDAC();		     //Calling function setupDAC() to turn on the audio channels so it can play in stereo or mono.
  setupTimer(SAMPLE_PERIOD); //Calling function setupTimer() to activate a timer for use with interrupts. 
  
  /* Enable interrupt handling */
  setupNVIC();		     //Calling function setupNVIC() to activate the CPU's interrupt handling.
  
  /* TODO for higher energy efficiency, sleep while waiting for interrupts
     instead of infinite loop for busy-waiting
  */
  while(1)
  {

  }
	
  return 0;
}

void setupNVIC()
{
  /* TODO use the NVIC ISERx registers to enable handling of interrupt(s)
     remember two things are necessary for interrupt handling:
      - the peripheral must generate an interrupt signal
      - the NVIC must be configured to make the CPU handle the signal
     You will need TIMER1, GPIO odd and GPIO even interrupt handling for this
     assignment.
  */

	*ISER0 = 0x802;
	*ISER0 |= (1<<12); 
}


/* if other interrupt handlers are needed, use the following names: 
   NMI_Handler
   HardFault_Handler
   MemManage_Handler
   BusFault_Handler
   UsageFault_Handler
   Reserved7_Handler
   Reserved8_Handler
   Reserved9_Handler
   Reserved10_Handler
   SVC_Handler
   DebugMon_Handler
   Reserved13_Handler
   PendSV_Handler
   SysTick_Handler
   DMA_IRQHandler
   GPIO_EVEN_IRQHandler
   TIMER0_IRQHandler
   USART0_RX_IRQHandler
   USART0_TX_IRQHandler
   USB_IRQHandler
   ACMP0_IRQHandler
   ADC0_IRQHandler
   DAC0_IRQHandler
   I2C0_IRQHandler
   I2C1_IRQHandler
   GPIO_ODD_IRQHandler
   TIMER1_IRQHandler
   TIMER2_IRQHandler
   TIMER3_IRQHandler
   USART1_RX_IRQHandler
   USART1_TX_IRQHandler
   LESENSE_IRQHandler
   USART2_RX_IRQHandler
   USART2_TX_IRQHandler
   UART0_RX_IRQHandler
   UART0_TX_IRQHandler
   UART1_RX_IRQHandler
   UART1_TX_IRQHandler
   LEUART0_IRQHandler
   LEUART1_IRQHandler
   LETIMER0_IRQHandler
   PCNT0_IRQHandler
   PCNT1_IRQHandler
   PCNT2_IRQHandler
   RTC_IRQHandler
   BURTC_IRQHandler
   CMU_IRQHandler
   VCMP_IRQHandler
   LCD_IRQHandler
   MSC_IRQHandler
   AES_IRQHandler
   EBI_IRQHandler
   EMU_IRQHandler
*/
