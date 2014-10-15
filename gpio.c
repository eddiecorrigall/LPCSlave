#include "lpc.h"
#include "gpio.h"
#include "lpc.h"
#include "interrupt.h"

#include "ptypes.h"

/*************************************/
/* ##### ##### Registers ##### ##### */
/*************************************/

#define GPIO_BASE_REGISTER	(0x20400)

volatile UINT16 * const gpio_dir_register							= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x00);
volatile UINT16 * const gpio_dir_clear_register						= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x04);

volatile UINT16 * const gpio_data_register							= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x08);
volatile UINT16 * const gpio_data_clear_register					= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x0C);

volatile UINT16 * const gpio_interrupt_enable_register				= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x10);
volatile UINT16 * const gpio_interrupt_disable_register				= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x14);

volatile UINT16 * const gpio_interrupt_status_register				= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x18);
volatile UINT16 * const gpio_interrupt_status_clear_register		= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x1C);

volatile UINT16 * const gpio_interrupt_trigger_mode_register		= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x20);
volatile UINT16 * const gpio_interrupt_trigger_mode_clear_register	= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x24);

volatile UINT16 * const gpio_interrupt_active_mode_register			= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x28);
volatile UINT16 * const gpio_interrupt_active_mode_clear_register	= (volatile UINT16 *)(GPIO_BASE_REGISTER + 0x2C);

/*************************************/
/* ##### ##### Functions ##### ##### */
/*************************************/

extern void LPC_ISR(void);
extern void LPC_Initialize(void);

void GPIO_ISR(void)
{
	/* ### Invoke Subroutine ### */
	
	LPC_ISR(); // and others...
	
	/* Clear the entire GPIO status register so that the interrupt is not handled again */
	*gpio_interrupt_status_clear_register = GPIO_MASK;
}

void GPIO_Initialize(void)
{
	INTERRUPT_MASK interrupt_states;
	
	/* Disable all interrupts... */
	interrupt_states = GetInterruptRegister();
	DisableInterruptRegister(ALL_INTERRUPTS_32);
	
	/* Replace the old (bootloader) Interrupt Service Routine... */
	((P_ISR_FUNCTION *)ISR_ENTRY_TABLE_LOCATION)[10] = GPIO_ISR;
	
	/* Restore previous interrupt states... */
	EnableInterruptRegister(interrupt_states);
	
	/* ### Initialize Subroutines ### */
	
	LPC_Initialize(); // and others...
}
