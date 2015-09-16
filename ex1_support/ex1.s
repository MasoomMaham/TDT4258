        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset	
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset: 
		
	//load CMU base address
	ldr r1, =CMU_BASE	
	//load current value of HFPERCUK ENABLE
	ldr r2, [r1, #CMU_HFPERCLKEN0]


	//set bit for GPIO clk
	ldr r3, =1
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO	//Logical shift left
	orr r2, r2, r3				 

	//store new value
	str r2, [r1, #CMU_HFPERCLKEN0]

	//setup NVIC for GPIO odd & even
	ldr R0, =ISER0
	ldr R1, [R0] // load any existing enabled interrupts
	movw R2, #0x802 // Pin 1 and 11 for even and odd
	orr R2, R2, R1 // or with existing value
	str R2, [R0]

	ldr R0, =GPIO_PA_BASE
	ldr R1, =GPIO_PC_BASE
	port_a .req R0
	port_c .req R1

	//setting up pins 8-15 of port A for output (LEDs)
	//high drive strength for LED
	ldr r3, =GPIO_PA_BASE
	ldr r2, =0x2
	str r2, [port_a, #GPIO_CTRL]

	//set pins 8-15 to output
	ldr r2, =0x55555555
	str r2, [port_a, #GPIO_MODEH]

	//setting up pins 0 - 7 of port C for input (buttons)
	//set pins 0 - 7 to input
	ldr r2, =0x33333333
	str r2, [port_c, #GPIO_MODEL]
	
	//Enable internal pull-ups
	ldr r2, =0xff
	str r2, [port_c, #GPIO_DOUT]	

	//Setup interupts
	ldr r4, =GPIO_BASE
	ldr r3, =0x22222222
	str r3, [r4, #GPIO_EXTIPSELL]
	ldr r3, =0xff
	str r3, [r4, #GPIO_EXTIFALL]
//	ldr r3, =0xff
//	str r3, [r4, #GPIO_EXTIRISE]
	//Enable interupt generation
	str r3, [r4, #GPIO_IEN]


	//Turn off LEDs
	ldr r2, =0xffffffff
	str r2, [port_a, #GPIO_DOUT]	
        
	//CPU enters sleep mode here and waits for an inertupt
	ldr r4, =SCR
	mov r3, #6
	str r3, [r4]
	wfi

	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  
	
	//Clear interupt flags
	ldr r3, =GPIO_BASE
	ldr r4, [r3, #GPIO_IF]
	str r4, [r3, #GPIO_IFC]
	
	//Status of pins by reading GPIO_DIN, then using that to turn on the LEDs 
	//ldr r5, [port_c, #GPIO_DIN]
	//lsl r5, r5, #8
	//str r5, [port_a, #GPIO_DOUT]	

	//Turn on all LEDs when button 1 is pressed
	ldr r5, [port_c, #GPIO_DIN]
	ldr r6, =0b11111110
	cmp r5, r6 
	beq turnAllOn
	

	//Jump to turnAllOff if button 3 is pressed
	ldr r6, =0b01111111
	cmp r5, r6
	beq turnAllOff
	//b someWhereElse


//Turn on LEDs
turnAllOn:
	ldr r5, [port_c, #GPIO_DIN]
	ldr r6, =0x00000000
	str r6, [port_a, #GPIO_DOUT]
	b end


//Turn off LEDs
turnAllOff:
	ldr r6, =0xffffffff
	str r6, [port_a, #GPIO_DOUT]
	b end

end:
	bx lr


	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing
