# nRF24 Component Example Projects

The nRF24 Component is added to this repository as git submodule.

# Projects

## 01 Echo (Rx/Tx)
In this project the Tx radio will send 1 byte to the Rx radio, the sent byte
is a char received via UART Rx.
The Rx radio will ACK + return the received byte (turned into CAPS), the Tx radio will receive the payload from the Rx radio and print it via UART.
Rx radio: CY8CKIT-059 (PSoC5LP)
Tx radio: CY8CKIT-042 (PSoC4200)
