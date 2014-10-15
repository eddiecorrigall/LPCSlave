/**********************************************************************
 PROJECT    :   Cygnus X-3
 COPYRIGHT  :   2004-2006, Pitney Bowes, Inc.  
 AUTHOR     :   Steve Terebesi
 MODULE     :   interrupt.h  
 
 DESCRIPTION:
    This header file is used to define the data structures and functions
    for the interrupt controller. 
     
 MODIFICATION HISTORY:
 
    03/01/2006 Hurd        Cleaned up the header
    06/02/2005 Yuan Yuan   Removed I2C/UART2 related codes
    05/16/2005 Yuan Yuan   Removed LFSR/DMA/ONE-WIRE related codes
 --------------------------------------------------------------------------
    04/19/2004 Terebesi    Moved register definitions to ASIC.H
    04/07/2004 Terebesi    Updated to GK3 design - new register definitions added
                   Function prototypes updated
    02/17/2004 Li          Added to X2BL development
    03/27/2002 John Hurd   Swapped the Address for the Interrupt Source Register
                           and the Interrupt Raw Source Register because of a
                               documentation error.
    07/24/2000 John Hurd   Added DES and FLASH interrupts.
    06/12/2000 John Hurd   Added INTR_ASIC.
    05/16/2000 John Hurd   Initial version.
 **********************************************************************/
#ifndef INTERRUPT_H
#define INTERRUPT_H
 


/************************************************************************
    Include header files
************************************************************************/
//#include "psdtypes.h"
#include "ptypes.h"



/************************************************************************
    Defines and Typedefs
************************************************************************/
/*  Interrupt Mask type */
typedef UINT32 INTERRUPT_MASK;

/* Interrupt Mask Definitions */
#define INTR_ASIC       0x00000001                 /* ASIC     Interrupt Mask */
#define INTR_TIMER0     0x00000002                 /* Timer0   Interrupt Mask */
#define INTR_UART       0x00000004                 /* UART     Interrupt Mask */
#define INTR_TIMER1     0x00000008                 /* Timer1   Interrupt Mask */
#define INTR_TIMER2     0x00000010                 /* Timer2   Interrupt Mask */
#define INTR_DES        0x00000040                 /* DES      Interrupt Mask */   
#define INTR_FLASH      0x00000080                 /* Flash    Interrupt Mask */
#define INTR_USB        0x00000100                 /* USB      Interrupt Mask */
#define INTR_GPIO       0x00000400                 /* GPIO     Interrupt Mask */
#define INTR_TIMER3     0x00001000                 /* Timer3   Interrupt Mask */
#define INTR_TIMER4     0x00002000                 /* Timer4   Interrupt Mask */
#define INTR_GPIO_0     0x00010000                 /* GPIO_0   Interrupt Mask */
#define INTR_GPIO_1     0x00020000                 /* GPIO_1   Interrupt Mask */
#define INTR_GPIO_2     0x00040000                 /* GPIO_2   Interrupt Mask */
#define INTR_GPIO_3     0x00080000                 /* GPIO_3   Interrupt Mask */
#define INTR_GPIO_4     0x00100000                 /* GPIO_4   Interrupt Mask */
#define INTR_GPIO_5     0x00200000                 /* GPIO_5   Interrupt Mask */
#define INTR_GPIO_6     0x00400000                 /* GPIO_6   Interrupt Mask */
#define INTR_GPIO_7     0x00800000                 /* GPIO_7   Interrupt Mask */
#define INTR_GPIO_8     0x01000000                 /* GPIO_8   Interrupt Mask */
#define INTR_GPIO_9     0x02000000                 /* GPIO_9   Interrupt Mask */
#define INTR_GPIO_10    0x04000000                 /* GPIO_10  Interrupt Mask */
#define INTR_GPIO_11    0x08000000                 /* GPIO_11  Interrupt Mask */
#define INTR_GPIO_12    0x10000000                 /* GPIO_12  Interrupt Mask */
#define INTR_GPIO_13    0x20000000                 /* GPIO_13  Interrupt Mask */
#define INTR_GPIO_14    0x40000000                 /* GPIO_14  Interrupt Mask */

typedef UINT32	INTERRUPT_INDEX;

