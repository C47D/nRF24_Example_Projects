# Echo Rx

This is the Radio receiver (Rx) part of the Echo project.
The nRF24 radio is configured as follows:




Once the API is generated you have all the nRF24 component functions
available, in this project we use the nRF24 ACK+payload feature, this way we can send data to the Transmitter radio togheter with the ACK packet (see nRF24 datasheet for more information).

All the application is written on the main.c file.

## main.c

The rx radio will have a 5 byte address:

```C
const uint8_t RX_ADDR[5]= {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
```

Also we need a variable that will store the received data and a variable with
the count of the number of times we have received data.
```C
unsigned char data = ' ';   // here we will store the received data
uint8_t count = 0;			// here we will count the received packets
```

We will use the IRQ pin of the nRF24 to trigger an interrupt when the nRF24
radio gets an event, in this case the only interrupt enabled is the Rx
interrupt. This is configured as follows:

```C
isr_IRQ_StartEx(IRQ_Handler); // set the irq interrupt handler
    
CyGlobalIntEnable; // enable interrupts
```

When we start the component it will load the configuration from the customizer and send it to the radio, next we configure the address of the pipe0, we also need to set the tx address of the radio (because it will send data back to the Transmitter radio) and finally we set the nRF24 to start listening.

The address of the pipe0 is 5 bytes long (last parameter of the nRF24_set_rx_pipe_address function) and the address is equal to the RX_ADDR array, remember that the address is written to the nRF24 radio starting with the LSB (Least Significant Byte).

```C
nRF24_start();
// Uploading Payload for ACK packet
nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
    
nRF24_set_rx_pipe_address(NRF_ADDR_PIPE0, RX_ADDR, 5);
nRF24_set_tx_address(RX_ADDR, 5);
nRF24_start_listening();
```

On the while loop we wait for the irq_flag to become true, then we clear the interrupt flag of the nRF24 radio and store the data received into the data variable, then we increment the count variable and place it on the pipe0 fifo.
You should notice that the data we place on the pipe0 fifo is send on the next time we get data from the transmitter radio (that's why i used the nRF24_rx_write_payload function just after starting the component).

```C
// waiting to get data
while(false == irq_flag);
        
// The RX_DR IRQ is asserted after the packet is received by the PRX
nRF24_clear_irq_flag(NRF_RX_DR_IRQ);
        
// Get the data
nRF24_get_rx_payload(&data, 1);
        
// increment 'count' and put it on the TX FIFO for the next ACK packet
count++;
nRF24_rx_write_payload(NRF_PIPE0, &count, 1);
        
irq_flag = false;
```

The IRQ_Handler function only set the irq_flag variable to true and toggles an LED.
```C
CY_ISR(IRQ_Handler)
{
    irq_flag = true;
    LED_Write(~LED_Read());
    IRQ_ClearInterrupt();
}
```
