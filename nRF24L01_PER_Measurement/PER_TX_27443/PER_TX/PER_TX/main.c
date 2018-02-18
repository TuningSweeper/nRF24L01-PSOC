/* Sample software for using nRF24L01 with PSoC 1 series microcontrollers.
 *
 * Target PSoC is CY8C27443-24PXI, but should work on other devices too.
 * 
 * This particular program can be used to measure packer error rate (PER).
 * When running, it transmits packets with current packet count.
 * 
 * This is not perfect in any way, but might be helpful.
 *
 * (C) 2018 Juho Vesanen, Released under MIT license.
 */

#include <m8c.h>
#include "PSoCAPI.h"
#include "PSoCGPIOINT.h"
#include "nRF24L01.h"

void main(void)
{
	BYTE payloadData[10];
	unsigned int i, counter;

	M8C_EnableGInt;	
   	LCD_Start(); 

	SPIM_Start(SPIM_SPIM_MODE_0 | SPIM_SPIM_MSB_FIRST);
	nRF24L01_commonConfiguration();
	nRF24L01_setMode(MODE_TRANSMITTER);
	
	while(1) {
		
		payloadData[0] = (counter>>8); // packet count high byte
		payloadData[1] = (counter>>8); // packet count high byte
		payloadData[2] = (counter>>8); // packet count high byte
		payloadData[3] = (counter>>8); // packet count high byte
		payloadData[4] = (counter & 0x00ff); // packet count low byte
		payloadData[5] = (counter & 0x00ff); // packet count low byte
		payloadData[6] = (counter & 0x00ff); // packet count low byte
		payloadData[7] = (counter & 0x00ff); // packet count low byte
			
		nRF24L01_sendPayload(W_TX_PAYLOAD, payloadData, 8);
		nRF24L01_transmitPayload();
			
		// this while waits until payload is successfully sent
		while( nRF24L01_is_TX_DS() != 1 ){
			if(nRF24L01_is_MAX_RT()) {
				nRF24L01_clear_MAX_RT();
				break;
			}		
		}
		nRF24L01_clear_TX_DS(); // Clear the tx complete irq

		Delay50uTimes(200);
		Delay50uTimes(200);
		counter++;
	}	
}
