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

volatile bool irq_flag = false;

// the IRQ pin triggered an interrupt
CY_ISR_PROTO(IRQ_Handler);

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
    nRF24_setRxPipe0Address(TX_ADDR, 5);
    nRF24_setTxAddress(TX_ADDR, 5);

    while (1) {
        
        UART_PutChar(0x0C);
        UART_PutString("Type a letter...");
        // wait until we get a letter to send
        while(0 == UART_GetRxBufferSize());
        // get the letter
        data = UART_GetChar();
        
        // transmitting the letter
        UART_PutString("\r\nSending letter ");
        UART_PutChar(data);
        UART_PutString("\r\n");
        nRF24_PTX_Transmit(&data, 1);
        
        while(false == irq_flag);

        LED_Write(~LED_Read());
        
        // TX_DT interrupt is triggered when receiving the ACK + payload
        NrfIRQ flag = nRF24_getIRQFlag();
        nRF24_clearIRQFlag(flag);

        UART_PutString("\r\nData received: ");
        // Get the data sent from the Rx
        nRF24_getRxPayload(&received, 1);
        // print the received char
        UART_PutChar(received);
        UART_PutString("\r\n");
        
        CyDelay(2500);
            
        irq_flag = false;
        
    }
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
