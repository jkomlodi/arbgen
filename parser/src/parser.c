#include <stdlib.h>

#include "parser.h"
#include "uart.h"
#include "strutils.h"

uint8_t parser_cmd_buf[PARSER_CMD_BUF_SIZE];
size_t parser_cmd_buf_size = 0;

bool parser_parse_commands(const uint8_t *cmd, size_t cmd_size, uint8_t *data_out, size_t *data_out_size)
{
	bool retval = true;
	size_t p_cmd = 0;

	*data_out_size = 0;

	while (p_cmd < cmd_size) {
		switch(cmd[p_cmd]) {
		case 'T':
			parser_handle_cmd_tx(&cmd[p_cmd], &p_cmd);
			break;
		case 'R':
			parser_handle_cmd_rx(&cmd[p_cmd], &p_cmd, &data_out[*data_out_size], data_out_size);
			break;
		case 'B':
			parser_handle_cmd_baudrate_set(&cmd[p_cmd], &p_cmd);
			break;
		case 'F':

			break;
		case 'C':

			break;
		// skip over spaces
		case ' ':
			++p_cmd;
			break;
		// We received something unknown
		default:
			retval = false;
			++p_cmd;
			break;
		}
	}

	return retval;
}

// Format is "T bus ## data", including the 1-byte whitespace.
// Bus number is 1 ASCII hex number data is many bytes in ASCII hex format
bool parser_handle_cmd_tx(const uint8_t *cmd, size_t *cmd_size)
{
	uint8_t bus;
	uint8_t byte_to_send;
	bool retval = true;
	size_t p_cmd = 0;

	// Skip over "T "
	p_cmd += 2;

	if (!parser_get_bus(&cmd[p_cmd], &p_cmd, &bus)) {
		debug_uart_tx("Error getting bus", __PRETTY_FUNCTION__);
		retval = false;
	}

	if (retval == true) {
		while ((cmd[p_cmd] != ' ') && (cmd[p_cmd] != '\0')) {
			if (!asciihex_to_byte(&cmd[p_cmd], 2, &byte_to_send, NULL)) {
				debug_uart_tx("Error getting byte to send", __PRETTY_FUNCTION__);
				retval = false;
			}
			else {
				uart_tx(UART_TABLE[bus], &byte_to_send, 1);
			}

			p_cmd += 2;
		}
	}


	*cmd_size += p_cmd;

	return retval;
}


// Format is "R bus ## number of bytes to RX", including the 1-byte whitespace.
// Bus number is 1 ASCII hex number, bytes to RX is ASCII base 10
bool parser_handle_cmd_rx(const uint8_t *cmd, size_t *cmd_size, uint8_t *data_out, size_t *data_out_size)
{
	bool retval;
	uint8_t bus;
	char *size_len;
	size_t p_cmd = 0;

	// Skip over "R "
	p_cmd += 2;

	if (!parser_get_bus(&cmd[p_cmd], &p_cmd, &bus)) {
		debug_uart_tx("Error getting bus", __PRETTY_FUNCTION__);
		retval = false;
	}
	else {
		*data_out_size = strtol((const char *)cmd, (char **)&size_len, 10);
		uart_rx(UART_TABLE[bus], data_out, *data_out_size);
	}

	return retval;
}

// Change the baud rate of a bus
// Format is "B bus baud", including the 1-byte whitespace.
// Bus number is 1 ASCII hex number, baud is ASCII base 10
bool parser_handle_cmd_baudrate_set(const uint8_t *cmd, size_t *cmd_size)
{
	uint32_t baud;
	uint8_t bus;
	uint8_t *num_len;
	size_t p_cmd = 0;
	bool retval = true;

	// Skip over "B "
	p_cmd += 2;

	if (!parser_get_bus(&cmd[p_cmd], &p_cmd, &bus)) {
		debug_uart_tx("Error getting bus", __PRETTY_FUNCTION__);
		retval = false;
	}
	else {
		baud = strtol((const char *)&cmd[p_cmd], (char **)&num_len, 10);
		uart_baudrate_set(UART_TABLE[bus], baud);

		p_cmd += (num_len - &cmd[p_cmd]);
	}

	*cmd_size += p_cmd;

	return retval;
}

// Internal use function to get the bus number from input
bool parser_get_bus(const uint8_t *cmd, size_t *cmd_size, uint8_t *bus)
{
	bool retval = true;

	if (!asciihex_to_byte(&cmd[0], 2, bus, NULL)) {
		debug_uart_tx("Error getting bus from ASCII string", __PRETTY_FUNCTION__);
		retval = false;
	}
	if (*bus >= NUM_UARTS) {
		debug_uart_tx("Bus number is greater than the number of UARTS", __PRETTY_FUNCTION__);
		retval = false;
	}

	// Skip over "## "
	*cmd_size += 3;

	return retval;
}
