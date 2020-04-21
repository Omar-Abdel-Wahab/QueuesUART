#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "tm4c123gh6pm.h"

//Tivaware Includes
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

// Macros Definitions
#define SENSOR GPIO_PIN_0 // Port E Off Sensor
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define SWITCH GPIO_PIN_4 // Port F On Switch

// Prototypes Declarations
void init_PortF(void);
void vTest(void *);
void initTask(void *);
void BTN1_CHK_TASK(void *);
void BTN2_CHK_TASK(void *);
void UART_TASK(void *);

// Global Variable
int counter = 0;

// Queue Handle
xQueueHandle xQueue;

// Task Handles
	
int main(){
	
	xQueue = xQueueCreate(1, sizeof( int ));
	if( xQueue != NULL ){
		xTaskCreate(BTN1_CHK_TASK, "Switch", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		xTaskCreate(BTN2_CHK_TASK, "Sensor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		xTaskCreate(UART_TASK, "UART", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
		xTaskCreate(initTask, "Init", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
	
	vTaskStartScheduler();
	for(;;);
	return 0;
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

void initTask(void *pvParameters){

for( ; ; ){
// Enable GPIO Ports E, F clocks and UART0
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	
// Wait for the GPIOA, GPIOE, GPIOF and UART0 modules to be ready.
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
	
// Set PF4 (SWITCH) and PE0 (SENSOR) as inputs and PF3-PF1 as outputs
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SWITCH);
GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, SENSOR);
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);
GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
// Set the switch to have 2mA current intensity strength and weak pull-up resistance
GPIOPadConfigSet(GPIO_PORTF_BASE , SWITCH, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
GPIOPadConfigSet(GPIO_PORTE_BASE , SENSOR, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	
// Initialize the UART. Set the baud rate, number of data bits, turn off
// parity, number of stop bits, and stick mode. The UART is enabled by the
// function call.
UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
UART_CONFIG_PAR_NONE));

// Only use this task once, no need to keep initializing
vTaskSuspend(NULL);

	}
}

void BTN1_CHK_TASK(void *pvParameters){

for( ; ; ){
	// Check if SWITCH is low
if((GPIO_PORTF_DATA_R & SWITCH) != SWITCH){
	GPIO_PORTF_DATA_R |= (RED_LED | BLUE_LED | GREEN_LED);
	counter++;
		}
	// No need to continue the rest of the time slice, yield to BTN2_CHK_TASK
taskYIELD();
	}
	
}

void BTN2_CHK_TASK(void *pvParameters){

for( ; ; ){
	// Check if SWITCH is low
if((GPIO_PORTE_DATA_R & SENSOR) != SENSOR){
	GPIO_PORTF_DATA_R &= ~(RED_LED | BLUE_LED | GREEN_LED);
	xQueueSendToBack( xQueue, &counter, 0);
	counter = 0;
		}
	// No need to continue the rest of the time slice, yield to BTN2_CHK_TASK
taskYIELD();
	}	
}

void UART_TASK(void *pvParameters){

portBASE_TYPE xStatus;
int receivedValue;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
char c;
	
for( ; ; ){

xStatus = xQueueReceive(xQueue, &receivedValue, xTicksToWait);
if(xStatus == pdPASS){
		c = (char)receivedValue;
		UARTCharPut(UART0_BASE, c);
		}

	}

}
