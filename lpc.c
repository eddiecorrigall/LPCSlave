#include "lpc.h"
#include "gpio.h"
#include "interrupt.h"
#include "debug.h"

/* TODO:
 * - The ABORT state has not been implemented
 * - The LRESET has not been implemented
 */

/***********************************/
/* ##### ##### Globals ##### ##### */
/***********************************/

#define LPC_MASK		(0x7F)		// 0000 0000 0111 1111 b

#define LPC_LCLK_MASK		(0x40)		// 0000 0000 0100 0000 b
#define LPC_LRESET_MASK		(0x20)		// 0000 0000 0010 0000 b
#define LPC_LFRAME_MASK		(0x10)		// 0000 0000 0001 0000 b

#define LPC_LAD_MASK		(0xF)		// 0000 0000 0000 1111 b
#define LPC_CYCTYPE_MASK	(0xC)		// 0000 0000 0000 1100 b
#define LPC_DIR_MASK		(0x2)		// 0000 0000 0000 0010 b

typedef enum {
	FRAME_START		= 0x0,		// 0000b
	FRAME_ABORT		= 0xF		// 1111b
} LPC_FRAME;

typedef enum {
	CYCTYPE_IO		= 0x0,		// 00b
	CYCTYPE_MEMORY		= 0x1,		// 01b
	CYCTYPE_DMA		= 0x3		// 11b
} LPC_CYCTYPE;

typedef enum {
	DIR_READ		= 0x0,		// 0b
	DIR_WRITE		= 0x1		// 1b
} LPC_DIR;

typedef enum {
	SYNC_READY		= 0x0,		// 0000b
	SYNC_SHORT_WAIT		= 0x5,		// 0101b
	SYNC_LONG_WAIT		= 0x6,		// 0110b
	SYNC_ERROR		= 0xA		// 1010b
} LPC_SYNC;

typedef enum {
	// Note: These enumerations are assigned values for debugging purposes only
	STATE_IDLE				= 0,
	STATE_CYCTYPE_AND_DIR			= 1,
	STATE_ADDR_0				= 2,
	STATE_ADDR_1				= 3,
	STATE_ADDR_2				= 4,
	STATE_ADDR_3				= 5,
	STATE_DATA_WRITE_0			= 6,
	STATE_DATA_WRITE_1			= 7,
	STATE_TAR_TO_PERIPHERAL_0		= 8,
	STATE_TAR_TO_PERIPHERAL_1		= 9,
	STATE_SYNC				= 10,
	STATE_DATA_READ_0			= 11,
	STATE_DATA_READ_1			= 12,
	STATE_TAR_TO_HOST_0			= 13,
	STATE_TAR_TO_HOST_1			= 14,
	STATE_ABORT				= 15
} LPC_IO_CYCLE_STATE;

LPC_IO_CYCLE_STATE lpc_state;

LPC_FRAME	lpc_frame_info;
LPC_CYCTYPE	lpc_cycle_type;
LPC_DIR		lpc_direction;
UINT16		lpc_address;
UINT8		lpc_data;
LPC_SYNC	lpc_synchronize_info;

/**************************************/
/* ##### ##### Prototypes ##### ##### */
/**************************************/

void				LPC_Initialize(void);
void				LPC_ISR(void);

__inline UINT16			LPC_ReadSignal(void);
__inline void			LPC_WriteSignal(UINT8 values);

__inline void			LPC_TurnAroundToPeripheral(void);
__inline void			LPC_TurnAroundToHost(void);

__inline LPC_IO_CYCLE_STATE	LPC_GetState(void);
__inline void			LPC_SetState(LPC_IO_CYCLE_STATE state);

void				LPC_HandleCycle(void);

// Protocols for io read and write

extern BOOL			LPC_HandleIORead(UINT16 address, UINT8 *data);
extern void			LPC_HandleIOWrite(UINT16 address, UINT8 data);

/*************************************/
/* ##### ##### Functions ##### ##### */
/*************************************/

void LPC_ISR(void)
{
	/* If the GPIO status register indicates that the interrupt was from the LCLK line */
	if((*gpio_interrupt_status_register) & LPC_LCLK_MASK)
	{
		LPC_HandleCycle();
	}
}

