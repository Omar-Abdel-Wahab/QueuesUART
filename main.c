#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#define SW2 0x01

void init_PortF(void);

int main(){

	return 0;
}

void init_PortF(){

// Unlocking Port F using Tivaware
HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= SW2;

}
