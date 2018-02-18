/* Sample software for using nRF24L01 with PSoC 1 series microcontrollers.
 *
 * Target PSoC is CY8C27443-24PXI, but should work on other devices too.
 *
 * This particular program can be used to measure packer error rate (PER).
 * When running, it displays the number of lost and broken packets of the
 * previous 255 packets.
 * 
 * This is not perfect in any way, but might be helpful.
 *
 * (C) 2018 Juho Vesanen, Released under MIT license.
 */

#include <m8c.h>
#include "PSoCAPI.h"
#include "PSoCGPIOINT.h"
#include "nRF24L01.h"
#include <stdlib.h>

void main(void)
{
	BYTE payloadData[10];
	BYTE buffer[10];
	unsigned int currentHighByte=0, countPkt=0, countOk=0, countLost, countBroken;
	
	M8C_EnableGInt;
   	LCD_Start();
	LCD_Position(0,0);
	LCD_PrCString("Waiting packets");

	SPIM_Start(SPIM_SPIM_MODE_0 | SPIM_SPIM_MSB_FIRST);
	nRF24L01_commonConfiguration();
	nRF24L01_setMode(MODE_RECEIVER);
		

	while(1) {
		// check if something has been received
		if( nRF24L01_is_RX_DR() == 1 ){
			nRF24L01_readPayload( R_RX_PAYLOAD, payloadData, 8);
			nRF24L01_clear_RX_DR(); // clear interrupt
			nRF24L01_sendOneByte(FLUSH_RX);	// flush stuff.

			// get number of rx packets.
			countPkt++;
			
			// check if it's internally ok?
			if (
				(payloadData[0] == payloadData[1]) &
				(payloadData[0] == payloadData[2]) &
				(payloadData[0] == payloadData[3]) &
				(payloadData[0] == payloadData[1]) &
				(payloadData[4] == payloadData[5]) &
				(payloadData[4] == payloadData[6]) &
				(payloadData[4] == payloadData[7]) ) {

				countOk++;	
					
				// high byte changed? 
				if(payloadData[0] != currentHighByte) {
					currentHighByte = payloadData[0];
					
					countLost = 0xff - countPkt;
					countBroken = countPkt - countOk;
					
					countPkt = 0;
					countOk = 0;
				} 
			}
			
			
			// Print it out
			LCD_Position(0,0);    
			LCD_PrCString("                ");
			LCD_Position(0,0);  

			LCD_PrCString("L: ");
			itoa(buffer, countLost, 10);
			LCD_PrString(buffer);

			LCD_PrCString(" B: ");
			itoa(buffer, countBroken, 10);
			LCD_PrString(buffer);
				
			LCD_Position(0,12);
			LCD_PrCString("/255");
			
			LCD_Position(1,0);    
			LCD_PrCString("                ");
		    LCD_Position(1,0);
			LCD_PrHexByte(payloadData[0]);
			LCD_PrHexByte(payloadData[1]);
			LCD_PrHexByte(payloadData[2]);
			LCD_PrHexByte(payloadData[3]);
			LCD_PrHexByte(payloadData[4]);
			LCD_PrHexByte(payloadData[5]);
			LCD_PrHexByte(payloadData[6]);
			LCD_PrHexByte(payloadData[7]);
		}
	}	
}