void LPC_Initialize(void)
{
	/* Ensure that the GPIO interrupts are disabled */
	DisableInterruptRegister(INTR_GPIO);
	
	/* Ensure that any current LPC LCLK interrupt is disabled ... */
	(*gpio_interrupt_disable_register) = LPC_LCLK_MASK;
	
	/* Have the GPIO module generate an edge triggered interrupt for the LPC LCLK pin... */
	(*gpio_interrupt_trigger_mode_register) = LPC_LCLK_MASK;
	
	/* Have the GPIO module generate an active-low triggered interrupt for the LPC LCLK so it is triggered when LCLK is falling... */
	(*gpio_interrupt_active_mode_clear_register) = LPC_LCLK_MASK;
	
	/* Set the LPC LCLK, LRESET and LFRAME line direction so the peripheral is receiving from the host... */
	(*gpio_dir_clear_register) = (LPC_LCLK_MASK | LPC_LRESET_MASK | LPC_LFRAME_MASK);
	
	/* Enable the new LPC LCLK interrupt... */
	(*gpio_interrupt_enable_register) = LPC_LCLK_MASK;
	
	// ##### ##### >>>>>
	
	/* Ensure that the host has initial control of the LAD values, so that the peripheral (this device) can read GPIO signals */
	LPC_TurnAroundToHost();
	
	/* Begin the state machine in IDLE */
	LPC_SetState(STATE_IDLE);
	
	// ##### ##### >>>>>
	
	/* Enable the GPIO interrupts */
	EnableInterruptRegister(INTR_GPIO);
}

__inline UINT8
LPC_Read(void)
{
	return (*gpio_data_register) & LPC_MASK;
}

__inline void
LPC_Write(UINT8 values)
{
	/* Filter values so that writing only happens to the LAD pins since this device is only the peripheral */
	values &= LPC_LAD_MASK;
	
	/* Set all pins high */
	(*gpio_data_register) = LPC_LAD_MASK;
	
	/* Clear pins that map to zeros bits */
	(*gpio_data_clear_register) = (~values) & LPC_LAD_MASK;
}

__inline void
LPC_TurnAroundToPeripheral(void)
{
	/* If bit is set high, the GPIO pin is in output mode */
	(*gpio_dir_register) = LPC_LAD_MASK;
}

__inline void
LPC_TurnAroundToHost(void)
{
	/* If bit is set low, the GPIO pin is in input mode */
	(*gpio_dir_clear_register) = LPC_LAD_MASK;
}

__inline LPC_IO_CYCLE_STATE
LPC_GetState(void)
{
	return lpc_state;
}

__inline void
LPC_SetState(LPC_IO_CYCLE_STATE state)
{
	lpc_state = state;
}

