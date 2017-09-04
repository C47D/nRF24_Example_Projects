/*
 * 01_Echo_Tx
 * 
 * This project is the transmitter side of the Echo project,
 * The nrf24 radio is configurd to transmit 1 byte and receive the same byte via
 * ACK with payload from the receiver radio.
 * The data sent is the char received with the UART component, when the receiver
 * radio send the char back, the transmitter radio will print it on the UART.
 */
#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;
volatile bool uart_flag = false;

volatile unsigned char data;

// This handler is executed when we get a byte via UART
CY_ISR(UART_Handler)
{
    uart_flag = true;
    data = UART_GetChar();
    isr_UART_ClearPending();
}

// This handler is executed when we get an interrupt from
// the IRQ pin of the radio
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable;
    
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    
    nRF24_start();
    nRF24_setTxAddress(TX_ADDR, 5);

    while (1) {
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt
            NrfIRQ flag = nRF24_getIRQFlag();
            nRF24_clearIRQFlag(flag);
            
            // Get the data sent from the receiver
            unsigned char received;
            nRF24_getRxPayload(&received, 1);
            
            // print the received char
            UART_PutChar(received);
            
            irq_flag = false;
        }
        
        if (true == uart_flag) {
            
            // Transmit the data from UART RX to nrf24 receiver
            nRF24_PTX_Transmit(&data, 1);
            
            uart_flag = false;
        }
        
    }
}

/* [] END OF FILE */
