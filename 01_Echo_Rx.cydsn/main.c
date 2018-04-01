/*
 * 01_Echo_Rx
 * 
 * The nrf24 radio is configured to send back ACK + 1 byte payload.
 * The payload byte is the cound of received data packets.
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
    // Uploading Payload for ACK packet
    nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
    
    nRF24_set_rx_pipe_address(NRF_ADDR_PIPE0, RX_ADDR, 5);
    nRF24_set_tx_address(RX_ADDR, 5);
    nRF24_start_listening();

    while (1) {
        
        // waiting to get data
        while(false == irq_flag);
        
        // The RX_DR IRQ is asserted after the packet is received by the PRX
        nRF24_clear_irq_flag(NRF_RX_DR_IRQ);
        
        // Get the data
        nRF24_get_rx_payload(&data, 1);
        
        // increment 'count' and put it on the TX FIFO for the next ACK packet
        count++;
        nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
        
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
