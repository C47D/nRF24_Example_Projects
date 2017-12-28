/*
 * 00_Basic_Rx
 * 
 * The nrf24 is configured as Rx, it will print the received data via UART.
 */

#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;

// here we will store the received data
unsigned char data;

// Executed when the IRQ pin triggers an interrupt
CY_ISR_PROTO(IRQ_Handler);

int main(void)
{
    const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    
    CyGlobalIntEnable;
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Basic project, Rx\r\n");
    
    nRF24_start();
    nRF24_set_rx_pipe_0_address(RX_ADDR, 5);
    nRF24_start_listening();
    
    while (1) {
        
        UART_PutString("Waiting for data...\r\n");
        
        while(false == irq_flag);
            
        // Get and clear the flag that caused the IRQ interrupt,
        nrf_irq flag = nRF24_get_irq_flag();
        nRF24_clear_irq_flag(flag);
            
        LED_Write(~LED_Read());
            
        // get the data from the transmitter
        nRF24_get_rx_payload(&data, 1);
            
        // send data via UART
        UART_PutString("Received: ");
        UART_PutChar(data);
        UART_PutCRLF();
            
        irq_flag = false;
    }
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
