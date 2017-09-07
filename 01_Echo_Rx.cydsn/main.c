/*
 * 01_Echo_Rx
 * 
 * The nrf24 radio is configured to send back ACK + 1 byte payload.
 * The payload byte is the received char turned into uppercase.
 */
#include "project.h"
#include <stdbool.h>
#include <ctype.h>

volatile bool irq_flag = false;

// Executed when the IRQ pin triggers an interrupt
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable;
    
    const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    
    nRF24_start();
    nRF24_setRxPipe0Address(RX_ADDR, 5);

    while (1) {
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt
            NrfIRQ flag = nRF24_getIRQFlag();
            nRF24_clearIRQFlag(flag);
            
            // here we will store the received data
            unsigned char data;
            
            // How many bytes are in the pipe0 (data received)?
            uint8_t payload_size = nRF24_getPayloadSize(NRF_DATA_PIPE0);
            
            // get the data from the transmitter
            nRF24_getRxPayload(&data, payload_size);
            
            // turn received data into uppercase
            data = toupper(data);

            // Send the ACK + data via pipe0
            nRF24_rxWritePayload(NRF_DATA_PIPE0, &data, 1);
            
            irq_flag = false;
        }
        
    }
}

/* [] END OF FILE */
