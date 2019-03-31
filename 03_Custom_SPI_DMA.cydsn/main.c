#include <stdbool.h>

#include "project.h"

static bool is_bit_set(nrf_register reg, uint8_t bit_pos);

static const uint8_t DMA_REQUEST_PER_BURST = 1;
static const uint8_t DMA_BYTES_PER_BURST = 1;
static uint8_t DMA_Rx_Chan = 0;
static uint8_t DMA_Tx_Chan = 0;
static uint8_t DMA_Rx_TD = 0;
static uint8_t DMA_Tx_TD = 0;

static void config_dma(void);

void irq_handler(void)
{
    UART_PutString("IRQ\r\n");
    IRQ_ClearInterrupt();
}

void custom(const uint8_t *in, uint8_t *out, const size_t xfer_size)
{
    nrf_spi_ClearFIFO();
    LED_Write(~LED_Read());
    
    CyDmaTdSetConfiguration(DMA_Rx_TD, xfer_size, CY_DMA_DISABLE_TD, CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_Tx_TD, xfer_size, CY_DMA_DISABLE_TD, CY_DMA_TD_INC_SRC_ADR);
    
    CyDmaTdSetAddress(DMA_Rx_TD, LO16((uint32_t) nrf_spi_RXDATA_PTR), LO16((uint32_t) out));
    CyDmaTdSetAddress(DMA_Tx_TD, LO16((uint32_t) in), LO16((uint32_t) nrf_spi_TXDATA_PTR));

    SS_Write(0);
    
    // enable DMAs
    CyDmaChEnable(DMA_Rx_Chan, 1);
    CyDmaChEnable(DMA_Tx_Chan, 1);
    
    while(!(nrf_spi_ReadTxStatus() & nrf_spi_STS_SPI_DONE)) {
    }
    
    SS_Write(1);
}

int main(void)
{    
    const uint8_t TX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
    const uint8_t DATA[3] = {0xAC, 0xCE, 0x55};
    
    isr_IRQ_StartEx(irq_handler);
    
    CyGlobalIntEnable;
    
    config_dma();

    UART_Start();
    UART_PutChar(0x0C);
    UART_PutString("Basic project, Rx\r\n");

    nRF24_register_spi_xfer(custom);
    nRF24_start();
    
    nRF24_set_rx_pipe_address(NRF_ADDR_PIPE0, TX_ADDR, 5);
    nRF24_set_tx_address(TX_ADDR, 5);
    
    uint8_t addr[5] = {0};
    nRF24_get_tx_address(addr, sizeof addr);
    
    for (size_t i = 0; i < sizeof addr; i++) {
        UART_PutString("[");
        UART_PutHexByte(addr[i]);
        UART_PutString("] ");
    }
    UART_PutCRLF();
    
    // nRF24_register_spi_xfer(nRF24_default_spi_xfer);
    
    size_t iterator = 0;
    while (1) {
        
        volatile uint8_t sts = nRF24_get_status();
        UART_PutString("Status: 0x");
        UART_PutHexByte(sts);
        UART_PutCRLF();

        if (is_bit_set(NRF_REG_FIFO_STATUS, NRF_FIFO_STATUS_BIT_TX_FULL)) {
            UART_PutString("Flushing TX FIFO\r\n");
            nRF24_cmd_flush_tx();
        }
        
        if (is_bit_set(NRF_REG_STATUS, NRF_STATUS_BIT_MAX_RT)) {
            UART_PutString("Clearing MAX_RT\r\n");
            nRF24_clear_irq_flag(NRF_MAX_RT_IRQ);
        }
        
        sts = nRF24_get_status();
        UART_PutString("Status: 0x");
        UART_PutHexByte(sts);
        UART_PutCRLF();
        
        CyDelay(500);
        
        nRF24_transmit(&DATA[iterator], 1);
        iterator++;
        
        if (sizeof DATA <= iterator) {
            iterator = 0;
        }
        
        CyDelay(500);
    }
}

static bool is_bit_set(nrf_register reg, uint8_t bit_pos)
{
    return true == nRF24_read_bit(reg, bit_pos);
}

static void config_dma(void)
{
    // Allocate DMA channels
    DMA_Rx_Chan = DMA_rx_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST,
            HI16(CYDEV_PERIPH_BASE), HI16(CYDEV_SRAM_BASE));
    
    DMA_Tx_Chan = DMA_tx_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST,
            HI16(CYDEV_SRAM_BASE), HI16(CYDEV_PERIPH_BASE));    
    
    // Allocate TDs
    DMA_Rx_TD = CyDmaTdAllocate();
    DMA_Tx_TD = CyDmaTdAllocate();
    
    CyDmaChSetInitialTd(DMA_Rx_Chan, DMA_Rx_TD);
    CyDmaChSetInitialTd(DMA_Tx_Chan, DMA_Tx_TD);
}

/* [] END OF FILE */
