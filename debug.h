#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "ptypes.h"

#define DEBUG_BUFFER_INITIAL_BYTE (0xFF)
#define DEBUG_BUFFER_LENGTH       (100)

extern void     DEBUG_ClearBuffer(UINT8 initial_byte);
extern void     DEBUG_SaveToBuffer(UINT8 byte);
extern void     DEBUG_ReturnBuffer(UINT8 *ordered_buffer);

#endif
