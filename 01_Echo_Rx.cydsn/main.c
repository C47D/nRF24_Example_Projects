/*
 * 01_Echo_Rx
 * 
 * The nrf24 radio is configured to send back ACK + 1 byte payload.
 * The payload byte is the last received char turned into uppercase.
 */
#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;

// Executed when the IRQ pin triggers an interrupt
CY_ISR_PROTO(IRQ_Handler);
CY_ISR_PROTO(SW_Handler);

int main(void)
{
    const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    // here we will store the received data
    unsigned char data = ' ';
    uint8_t count = 0;
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    isr_SW_StartEx(SW_Handler);
    
    CyGlobalIntEnable;
    
    nRF24_start();
    // Place 'count' in the TX FIFO to send it on the ACK packet
    nRF24_rxWritePayload(NRF_PIPE0, &count, 1);
    
    nRF24_setRxPipe0Address(RX_ADDR, 5);
    nRF24_startListening();

    while (1) {
        
        // waiting to get data
        while(false == irq_flag);
        
        // We got data, so we clear the interrupt flag
        NrfIRQ flag = nRF24_getIRQFlag();
        nRF24_clearIRQFlag(flag);
                
        // How many bytes did we received?
        //uint8_t payload_size = nRF24_getPayloadSize(NRF_PIPE0_PAYLOAD_SIZE);
        
        // get the data and store it into 'data'
        //nRF24_getRxPayload(&data, payload_size);
        nRF24_getRxPayload(&data, 1);
        
        // increment 'count' and put it on the TX FIFO
        count++;
        nRF24_rxWritePayload(NRF_PIPE0, &count, 1);
        
        irq_flag = false;
        
    }
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    LED_Write(~LED_Read());
    IRQ_ClearInterrupt();
}

CY_ISR(SW_Handler)
{
    LED_Write(~LED_Read());
    SW_ClearInterrupt();
}

/* [] END OF FILE */
