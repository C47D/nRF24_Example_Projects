# nRF24 Component Example Projects

The nRF24 Component is added to this repository as git submodule.

# Projects

## 00 Basic (Rx/Tx)
In this project the Tx radio will get 1 char from the UART and send it to the Rx radio.

The Rx radio will get the data and print it via UART. also will ACK the packet.

Rx radio: CY8CKIT-041-40xx (PSoC4000S)

Tx radio: CY8CKIT-042 (PSoC4200)

## 01 Echo (Rx/Tx)
In this project the Tx radio will send 1 byte to the Rx radio, the sent byte
is a char received via UART Rx.

The Rx radio will ACK + return the received byte (turned into CAPS), the Tx radio will receive the payload from the Rx radio and print it via UART.

Rx radio: CY8CKIT-059 (PSoC5LP)

Tx radio: CY8CKIT-042 (PSoC4200)