void LPC_HandleCycle(void)
{
	static UINT8 lframe = FALSE;
	
	UINT8 last_lframe;
	
	UINT8 signal;
	
	// Important values
	
	BOOL lframe_falling;
	BOOL lframe_rising;
	BOOL lframe_active;
	
	UINT8 lad;
	
	// Signal
	
	signal = LPC_Read();
	
	//DEBUG_SaveToBuffer(0xAA);
	//DEBUG_SaveToBuffer(signal);
	
	// LFRAME
	
	last_lframe = lframe;
	lframe = (signal & LPC_LFRAME_MASK);
	
	lframe_falling = IS_HIGH(last_lframe) && IS_LOW(lframe);
	lframe_rising = IS_LOW(last_lframe) && IS_HIGH(lframe);
	lframe_active = IS_LOW(lframe);
	
	// LAD
	
	lad = (signal & LPC_LAD_MASK);
	
	/* ### State Machine - Handle Frame ### */
	
	if(lframe_falling)
	{
		/* Note: The host may have aborted unexpectently, so the state machine must be cleaned */
		
		/* Ensure that the cycle state has been returned to an idle state */
		LPC_SetState(STATE_IDLE);
		
		/* Ensure that the LAD values are readable */
		LPC_TurnAroundToHost();
		
		return; /* Exit early (collect the newly accessable LAD values before the next falling LCLK) */
	}
	
	if(lframe_active)
	{
		/* Store the frame to be recalled for later use */
		lpc_frame_info = (LPC_FRAME)lad;
		
		return; /* Exit early (wait for next falling LCLK) */
	}
	
	if(lframe_rising)
	{
		switch(lpc_frame_info)
		{
			case FRAME_START: /* (0000b) Start cycle */
			{
				LPC_SetState(STATE_CYCTYPE_AND_DIR);
			
			} break;
			
			case FRAME_ABORT: /* (1111b) Stop cycle */
			{
				LPC_SetState(STATE_ABORT);
			
			} break;
			
			default:
			{
				LPC_SetState(STATE_IDLE);
			
			} break;
		}
	}
	
	/* ### State Machine - Handle Cycle ### */
	
	//DEBUG_SaveToBuffer(0xBB);
	//DEBUG_SaveToBuffer(LPC_GetState());
	
	switch(LPC_GetState())
	{
		// ### STATE_CYCTYPE_AND_DIR ### //
	
		case STATE_CYCTYPE_AND_DIR:
		{
			lpc_cycle_type = (LPC_CYCTYPE)((lad & LPC_CYCTYPE_MASK) >> 2);
			lpc_direction = (LPC_DIR)((lad & LPC_DIR_MASK) >> 1);
			
			if(lpc_cycle_type == CYCTYPE_IO)
			{
				LPC_SetState(STATE_ADDR_0);
			}
			else
			{
				LPC_SetState(STATE_IDLE);
			}
		
		} break;
		
		// ### STATE_ADDR ### //
		
		case STATE_ADDR_0:
		{
			lpc_address = 0;
			
			lpc_address <<= 4;
			lpc_address |= lad;
			
			LPC_SetState(STATE_ADDR_1);
		
		} break;
		
		case STATE_ADDR_1:
		{
			lpc_address <<= 4;
			lpc_address |= lad;
			
			LPC_SetState(STATE_ADDR_2);
		
		} break;
		
		case STATE_ADDR_2:
		{
			lpc_address <<= 4;
			lpc_address |= lad;
			
			LPC_SetState(STATE_ADDR_3);
		
		} break;
		
		case STATE_ADDR_3:
		{
			lpc_address <<= 4;
			lpc_address |= lad;
			
			if(lpc_direction == DIR_WRITE)
			{
				LPC_SetState(STATE_DATA_WRITE_0);
			}
			else
			{
				LPC_SetState(STATE_TAR_TO_PERIPHERAL_0);
			}
		
		} break;
		
		// ### STATE_DATA_WRITE ### //
		
		case STATE_DATA_WRITE_0:
		{
			lpc_data = 0;
			
			lpc_data |= (lad << 0);
			
			LPC_SetState(STATE_DATA_WRITE_1);
		
		} break;
		
		case STATE_DATA_WRITE_1:
		{
			lpc_data |= (lad << 4);
			
			LPC_SetState(STATE_TAR_TO_PERIPHERAL_0);
		
		} break;
		
		// ### STATE_TAR_TO_PERIPHERAL ### //
		
		case STATE_TAR_TO_PERIPHERAL_0:
		{
			/* Drive out an early sync signal so that the sync is less likely to be missed by the host... */
			
			if(lpc_direction == DIR_READ)
			{
				lpc_synchronize_info = SYNC_SHORT_WAIT;
			}
			else
			{
				lpc_synchronize_info = SYNC_READY;
			}
			
			LPC_Write(lpc_synchronize_info);
			
			LPC_SetState(STATE_TAR_TO_PERIPHERAL_1);
		
		} break;
		
		case STATE_TAR_TO_PERIPHERAL_1:
		{
			LPC_TurnAroundToPeripheral();
			
			LPC_SetState(STATE_SYNC);
		
		} break;
		
		// ### STATE_SYNC ### //
		
		case STATE_SYNC:
		{
			if(lpc_direction == DIR_READ)
			{
				if(LPC_HandleIORead(lpc_address, (&lpc_data)))
				{
					lpc_synchronize_info = SYNC_READY;
					LPC_Write(lpc_synchronize_info);
					
					LPC_SetState(STATE_DATA_READ_0);
				}
				else
				{
					/* IMPORTANT: If the Sync pattern is '0101b',
					 * then the maximum number of SYNC clocks is assumed to be 8.
					 * If the host sees that more than 8 clocks of this SYNC value have been driven,
					 * it may abort the cycle.
					 */
					
					lpc_synchronize_info = SYNC_SHORT_WAIT; /* Allow host to abort */
					LPC_Write(lpc_synchronize_info);
				}
			}
			else
			{
				lpc_synchronize_info = SYNC_READY;
				LPC_Write(lpc_synchronize_info);
				
				if(lpc_direction == DIR_WRITE)
				{
					LPC_HandleIOWrite(lpc_address, lpc_data);
				}
				
				LPC_SetState(STATE_TAR_TO_HOST_0);
			}
		
		} break;
		
		// ### STATE_DATA_READ ### //
		
		case STATE_DATA_READ_0:
		{
			LPC_Write((lpc_data >> 0) & LPC_LAD_MASK);
			
			LPC_SetState(STATE_DATA_READ_1);
		
		} break;
		
		case STATE_DATA_READ_1:
		{
			LPC_Write((lpc_data >> 4) & LPC_LAD_MASK);
			
			LPC_SetState(STATE_TAR_TO_HOST_0);
		
		} break;
		
		// ### STATE_TAR_TO_HOST ### //
		
		case STATE_TAR_TO_HOST_0:
		{
			LPC_Write(0xF);
			
			LPC_SetState(STATE_TAR_TO_HOST_1);
			
		} break;
		
		case STATE_TAR_TO_HOST_1:
		{
			LPC_TurnAroundToHost();
			
			LPC_SetState(STATE_IDLE);
		
		} break;
		
		// ### STATE_ABORT ### //
		
		case STATE_ABORT:
		{
			// TODO: ...
			
		} break;
	}
}
