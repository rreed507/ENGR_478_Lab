#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"	//manually added by the programmer

#define 	GREEN_MASK 	0x08
#define 	Blue_MASK 	0x04
#define 	RED_MASK 		0x02


//*****************************************************************************
//
//!
//! A very simple example that toggles the on-board blue LED using PinMux for 
//! port initialization and functions in Peripheral Driver Library for port access
//
//*****************************************************************************
void
PortFunctionInit(void)
{
	
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		//
    // Enable pin PF0 for GPIOInput
    //
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
		//
    // Enable pin PF4 for GPIOInput
    //
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    //
    // Enable pin PF1 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	  //
    // Enable pin PF2 for GPIOOutput
    //
	  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	  //
    // Enable pin PF3 for GPIOOutput
    //
	  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
		//
		//Enable pull-up on PF0 and PF4
		//
		GPIO_PORTF_PUR_R |= 0x11; 

 
}

int main(void)
{
    uint8_t LED_data;
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
	
    //
    // Loop forever.
    //
    while(1)
    {
				if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0x00)	//SW1 is pressed
					{
						//Turn Pin 1 off
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00); 
						// Delay for a bit.
						SysCtlDelay(2000000);	
						// Toggle the LED.
						LED_data^=GREEN_MASK;	//toggle = green LED (PF3)
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, LED_data);
					}
				else if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0x00)	//SW2 is pressed
					{

						//Turn Pin 2 off
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00); 
						//Turn Pin 3 off
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); 
						// Delay for a bit.
						SysCtlDelay(2000000);	
						// Toggle the LED.
						LED_data^=RED_MASK;	//toggle = green LED (PF3)
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, LED_data);						
						
					}
				else
					{
				  //Turn Pin 3 off
				  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); 
					// Delay for a bit.
					SysCtlDelay(2000000);	
					// Toggle the LED.
					LED_data^=RED_MASK;	//toggle = red LED (PF1)
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, LED_data);
					}

			}
}