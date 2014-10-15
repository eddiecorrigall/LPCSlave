#include "ptypes.h"
//#include "debug.h"

/* ### I/O Transmission Test Code ###
 *
 * Note: These tests assume that the data sent will be the same as received
 *
 * ### Test #1 - Simple tests that will not challenge checksum extraordinarily
 *
 * Single-Byte >>>
 * 
 * State	Command				Expected Return Value
 *
 * LENGTH	"lpc io_write 0100 01"		N/A
 * DATA		"lpc io_write 0000 69"		N/A
 * CHECKSUM	"lpc io_write 0101 69"		N/A
 * ACK		"lpc io_read 0102"		0xA0
 * 
 * LENGTH	"lpc io_read 0100"		0x01
 * DATA		"lpc io_read 0000"		0x69
 * CHECKSUM	"lpc io_read 0101"		0x69
 * ACK		"lpc io_write 0102 A0"		N/A
 *
 * Multi-Byte >>>
 *
 * LENGTH	"lpc io_write 0100 03"		N/A
 * DATA		"lpc io_write 0000 69"		N/A
 * DATA		"lpc io_write 0001 3C"		N/A
 * DATA		"lpc io_write 0002 5A"		N/A
 * CHECKSUM	"lpc io_write 0101 FF"		N/A
 * ACK		"lpc io_read 0102"		0xA0
 *
 * LENGTH	"lpc io_read 0100"		0x03
 * DATA		"lpc io_read 0002"		0x5A
 * DATA		"lpc io_read 0000"		0x69
 * DATA		"lpc io_read 0001"		0x3C
 * CHECKSUM	"lpc io_read 0101"		0xFF
 * ACK		"lpc io_write 0102 A0"		N/A
 *
 * ### Test #2 - Test checksum by causing the checksum to overflow
 *
 * State	Command				Expected Return Value
 *
 * LENGTH	"lpc io_write 0100 04"		N/A
 * DATA		"lpc io_write 0000 5A"		N/A
 * DATA		"lpc io_write 0001 69"		N/A
 * DATA		"lpc io_write 0002 3C"		N/A
 * DATA		"lpc io_write 0003 D2"		N/A
 * CHECKSUM	"lpc io_write 0101 D1"		N/A
 * ACK		"lpc io_read 0102"		0xA0
 *
 * LENGTH	"lpc io_read 0100"		0x04
 * DATA		"lpc io_read 0002"		0x3C
 * DATA		"lpc io_read 0000"		0x5A
 * DATA		"lpc io_read 0003"		0xD2
 * DATA		"lpc io_read 0001"		0x69
 * CHECKSUM	"lpc io_read 0101"		0xD1
 * ACK		"lpc io_write 0102 A0"		N/A
 *
 * ### Test #3 - Test checksum by giving bad data
 * 
 * State	Command				Expected Return Value
 *
 * LENGTH	"lpc io_write 0100 02"		N/A
 * DATA		"lpc io_write 0001 12"		N/A
 * DATA		"lpc io_write 0000 34"		N/A
 * CHECKSUM	"lpc io_write 0101 70"		N/A			(Give peripheral a bad checksum)
 * ACK		"lpc io_read 0102"		0xAF
 *
 * LENGTH	"lpc io_write 0100 02"		N/A
 * DATA		"lpc io_write 0001 12"		N/A
 * DATA		"lpc io_write 0000 34"		N/A
 * CHECKSUM	"lpc io_write 0101 46"		N/A
 * ACK		"lpc io_read 0102"		0xA0
 *
 * LENGTH	"lpc io_read 0100"		0x02
 * DATA		"lpc io_read 0001"		0x12
 * DATA		"lpc io_read 0000"		0x34
 * CHECKSUM	"lpc io_read 0101"		0x46
 * ACK		"lpc io_write 0102 AF"		N/A			(Claim that the peripheral gave bad data)
 *
 * LENGTH	"lpc io_read 0100"		0x02
 * DATA		"lpc io_read 0001"		0x12
 * DATA		"lpc io_read 0000"		0x34
 * CHECKSUM	"lpc io_read 0101"		0x46
 * ACK		"lpc io_write 0102 A0"		N/A
 *
 */

// Note: All addresses referencing data are in the form of 00xx where xx is [0, 0xFF)

#define MSG_MAX_LENGTH		(256)

#define MSG_ADDR_OF_DATA	(0x0000)
// ...
#define MSG_ADDR_OF_LENGTH	(MSG_MAX_LENGTH + 0)
#define MSG_ADDR_OF_CHECKSUM	(MSG_MAX_LENGTH + 1)
#define MSG_ADDR_OF_ACK		(MSG_MAX_LENGTH + 2)

typedef enum {
	ACK_PASS = 0xA0,
	ACK_FAIL = 0xAF
} MSG_ACK;

BOOL	usr_has_control = FALSE;

MSG_ACK	msg_ack;
UINT8	msg_length;
UINT8	msg_checksum;
UINT8	msg_data[MSG_MAX_LENGTH];

/* ### Mater Driver Code: Send Msg to Peripheral ###
 *
 * $length = sizeof($msg)
 * lpc(IO_WRITE, MSG_ADDR_OF_LENGTH, $length)
 * while(true)
 * 		for($i = 0; $i < $length; $i++)
 * 			lpc(IO_WRITE, $i, $msg[$i])
 * 		lpc(IO_WRITE, MSG_ADDR_OF_CHECKSUM, checksum($msg))
 * 		$ack = lpc(IO_READ, MSG_ADDR_OF_ACK)
 * 		if($ack)
 * 			break
 *		else
 * 			continue
 */

