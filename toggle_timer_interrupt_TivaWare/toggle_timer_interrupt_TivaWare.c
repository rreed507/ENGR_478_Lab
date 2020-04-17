#include <stdint.h>
#include <stdbool.h>
#include "toggle_timer_interrupt_TivaWare.h"
#include "switch_counter_interrupt_TivaWare.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"

#define		RED	0x02
#define 	BLUE 0x04
#define 	GREEN 0x08

//*****************************************************************************
//
//!
//! A very simple example that uses a general purpose timer generated periodic 
//! interrupt to toggle the on-board LED.
//
//*****************************************************************************
int count = 0;
bool start = true;

void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    //
    // Enable pin PF2 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
	
		HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
		HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;
		
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	
		//Enable pull-up on PF4 and PF0
		GPIO_PORTF_PUR_R |= 0x11; 
}

//Globally enable interrupts 
void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

void Timer0A_Init(unsigned long period)
{   
	//
  // Enable Peripheral Clocks 
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER0A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER0A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
}

void Number(){
//word
	if (count == 0){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	}else if (count == 1){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, RED);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	}else if (count == 2){ 
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	}else if(count == 3){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, RED);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	}else if(count == 4){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GREEN);
	}else if(count == 5){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, RED);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GREEN);
	}else if(count == 6){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GREEN);
	}else if (count == 7){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, RED);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, BLUE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GREEN);	
	}
}

//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
	// acknowledge flag for Timer0A timeout
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	//check if sw1 or sw2 has been pressed
	count++;
	if(count==8){
		count = 0;
	}
	Number();
	
}

void Interrupt_Init(void)
{
  IntEnable(INT_GPIOF);  							// enable interrupt 30 in NVIC (GPIOF)
	IntPrioritySet(INT_GPIOF, 0x02); 		// configure GPIOF interrupt priority as 0
	GPIO_PORTF_IM_R |= 0x11;   		// arm interrupt on PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;     // PF0 and PF4 are edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;   	// PF0 and PF4 not both edges trigger 
  GPIO_PORTF_IEV_R &= ~0x11;  	// PF0 and PF4 falling edge event
	IntMasterEnable();       		// globally enable interrupt
}

//interrupt handler
void GPIOPortF_Handler(void)
{
	NVIC_EN0_R &= ~0x40000000; 
	SysCtlDelay(53333);	// Delay for a while
	NVIC_EN0_R |= 0x40000000; 
	
	//SW1 is pressed
	if(GPIO_PORTF_RIS_R&0x10)
	{
		// acknowledge flag for PF4
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
		count++;
	}
	
	//SW2 is pressed
  if(GPIO_PORTF_RIS_R&0x01)
	{
		// acknowledge flag for PF0
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
		count--;
	}
	Number();
}

int main(void)
{	
		unsigned long period = 16000000; //reload value to Timer0A to generate half second delay
	
		//initialize the GPIO ports	
		PortFunctionInit();
		Interrupt_Init();
		GPIOPortF_Handler();
	
		if(start == true){
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, RED);
			start = false;
		}

    //initialize Timer0A and configure the interrupt
		Timer0A_Init(period);
	
		IntMasterEnable();        		// globally enable interrupt
	
    //
    // Loop forever.
    //
    while(1)
    {
			
    }
}
