#ifndef UART_INC_UART_H_
#define UART_INC_UART_H_

#include <stdint.h>

#include "em_usart.h"
#include "em_leuart.h"
#include "em_cmu.h"
#include "em_gpio.h"

// Redefine SiLabs' definition into our own, so it's easier to change in the future
#define NUM_UARTS USART_COUNT

#define UART0_TX_PORT gpioPortE
#define UART0_TX_PIN 10
#define UART0_RX_PORT gpioPortE
#define UART0_RX_PIN 11
#define UART0_LOCATION USART_ROUTE_LOCATION_LOC0

#define UART1_TX_PORT gpioPortC
#define UART1_TX_PIN 0
#define UART1_RX_PORT gpioPortC
#define UART1_RX_PIN 1
#define UART1_LOCATION USART_ROUTE_LOCATION_LOC0

// These definitions use a UART for debug output
// For now, we're using the LEUART, since we only have 2 UARTs on this MCU
#if defined(DEBUG)
#define UART_DEBUG_OUTPUT

#define DEBUG_UART_TX_PORT gpioPortD
#define DEBUG_UART_TX_PIN 4
#define DEBUG_UART_LOCATION LEUART_ROUTE_LOCATION_LOC0

#endif // DEBUG

extern USART_TypeDef *UART_TABLE[NUM_UARTS];
extern LEUART_TypeDef *DEBUG_UART;

void uart_tx(USART_TypeDef *uart, const uint8_t *data, size_t size);
void uart_rx(USART_TypeDef *uart, uint8_t *data, size_t size);
void uart_baudrate_set(USART_TypeDef *uart, uint32_t baud);
void uart_clock_init(void);
void uart_clock_disable(void);
void uart_init(USART_TypeDef *uart, uint32_t baud);

void debug_uart_tx(const char *str, const char *fn_name);
void debug_uart_init(LEUART_TypeDef *uart, uint32_t baud);
void debug_uart_clock_init(CMU_Select_TypeDef source);
void debug_uart_clock_disable(void);


#endif /* UART_INC_UART_H_ */
