# Basic Rx

This is the Radio receiver (Rx) part of the Basic project.
The nRF24 radio is configured as follows:




Once the API is generated you have all the nRF24 component functions
available, in this project we use the most basic features of the nRF24 radio.
All the application is written on the main.c file.

We will use the IRQ pin of the nRF24 to trigger an interrupt when the nRF24
radio gets an event, in this case the only interrupt enabled is the Rx
interrupt.

Assuming you already used PSoC before you should know that when using the
interrupt component we can specify the function that will be executed when
the interrupt is triggered, in this case the function to be executed is named
```C
IRQ_Handler 
```
and we configure it as follows:

```C
isr_IRQ_StartEx(IRQ_Handler)
```

Next we start the UART component:

```C
UART_Start();
UART_PutChar(0x0C); // This command will clear the terminal (i'm using Putty)
UART_PutString("Basic project, Rx\r\n");
```

Now we are going to configure the nRF24 radio, first we start the component, this will load the configuration from the customizer and send it to the radio, next we configure the address of the pipe0 and finally we set the nRF24 to start listening.

The address of the pipe0 is 5 bytes long (last parameter of the nRF24_set_rx_pipe_address function) and the address is equal to the RX_ADDR array, remember that the address is written to the nRF24 radio starting with the LSB (Least Significant Byte).

```C
const uint8_t RX_ADDR[5] = {0xBA, 0xAD, 0xC0, 0xFF, 0xEE};
nRF24_start();
nRF24_set_rx_pipe_address(NRF_ADDR_PIPE0, RX_ADDR, 5);
nRF24_start_listening();
```

On the while loop we wait for the irq_flag to become true, then we get ad clear the interrupt flag of the nRF24 radio and toggle the LED, then we get the data received and write it over UART.

```C
nrf_irq flag = nRF24_get_irq_flag(); // get the flag
nRF24_clear_irq_flag(flag); // clear it!

nRF24_get_rx_payload(&data, 1); // get one byte of payload and write it into the data variable

// send data via UART
UART_PutString("Received: ");
UART_PutChar(data);
UART_PutCRLF();
```
