#include "debug.h"

UINT8	debug_buffer[DEBUG_BUFFER_LENGTH];
UINT8	debug_buffer_initial_byte = DEBUG_BUFFER_INITIAL_BYTE;
int	debug_buffer_index = 0;

void DEBUG_ClearBuffer(UINT8 initial_byte)
{
	int i;
	
	debug_buffer_initial_byte = initial_byte;
	
	for(i = 0; i < DEBUG_BUFFER_LENGTH; i++)
	{
		debug_buffer[i] = debug_buffer_initial_byte;
	}
}

void DEBUG_SaveToBuffer(UINT8 byte)
{
	debug_buffer[debug_buffer_index] = byte;
	debug_buffer_index += 1;
	debug_buffer_index %= DEBUG_BUFFER_LENGTH;
}

void DEBUG_ReturnBuffer(UINT8 *ordered_buffer)
{
	int i, j;
	
	j = debug_buffer_index;
	
	for(i = 0; i < DEBUG_BUFFER_LENGTH; i++)
	{
		ordered_buffer[i] = debug_buffer[j];
		j += 1;
		j %= DEBUG_BUFFER_LENGTH;
	}
}
