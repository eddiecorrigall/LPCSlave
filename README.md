LPC
===

State machine IO driver for the Low-Pin Count bus slave. The source also includes a message level protocol I designed to transmit data over LPC between a master and a slave, see lpc_io_transmission.c for details.

## Instructions

The current project was used in a prototype I worked on for Pitney Bowes. For your application, you will need to modify the code.

  1. Set up an interupt service routine (ISR) for on your embedded device, then modify and invoke the GPIO_ISR function in gpio.c.
  2. When your device is booting ensure a call to GPIO_Initialize/LPC_Initialize is made.

## Literature

http://www.intel.com/design/chipsets/industry/lpc.htm
