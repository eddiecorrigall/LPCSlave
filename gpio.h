#ifndef __GPIO_H_
#define __GPIO_H_

#include "ptypes.h"

#define GPIO_MASK	(0x7FFF)

/****************************************/
/* ##### ##### Binary Tools ##### ##### */
/****************************************/

#define IS_LOW(bit)		((bit) == 0)
#define IS_HIGH(bit)	((bit) != 0)

/*************************************/
/* ##### ##### Registers ##### ##### */
/*************************************/

extern volatile UINT16 * const gpio_dir_register;
extern volatile UINT16 * const gpio_dir_clear_register;

extern volatile UINT16 * const gpio_data_register;
extern volatile UINT16 * const gpio_data_clear_register;

extern volatile UINT16 * const gpio_interrupt_enable_register;
extern volatile UINT16 * const gpio_interrupt_disable_register;

extern volatile UINT16 * const gpio_interrupt_status_register;
extern volatile UINT16 * const gpio_interrupt_status_clear_register;

extern volatile UINT16 * const gpio_interrupt_trigger_mode_register;
extern volatile UINT16 * const gpio_interrupt_trigger_mode_clear_register;

extern volatile UINT16 * const gpio_interrupt_active_mode_register;
extern volatile UINT16 * const gpio_interrupt_active_mode_clear_register;

/*************************************/
/* ##### ##### Prototype ##### ##### */
/*************************************/

extern void GPIO_Initialize(void);
extern void GPIO_ISR(void);

extern void LPC_Initialize(void);
extern void LPC_ISR(void);

#endif
