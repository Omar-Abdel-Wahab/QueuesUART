#include "stdint.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "tm4c123gh6pm.h"

//Tivaware Includes
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

// Macros Definitions
#define SENSOR GPIO_PIN_0 // Port E Off Sensor
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define SWITCH GPIO_PIN_4 // Port F On Sensor

// Prototypes Declarations
void init_PortF(void);
void vTest(void *);
void initTask(void *);
void BTN1_CHK_TASK(void *);
void BTN2_CHK_TASK(void *);
void UART_TASK(void *);

// Global Variables
int counter = 0;

// Queue Handle
xQueueHandle xQueue;

// Task Handles
	
int main(){
	
	init_PortF();
	xTaskCreate(vTest, "Test", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
	return 0;
}

void init_PortF(){
// Enable GPIO Port E, F clocks
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	
// Wait for the GPIOE and GPIOF modules to be ready.
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
	
// Set PF4 (SWITCH) and PE0 (SENSOR) as inputs and PF3-PF1 as outputs
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SWITCH);
GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, SENSOR);
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);
	
// Set the switch to have 2mA current intensity strength and weak pull-up resistance
GPIOPadConfigSet(GPIO_PORTF_BASE , SWITCH, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
GPIOPadConfigSet(GPIO_PORTE_BASE , SENSOR, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	
// Unlocking Port F using Tivaware
//HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= SW2;

}

void vTest(void *pvParameters){

for( ; ; ){
	// Check if SWITCH is low
if((GPIO_PORTF_DATA_R & SWITCH) != SWITCH){
	GPIO_PORTF_DATA_R |= (RED_LED | BLUE_LED | GREEN_LED);
		}

// Check if SENSOR is low
if((GPIO_PORTE_DATA_R & SENSOR) != SENSOR){
	GPIO_PORTF_DATA_R &= ~(RED_LED | BLUE_LED | GREEN_LED);
		}
	}
}
