#ifndef LPC_STATE_MACHINE_H
#define LPC_STATE_MACHINE_H

#include "ptypes.h"

/* When the LPC state machine receives a message use LPC_GetIOMessage to read the message from the host.
 *
 * You may call LPC_GetIOMessage as many times as you like before you call LPC_SetIOMessage,
 * it will make a copy of the message buffer and write it to the memory location provided,
 * and return TRUE, otherwise it will return FALSE.
 */
extern BOOL	LPC_GetIOMessage(UINT8 *message, UINT8 *message_length);

/* When the LPC state machine receives a message use LPC_SetIOMessage to write a message to the host.
 *
 * You can only call LPC_SetIOMessage once per message received,
 * this will copy the contents of the parameters into the io transmission buffer,
 * give control to the io transmission state machine,
 * and return TRUE, otherwise it will return FALSE.
 */
extern BOOL	LPC_SetIOMessage(UINT8 *message, UINT8 message_length);

#endif
