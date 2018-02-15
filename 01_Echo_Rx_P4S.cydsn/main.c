/*
 * 00_Basic_Rx
 * 
 * The nrf24 is configured as Rx, it will print the received data via UART.
 */

#include "project.h"
#include <stdbool.h>
#include <stdio.h>

volatile bool irq_flag = false;

// here we will store the received data
unsigned char data;

// Executed when the IRQ pin triggers an interrupt
CY_ISR_PROTO(IRQ_Handler);
void print_status(void);

int main(void)
{
    const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // here we will store the received data
    unsigned char data = ' ';
    uint8_t count = 0;
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    
    CyGlobalIntEnable;
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Echo project, Rx\r\n");
    
    nRF24_start();
    
    nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
    
    nRF24_set_rx_pipe_address(NRF_ADDR_PIPE0, RX_ADDR, 5);
    nRF24_set_tx_address(RX_ADDR, 5);
    
    nRF24_start_listening();
    
    while (1) {
        
        UART_PutString("Waiting for data...\r\n");
        
        while(false == irq_flag);
        LED_Write(~LED_Read());
        irq_flag = false;
        
        // "Safe" zone
        nRF24_stop_listening();

        nRF24_clear_all_irqs();
        
        // get the data from the transmitter
        nRF24_get_rx_payload(&data, 1);
        // put count into the TX FIFO
        count++;
        nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
        
        // end of Safe zone
        nRF24_start_listening();
        
        UART_PutString("Received: ");
        UART_PutChar(data);
        UART_PutCRLF();

    }
}

void print_status(void)
{
    char array[10];
    uint8_t sts = nRF24_get_status();
    UART_PutString("Status: 0x");
    sprintf(array, "%02X\r\n", sts);
    UART_PutString(array);
    UART_PutCRLF();
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
