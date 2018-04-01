# nRF24 Example Projects

This repository contain multiple example projects using the nRF24 component for PSoC devices.
The nRF24 component is developed in a separated repository, you can see it here: [nRF24_Component](https://github.com/C47D/nRF24_Component).

This projects are developed using PSoC Creator 4.1 and multiple PSoC kits, PSoC 4200 devices, PSoC 4S, PSoC 4M and PSoC 5LP devices.


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

For details each project directory have it's own README, the following list is just an overview.

## 00 Basic (Working)
In this project the Tx radio will get 1 char from the UART and send it to the Rx radio, the Rx radio will get the data and print it via UART.

Rx: CY8CKIT-041-40xx (PSoC4000S)
Tx: CY8CKIT-042 (PSoC4200)

## 01 Echo (Working)
In this project the Tx radio will get 1 byte from the UART and send it to the Rx radio, the Rx radio will print the received data via UART and reply back with an ACK packet containing the count of received packets, the Tx radio will receive the ACK packet from the Rx radio and print it via UART.

PSoC Rx: CY8CKIT-059 (PSoC5LP)
PSoC Tx: CY8CKIT-042 (PSoC4200)

```
                PSoC Tx            nRF24 Tx                   nRF24 Rx             PSoC Rx
              +---------+  MOSI  +----------+               +-----------+  MOSI  +----------+
   UART       |         +------->+          |               |           <--------+          |
+--------+ TX |         |  MISO  |          |               |           |  MISO  |          |
|        +---->         <--------+          |     DATA      |           +-------->          |
+--------+    |         |  SCLK  |          |  +--------->  |           |  SCLK  |          |
              |         +------->+          |               |           <--------+          |
              |         |  /SS   |          |               |           |  /SS   |          |
              |         +------->+          |     COUNT     |           <--------+          |
              |         |  CE    |          |  <----------  |           |  CE    |          |
              |         +------->+          |               |           <--------+          |
              |         |  IRQ   |          |               |           |  IRQ   |          |
              |         +--------+          |               |           +-------->          |
              +---------+        +----------+               +-----------+        +----------+
```

## More examples to come...
