/*
 * 01_Basic_Tx
 * 
 * The nrf24 radio is configured to transmit 1 byte payload.
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
    unsigned char data = ' ';
    
    // Set the interrupts handlers
    isr_IRQ_StartEx(IRQ_Handler);
    
    CyGlobalIntEnable;
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Basic project: Tx\r\n");
    
    nRF24_start();
    nRF24_set_rx_pipe_0_address(TX_ADDR, 5);
    // set tx pipe address to match the receiver address
    nRF24_set_tx_address(TX_ADDR, 5);

    while (1) {
    
        UART_PutChar(0x0C);
        UART_PutString("Type a letter...");
        // wait until we get a letter to send
        while(0 == UART_GetRxBufferSize());
        // get the letter
        data = UART_GetChar();
        
        print_status();
        UART_PutString("\r\nSending letter ");
        UART_PutCRLF(data);
        nRF24_transmit(&data, 1);
        
        print_status();
        while(false == irq_flag);
            
        // Get and clear the flag that caused the IRQ interrupt,
        // in this project the only IRQ cause is the caused by
        // transmitting data (NRF_STATUS_TX_DS_MASK) or timeout
        nrf_irq flag = nRF24_get_irq_flag();
        
        switch (flag) {
        case NRF_TX_DS_IRQ:
            // turn on the Green LED if the transmit was successfull
            LED_Write(0);
            break;
        case NRF_MAX_RT_IRQ:
            // turn off the Green LED if the transmit was not successfull
            LED_Write(1);
            break;
        default:
            break;
        }
        
        nRF24_clear_irq_flag(flag);
        
        irq_flag = false;
        
        CyDelay(250);
        
    }
}

void print_status(void)
{
    char status_str[20];
    uint8_t status = nRF24_get_status();
    sprintf(status_str, "0x%2X\r\n", status);
    UART_PutString(status_str);
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
