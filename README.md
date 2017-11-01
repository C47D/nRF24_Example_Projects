# nRF24 Example Projects

This repoitory contain multiple example projects using the nRF24 component developed for PSoC devices.
The nRF24 component is developed in a separated repository, you can see it here: ![nRF24_Component](https://github.com/C47D/nRF24_Component).
This projects are developed using PSoC Creator 4.1.


To clone this repo:
```
$ git clone https://github.com/C47D/nRF24_Example_Projects
$ cd nRF24_Example_Projects
nRF24_Example_Projects $ git submodule init
nRF24_Example_Projects $ git submodule update
```

or

```
$ git clone --recursive https://github.com/C47D/nRF24_Example_Projects
```

To update the submodule:
```
nRF24_Example_Projects $ git submodule update --remote nRF24_Component
```

# Projects

## 00 Basic (Working)
In this project the Tx radio will get 1 char from the UART and send it to the Rx radio.

The Rx radio will get the data and print it via UART.

Rx radio: CY8CKIT-041-40xx (PSoC4000S)

Tx radio: CY8CKIT-042 (PSoC4200)

## 01 Echo (Work in progress)
In this project the Tx radio will send 1 byte to the Rx radio, the sent byte
is a char received via UART Rx.

The Rx radio will ACK + return the received byte (turned into CAPS), the Tx radio will receive the payload from the Rx radio and print it via UART.

Rx radio: CY8CKIT-059 (PSoC5LP)

Tx radio: CY8CKIT-042 (PSoC4200)