#define INTR_INDEX_0  0x00
#define INTR_INDEX_1  0x01
#define INTR_INDEX_2  0x02
#define INTR_INDEX_3  0x03
#define INTR_INDEX_4  0x04
#define INTR_INDEX_5  0x05
#define INTR_INDEX_6  0x06
#define INTR_INDEX_7  0x07
#define INTR_INDEX_8  0x08
#define INTR_INDEX_9  0x09
#define INTR_INDEX_10 0x0A
#define INTR_INDEX_11 0x0B
#define INTR_INDEX_12 0x0C
#define INTR_INDEX_13 0x0D
#define INTR_INDEX_14 0x0E
#define INTR_INDEX_15 0x0F
#define INTR_INDEX_16 0x10
#define INTR_INDEX_17 0x11
#define INTR_INDEX_18 0x12
#define INTR_INDEX_19 0x13
#define INTR_INDEX_20 0x14
#define INTR_INDEX_21 0x15
#define INTR_INDEX_22 0x16
#define INTR_INDEX_23 0x17
#define INTR_INDEX_24 0x18
#define INTR_INDEX_25 0x19
#define INTR_INDEX_26 0x1A
#define INTR_INDEX_27 0x1B
#define INTR_INDEX_28 0x1C
#define INTR_INDEX_29 0x1D
#define INTR_INDEX_30 0x1E
#define INTR_INDEX_31 0x1F

#define ALL_INTERRUPTS_32   0xFFFFFFFF        /* All interrupts - 32 bit      */
#define ALL_INTERRUPTS_16   0xFFFF            /* All interrupts - 16 bit      */
#define ALL_INTERRUPTS_8    0xFF              /* All interrupts -  8 bit      */

typedef void (*P_ISR_FUNCTION)(void);

#define ISR_ENTRY_TABLE_LOCATION 0x10080



/************************************************************************
    Function prototypes
************************************************************************/



/************************************************************************
DESCRIPTION :
    The EnableInterruptRegister function will set the interrupt 
    corresponding to the set bit in the mask passed to the function.  
    Interrupts corresponding to non-set bits will not be affected.  This 
    affects the Interrupt Set Register.

INPUT PARAMS :
    mask                - interrupt enable mask

RETURN VALUE :
    None
************************************************************************/
void EnableInterruptRegister(const INTERRUPT_MASK mask);



/************************************************************************
DESCRIPTION :
    The GetInterruptRegister function will return the current value of 
    the Interrupt Set Register.

INPUT PARAMS :
    None

RETURN VALUE :
    INTERRUPT_MASK      - interrupt enable mask
************************************************************************/
INTERRUPT_MASK GetInterruptRegister(void);



/************************************************************************
DESCRIPTION :
    The DisableInterruptRegister function will disable the interrupt 
    corresponding to the set bit in the mask passed to the function.  
    Interrupts corresponding to non-set bits will not be affected.  This 
    affects the Interrupt Clear Register.

INPUT PARAMS :
    mask                - interrupt disable mask

RETURN VALUE :
    None
************************************************************************/
void DisableInterruptRegister(const INTERRUPT_MASK mask);



/************************************************************************
DESCRIPTION :
    The GetInterruptSrcRegister function returns the current value of
    the Interrupt Source Register.  This is the state of the interrupts 
    before the interrupt mask (Interrupt Mask Register) is applied.

INPUT PARAMS :
    None

RETURN VALUE :
    INTERRUPT_MASK      - register status mask
************************************************************************/
INTERRUPT_MASK GetInterruptSrcRegister(void);



/************************************************************************
DESCRIPTION :
    The GetInterruptMaskRegister function returns the current value of 
    the Interrupt Mask Register. 

INPUT PARAMS :
    None

RETURN VALUE :
    INTERRUPT_MASK      - register status mask
************************************************************************/
INTERRUPT_MASK GetInterruptMaskRegister(void);



/************************************************************************
DESCRIPTION :
    The GetInterruptIndexRegister function returns the current value of 
    the Interrupt Index Register. 

INPUT PARAMS :
    None

RETURN VALUE :
    INTERRUPT_MASK      - register status mask
************************************************************************/
INTERRUPT_MASK GetInterruptIndexRegister(void);



#endif // INTERRUPT_H
