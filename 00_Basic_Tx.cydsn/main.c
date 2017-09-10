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

volatile bool irq_flag = false;
volatile bool uart_flag = false;

// the IRQ pin triggered an interrupt
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable;
    
    const unsigned char *mesg= "Hola nRF24!";
    uint8_t i = 0;
    
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the interrupts handlers
    isr_IRQ_StartEx(IRQ_Handler);
    
    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Basic project: Tx\r\n");
    
    nRF24_start();
    
    // set pipe0 addr to get the ACK from the receiver
    nRF24_setRxPipe0Address(TX_ADDR, 5);
    // set tx pipe address to match the receiver address
    nRF24_setTxAddress(TX_ADDR, 5);

    while (1) {
        
        CyDelay(250);
        UART_PutString("Sending ");
        UART_PutChar(mesg[i]);
        UART_PutString("\r\n");
        nRF24_PTX_Transmit(&mesg[i], 1);
        i++;
        if (i == 12) {
            i = 0;
        }
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt,
            // in this project the only IRQ cause is the caused by
            // transmitting data (NRF_STATUS_TX_DS_MASK)
            NrfIRQ flag = nRF24_getIRQFlag();
            nRF24_clearIRQFlag(flag);
            
            LED_Write(~LED_Read());
            
            //UART_PutChar(0x0C);
            switch (flag) {
            case NRF_TX_DS_IRQ:
                UART_PutString("Data sent!");
                break;
            case NRF_RX_DR_IRQ:
                UART_PutString("Data received!");
                break;
            case NRF_MAX_RT_IRQ:
                UART_PutString("Timeout!");
                break;
            default:
                break;
            }
            UART_PutString("\r\n");
            
            UART_PutString("Status: ");
            uint8_t sts = nRF24_NOPCmd();
            UART_PutChar(sts);
            
            if (flag == NRF_TX_DS_IRQ) {

            } else if (flag)
            
            irq_flag = false;
        }
        
    }
}

/* [] END OF FILE */
