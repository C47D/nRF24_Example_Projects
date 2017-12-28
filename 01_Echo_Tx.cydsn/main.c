/*
 * 01_Echo_Tx
 * 
 * The nrf24 radio is configured to transmit 1 byte and receive the same byte via
 * ACK + payload from the Rx radio.
 * The data sent is the char received with the UART Rx, when the receiver
 * radio send reply with payload, the transmitter radio will print it on the UART.
 * Note that acknowledgement payloads must be preloaded; it is not possible for
 * a transmitter to send a command and receive a direct response to that command
 * in the acknowledgement. Instead, the response would have to be preloaded so it
 * could be sent as the acknowledgement for the next command that is received.
 * Preloaded acknowledgement payloads are required in order to guarantee that 
 * acknowledgement timing is deterministic.
 */
#include "project.h"
#include <stdbool.h>
#include <stdio.h>

volatile bool irq_flag = false;

// the IRQ pin triggered an interrupt
CY_ISR_PROTO(IRQ_Handler);
void print_status(void);

int main(void)
{
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    unsigned char data;
    unsigned char received;
    
    // Set the IRQ interrupt handler
    isr_IRQ_StartEx(IRQ_Handler);
    
    CyGlobalIntEnable;
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Echo test project\r\n");
    
    nRF24_start();
    nRF24_set_rx_pipe_0_address(TX_ADDR, 5);
    nRF24_set_tx_address(TX_ADDR, 5);

    while (1) {
        
        UART_PutString("Type a letter...");
        // wait until we get a letter to send
        while(0 == UART_GetRxBufferSize());
        // get the letter
        data = UART_GetChar();
        
        // transmitting the letter
        UART_PutString("\r\nSending letter ");
        UART_PutChar(data);
        UART_PutString("\r\n");
        nRF24_transmit(&data, 1);
        
        while(false == irq_flag);
        
        // TX_DT IRQ is asserted when the ACK packet is received by the PTX.
        nRF24_clear_all_irqs();
        
        // wait until we get the ACK+payload
        while(nRF24_is_rx_fifo_empty());
        
        UART_PutString("\r\nData received: ");
        // Get the data sent from the Rx
        nRF24_get_rx_payload(&received, 1);
        
        // print the received char
        char received_str[10];
        sprintf(received_str, "%d\r\n", received);
        UART_PutString(received_str);
        
        CyDelay(500);
            
        irq_flag = false;
        
    }
}

void print_status(void)
{
    char array[10];
    uint8_t sts = nRF24_get_status();
    UART_PutString("Status: 0x");
    sprintf(array, "%02X\r\n", sts);
    UART_PutString(array);
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
