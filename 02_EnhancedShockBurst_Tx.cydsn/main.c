/*
 * 02_EnhancedShockBurst_Tx
 *
 * Configuration based on Appendix A of the nRF24 datasheet.
 *
 * 1. Set PRIM_RX low
 * 2. Clock the address for the receiving node (TX_ADDR) and payload data (TX_PLD)
 *    if the PTX device shall receive ACK, configure data pipe 0 to receive the
 *    ACK packet. The RX address for data pipe 0 (RX_ADDR_P0) must be equal to the
 *    TX address (TX_ADDR) in the PTX device.
 * 3. A high pulse on CE starts the transmission. The minimum pulse width on CE is 10us.
 * 4. nRF24L01+ ShockBurst:
 *    - Radio is powered up
 *    - 16MHz internal clock is started
 *    - RF packet is completed
 *    - Data is transmitted at high speed (1Mbps or 2Mbps)
 * 5. If auto ack is activated (ENAA_P0 = 1) the radio goes into RX mode immediately,
 *    unless the NO_ACK bit is set in the received packet.
 */
#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;
volatile bool transfer_flag = false;

// the IRQ pin triggered an interrupt
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

CY_ISR(TIMER_Handler)
{
    transfer_flag = true;
    Timer_ReadStatusRegister();
    isr_TIMER_ClearPending();
}

int main(void)
{
    CyGlobalIntEnable;
    
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    const uint8_t DATA[3]= {0xAC, 0xCE, 0x55};
    uint8_t i = 0;
    
    // Set the interrupts handlers
    isr_IRQ_StartEx(IRQ_Handler);
    isr_TIMER_StartEx(TIMER_Handler);
    
    UART_Start();
    UART_PutChar(0x0C);
    
    nRF24_start();
    
    // set pipe0 addr to get the ACK from the receiver
    nRF24_set_rx_pipe_0_address(TX_ADDR, 5);
    // set tx pipe address to match the receiver address
    nRF24_set_tx_address(TX_ADDR, 5);
    
    Timer_Start();

    while (1) {
        
        if (true == transfer_flag) {
            nRF24_transmit(&DATA[i], 1);
            i++;
            if (3 == i) {
                i = 0;
            }
            transfer_flag = false;
        }
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt,
            // in this project the only IRQ cause is the caused by
            // transmitting data (NRF_STATUS_TX_DS_MASK)
            nrf_irq flag = nRF24_get_irq_flag();
            nRF24_clear_irq_flag(flag);
            
            //UART_PutChar(0x0C);
            switch (flag) {
            case NRF_TX_DS_IRQ:
                LED_Write(0);
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
            
            irq_flag = false;
        }
        
    }
}

/* [] END OF FILE */
