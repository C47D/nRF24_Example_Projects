/*
 * 00_Basic_Rx
 * 
 * The nrf24 is configured as Rx, it will print the received data via UART.
 */

#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;

// here we will store the received data
char data;

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
    
    UART_Start();
    UART_PutChar(0x0c);
    UART_PutString("Basic project, Rx\r\n");
    
    nRF24_start();
    nRF24_setRxPipe0Address(RX_ADDR, 5);
    nRF24_setTxAddress(RX_ADDR, 5);
    nRF24_startListening();
    
    while (1) {
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt,
            // in this project the only IRQ cause is the caused by
            // receiving data (NRF_STATUS_RX_DR_MASK)
            NrfIRQ flag = nRF24_getIRQFlag();
            nRF24_clearIRQFlag(flag);
            
            LED_Write(~LED_Read());
            
            // How many bytes are in the pipe0 (data received)?
            uint8_t payload_size = nRF24_getPayloadSize(NRF_DATA_PIPE0);
            
            // get the data from the transmitter
            nRF24_getRxPayload(&data, payload_size);
            
            // send data via UART
            UART_PutChar(data);
            
            irq_flag = false;
        }
        
    }
}

/* [] END OF FILE */
