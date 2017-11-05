## 01 Echo Tx
Kit used: CY8CKIT-042 (PSoC4200)

For this program we use the following features:
- The ability to get the ACK+payload from the Rx radio. For this we need to enable Dynamic Payload.

The following images show the Tx nRF24 component configuration.

// TODO: Add images

The Tx radio will ask the user for a letter, this letter is sent (as 1 byte) to the Rx radio.

The Rx radio will "reply" with an ACK + 1 byte of payload, the payload byte is the count of times it received something from the Tx radio.