void LPC_HandleIOWrite(UINT16 address, UINT8 data)
{
	UINT16 tmp_checksum;
	
	if(usr_has_control) return;
	
	switch(address)
	{
		// #1
		case MSG_ADDR_OF_LENGTH: {
		
			msg_ack = ACK_FAIL;
			msg_length = data;
			msg_checksum = 0;
			
			//DEBUG_SaveToBuffer(1);
		
		} break;
		
		// #3
		case MSG_ADDR_OF_CHECKSUM: {
			
			msg_ack = (msg_checksum == data)
				? ACK_PASS
				: ACK_FAIL;
			
			//DEBUG_SaveToBuffer(3);
		
		} break;
		
		// #10
		case MSG_ADDR_OF_ACK: {
		
			msg_ack = (MSG_ACK)data;
			msg_checksum = 0; // Important: Otherwise checksum will always contain at least one bad byte
			
			//DEBUG_SaveToBuffer(10);
		
		} break;
	
		// #2
		default: {
		
			if(address >= MSG_MAX_LENGTH) return;
			
			msg_data[address] = data;
			
			tmp_checksum = msg_checksum;
			tmp_checksum += data;
			tmp_checksum %= 256;
			msg_checksum = tmp_checksum;
			
			//DEBUG_SaveToBuffer(2);
		
		} break;
	}
	
	// ##### ##### >>>>>
	
	/* LPC TEST CODE... */
	
	//DEBUG_SaveToBuffer(0xAA);
	//DEBUG_SaveToBuffer((address >> 8) & 0xFF);
	//DEBUG_SaveToBuffer((address >> 0) & 0xFF);
	
	//DEBUG_SaveToBuffer(0xDD);
	//DEBUG_SaveToBuffer((data >> 0) & 0xFF);
}

/* ### Master Driver Code: Receive Msg from Peripheral ###
 *
 * $length = lpc(IO_READ, MSG_ADDR_OF_LENGTH)
 * while(true)
 * 		for($i = 0; $i < $length; $i++)
 * 			$msg[$i] = lpc(IO_READ, $i)
 * 		$ack = (lpc(IO_READ, MSG_ADDR_OF_CHECKSUM) == checksum($msg))
 * 		lpc(IO_WRITE, ADDR_OF_ACK, $ack)
 *		if($ack)
 * 			break
 *		else
 * 			continue
 */

BOOL LPC_HandleIORead(UINT16 address, UINT8 *data)
{
	UINT16 tmp_checksum;
	
	if(usr_has_control) return FALSE;
	
	switch(address)
	{
		// #7
		case MSG_ADDR_OF_LENGTH: {
		
			msg_ack = ACK_FAIL;
			(*data) = msg_length;
			msg_checksum = 0;
			
			//DEBUG_SaveToBuffer(7);
		
		} break;
		
		// #9
		case MSG_ADDR_OF_CHECKSUM: {
		
			(*data) = msg_checksum;
			
			//DEBUG_SaveToBuffer(9);
		
		} break;
		
		// #4
		case MSG_ADDR_OF_ACK: {
		
			(*data) = (UINT8)msg_ack;
			msg_checksum = 0; // Important: Otherwise checksum will always contain at least one bad byte
			usr_has_control = (msg_ack == ACK_PASS);
			
			//DEBUG_SaveToBuffer(4);
		
		} break;
		
		// #8
		default: {
		
			if(address >= MSG_MAX_LENGTH) return FALSE;
			
			(*data) = msg_data[address];
			
			tmp_checksum = msg_checksum;
			tmp_checksum += msg_data[address];
			tmp_checksum %= 256;
			msg_checksum = tmp_checksum;
			
			//DEBUG_SaveToBuffer(8);
		
		} break;
	}
	
	// ##### ##### >>>>>
	
	/* LPC TEST CODE... */
	
	//DEBUG_SaveToBuffer(0xAA);
	//DEBUG_SaveToBuffer((address >> 8) & 0xFF);
	//DEBUG_SaveToBuffer((address >> 0) & 0xFF);
	
	//DEBUG_SaveToBuffer(0xDD);
	//DEBUG_SaveToBuffer(((*data) >> 0) & 0xFF);
	
	return TRUE;
}

// #5
BOOL LPC_GetIOMessage(UINT8 *buffer, UINT8 *buffer_length)
{
	int i;
	
	if(usr_has_control)
	{
		//DEBUG_SaveToBuffer(5);
		
		for(i = 0; i < msg_length; i++)
		{
			buffer[i] = msg_data[i];
		}
		
		(*buffer_length) = msg_length;
		
		return TRUE;
	}
	
	return FALSE;
}

// #6
BOOL LPC_SetIOMessage(UINT8 *buffer, UINT8 buffer_length)
{
	int i;
	
	if(usr_has_control)
	{
		//DEBUG_SaveToBuffer(6);
		
		msg_length = buffer_length;
		
		for(i = 0; i < msg_length; i++)
		{
			msg_data[i] = buffer[i];
		}
		
		usr_has_control = FALSE;
		
		return TRUE;
	}
	
	return FALSE;
}
