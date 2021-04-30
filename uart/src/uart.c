#include "uart.h"

// Omit adding const after the UART type.  It adds warnings with every SiLabs UART function
// There's some safety lost, but it compiles to the same code.
USART_TypeDef *UART_TABLE[NUM_UARTS] = {USART0, USART1};
LEUART_TypeDef *DEBUG_UART = LEUART0;

void uart_tx(USART_TypeDef *uart, const uint8_t *data, size_t size)
{
	size_t i;

	for (i=0; i<size; ++i) {
		USART_Tx(uart, data[i]);
	}
}

void uart_rx(USART_TypeDef *uart, uint8_t *data, size_t size)
{
	size_t i;

	for (i=0; i<size; ++i) {
		data[i] = USART_Rx(uart);
	}
}

void uart_baudrate_set(USART_TypeDef *uart, uint32_t baud)
{
	USART_BaudrateAsyncSet(uart, 0, baud, usartOVS16);
}

void uart_clock_init(void)
{
	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
}

void uart_clock_disable(void)
{
	CMU_ClockEnable(cmuClock_USART0, false);
	CMU_ClockEnable(cmuClock_USART1, false);
	CMU_ClockEnable(cmuClock_GPIO, false);
}

void uart_init(USART_TypeDef *uart, uint32_t baud)
{
	USART_InitAsync_TypeDef init_data;

	init_data.enable = true;
	init_data.refFreq = 0;
	init_data.baudrate = baud;
	init_data.oversampling = usartOVS16;
	init_data.databits = usartDatabits8;
	init_data.parity = usartNoParity;
	init_data.stopbits = usartStopbits1;
	init_data.mvdis = false;
	init_data.prsRxCh = false;

	USART_InitAsync(uart, &init_data);

	// Initialize the routing and GPIO for the specific UART
	if (uart == UART_TABLE[0]) {
		GPIO_PinModeSet(UART0_TX_PORT, UART0_TX_PIN, gpioModePushPull, 1);
		GPIO_PinModeSet(UART0_RX_PORT, UART0_RX_PIN, gpioModeInput, 0);

		uart->CMD |= USART_CMD_TXEN | USART_CMD_RXEN;
		uart->ROUTE |= USART_ROUTE_LOCATION_LOC0 | USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;
	}
	else if (uart == UART_TABLE[1]) {
		GPIO_PinModeSet(UART1_TX_PORT, UART1_TX_PIN, gpioModePushPull, 1);
		GPIO_PinModeSet(UART1_RX_PORT, UART1_RX_PIN, gpioModeInput, 0);

		uart->CMD |= USART_CMD_TXEN | USART_CMD_RXEN;
		uart->ROUTE |= USART_ROUTE_LOCATION_LOC0 | USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;
	}
}

#if defined(UART_DEBUG_OUTPUT)
void debug_uart_init(LEUART_TypeDef *uart, uint32_t baud)
{
	LEUART_Init_TypeDef init_data;

	init_data.enable = true;
	init_data.refFreq = 0;
	init_data.baudrate = baud;
	init_data.databits = leuartDatabits8;
	init_data.parity = leuartNoParity;
	init_data.stopbits = leuartStopbits1;

	LEUART_Init(uart, &init_data);

	GPIO_PinModeSet(DEBUG_UART_TX_PORT, DEBUG_UART_TX_PIN, gpioModePushPull, 1);
	uart->CMD |= LEUART_CMD_TXEN | LEUART_CMD_RXEN;
	uart->ROUTE |= LEUART_ROUTE_LOCATION_LOC0 | LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN;
}

void debug_uart_clock_init(CMU_Select_TypeDef source)
{
	CMU_ClockEnable(cmuClock_LEUART0, true);
	CMU_ClockEnable(cmuClock_LFB, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockSelectSet(cmuClock_LFB, source);
}

void debug_uart_clock_disable(void)
{
	CMU_ClockEnable(cmuClock_LEUART0, false);
}

// To avoid causing extra buffer copying outside of the function, the name of the function that caused the error can
// optionally be passed in
void debug_uart_tx(const char *str, const char *fn_name)
{
	size_t i;

	if (fn_name) {
		for (i=0; i<strlen(fn_name); ++i) {
			LEUART_Tx(DEBUG_UART, (uint8_t)fn_name[i]);
		}
		LEUART_Tx(DEBUG_UART, 0x00);
	}

	for (i=0; i<strlen(str); ++i) {
		LEUART_Tx(DEBUG_UART, (uint8_t)str[i]);
	}
}
#else
void debug_uart_init(LEUART_TypeDef *uart, uint32_t baud)
{
}

void debug_uart_clock_init(CMU_Select_TypeDef source)
{
}

void debug_uart_clock_disable(CMU_Select_TypeDef source)
{
}
void debug_uart_tx(LEUART_TypeDef uart, const char *str, const char *fn_name)
{
}

#endif
