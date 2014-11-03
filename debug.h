#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "ptypes.h"

#define DEBUG_BUFFER_INITIAL_BYTE (0xFF)
#define DEBUG_BUFFER_LENGTH       (100)

// Circular array used to buffer debugging information.
// 1. Initialize the buffer using DEBUG_ClearBuffer,
// 2. Push a byte onto the buffer using DEBUG_SaveToBuffer,
// 3. Allocate a block of memory, pass it to DEBUG_ReturnBuffer.
// DEBUG_ReturnBuffer will write the last DEBUG_BUFFER_LENGTH bytes saved into ordered_buffer.

extern void     DEBUG_ClearBuffer(UINT8 initial_byte);
extern void     DEBUG_SaveToBuffer(UINT8 byte);
extern void     DEBUG_ReturnBuffer(UINT8* ordered_buffer);

#endif
