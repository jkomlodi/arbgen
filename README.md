# arbgen (WIP)
USB-controlled abritrary pattern generator with data reception and conditional statements

# Done
 * Rough draft of human readable protocol
 * Debug UART
 * UART TX
 * UART RX
 * Bus selection
 * Baud rate changing
 * USB CDC OUT
 
# Todo
 * USB CDC IN
 * Simultaneous UART TX (e.g. RS485 D+ D-)
 * Clock output
 * Desktop software controlled UART pin modes (wired-AND, push-pull, etc)
 * Ensure clocked protocols (SPI, I2C, etc) work correctly
 * Conditional statements and comparison operators for UART RX
 * Pre-parse and use DMA for UART TX and maybe RX
 * Look into using DMA for USB IN
 * Periodic stress testing + determine hardware needs
 * Desktop software control for Other useful UART registers (loopback, some possible SPI functionality)
 * Determine board profile for hardware (# busses, max frequency, etc)
 * Make protocol less verbose, and probably less human readable
