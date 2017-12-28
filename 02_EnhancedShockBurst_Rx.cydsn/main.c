/*
 * 02_EnhancedShockBurst_Tx
 *
 * Configuration based on Appendix A of the nRF24 datasheet.
 *
 * 1. Set PRIM_RX high, all data pipes that receive data must be enabled (EN_RXADDR),
 *    enable auto ack for all pipes running EN_AA, and set the correct payload widths
 *    (RX_PW_Px). Set up addresses.
 * 2. Start Active Rx mode by setting CE high.
 * 3. After 130us nRF24 monitors the air for incomming communication.
 * 4. When a valid packet is received, the payload is stored in the RX FIFO, and
 *    the RX_DR bit in STATUS register is set high. RX_P_NO in STATUS register
 *    indicates what data pipe the payload has been received in.
 * 5. If auto-ack is enabled, an ACK packet is transmitted back, unless the NO_ACK
 *    bit is set in the received packet. If there is a payload in the TX_PLD FIFO,
 *    this payload is added to the ACK packet.
 * 6. MCU sets the CE pin low to enter standby-I mode
 * 7. MCU can clock out the payload data at a suitable rate through the SPI.
 * 8. nRF24 is now ready for entering TX or RX mode or power down mode.
 */

#include "project.h"
#include <stdbool.h>

volatile bool irq_flag = false;

// here we will store the received data
unsigned char data;

// Executed when the IRQ pin triggers an interrupt
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    IRQ_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable;
    
    const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    
    // Set the Handler for the IRQ interrupt
    isr_IRQ_StartEx(IRQ_Handler);
    
    UART_Start();
    UART_PutChar(0x0c);
    UART_PutString("Basic project, Rx\r\n");
    
    nRF24_start();
    nRF24_set_rx_pipe_0_address(RX_ADDR, 5);
    nRF24_set_tx_address(RX_ADDR, 5);
    nRF24_start_listening();
    
    while (1) {
        
        if (true == irq_flag) {
            
            // Get and clear the flag that caused the IRQ interrupt,
            // in this project the only IRQ cause is the caused by
            // receiving data (NRF_STATUS_RX_DR_MASK)
            nrf_irq flag = nRF24_get_irq_flag();
            nRF24_clear_irq_flag(flag);
            
            LED_Write(~LED_Read());
            
            // How many bytes are in the pipe0 (data received)?
            uint8_t payload_size = nRF24_get_payload_size(NRF_PIPE0_PAYLOAD_SIZE);
            
            // get the data from the transmitter
            nRF24_get_rx_payload(&data, payload_size);
            
            // send data via UART
            UART_PutChar(data);
            
            irq_flag = false;
        }
        
    }
}

/* [] END OF FILE */
