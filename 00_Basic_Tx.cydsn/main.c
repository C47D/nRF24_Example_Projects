/*
 * 01_Basic_Tx
 * 
 * The nrf24 radio is configured to transmit 1 byte payload.
 *
 * nRF24 Setup:
 * POWER_ON_RESET delay
 * EN_AA = Enable auto-ack (RX_ADDR0_P0 == TX_ADDR)
 * EN_RXADDR = Enable data pipes
 * SETUP_AW = Width of address
 * RF_CH = Channel
 * RF_SETUP = Power mode and data speed
 * RX_ADDR_P0 = set receiver address
 * TX_ADDR = Tx address
 * RX_PW_P0 = payload width setup
 * CONFIG = Set up the radio, pwr_up, rx or tx and interrupts
 * delay to reach standby mode
 */
#include "project.h"
#include <stdbool.h>
#include <stdio.h>

volatile bool irq_flag = false;

volatile char data;

// the IRQ pin triggered an interrupt
CY_ISR_PROTO(IRQ_Handler);
void print_status(void);

int main(void)
{
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the interrupts handlers
    isr_IRQ_StartEx(IRQ_Handler);
    
    CyGlobalIntEnable;
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Basic project: Tx\r\n");
    
    nRF24_start();
    nRF24_setRxPipe0Address(TX_ADDR, 5);
    // set tx pipe address to match the receiver address
    nRF24_setTxAddress(TX_ADDR, 5);

    while (1) {
    
        UART_PutChar(0x0C);
        UART_PutString("Type a letter...");
        // wait until we get a letter to send
        while(0 == UART_GetRxBufferSize());
        // get the letter
        data = UART_GetChar();
        UART_PutString("\r\n");
        
        // transmitting the letter
        UART_PutString("\nSending letter ");
        UART_PutChar(data);
        UART_PutString("\r\n");
        nRF24_PTX_Transmit(&data, 1);
        
        while(false == irq_flag);
            
        // Get and clear the flag that caused the IRQ interrupt,
        // in this project the only IRQ cause is the caused by
        // transmitting data (NRF_STATUS_TX_DS_MASK) or timeout
        NrfIRQ flag = nRF24_getIRQFlag();
        
        switch (flag) {
        case NRF_TX_DS_IRQ:
            LED_Write(0);
            break;
        case NRF_MAX_RT_IRQ:
            LED_Write(1);
            break;
        default:
            break;
        }
        
        nRF24_clearIRQFlag(flag);
        
        // flush the tx fifo
        nRF24_flushTxCmd();
        
        irq_flag = false;
        
        CyDelay(250);
        
    }
}

void print_status(void)
{
    char status_str[20];
    uint8_t status = nRF24_getStatus();
    sprintf(status_str, "0x%2X\r\n", status);
    UART_PutString(status_str);
}

CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

/* [] END OF FILE */
