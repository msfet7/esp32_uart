# UART usage example
## Description
Code demonstrates an example usage of UART in esp32 microcontroller.

## Hardware
The connections are showed at the diagram below:
```
ESP32      Hardware      Config
---------------------------------
PIN14      RED_LED       OUTPUT
PIN26      YELLOW_LED    OUTPUT
```

## Working principle
We have two tasks. The firdt one is responsible for handling UART communication and the second one 
is controlling LED's.\
To observe how this program behaves, we need to send a string via serial port. If the string is equal to "Hello", 
the microcontroller will send back the word "World!" and the yellow LED will light up. Otherwise esp won't send anything and red LED
will light up.
##
Author: Mateusz Szpot\
Date: 26.02.2025
