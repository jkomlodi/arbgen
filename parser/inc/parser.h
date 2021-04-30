#ifndef PARSER_INC_PARSER_H_
#define PARSER_INC_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

#define PARSER_CMD_BUF_SIZE 512

extern uint8_t parser_cmd_buf[PARSER_CMD_BUF_SIZE];
extern size_t parser_cmd_buf_size;

bool parser_parse_commands(const uint8_t *cmd, size_t cmd_size, uint8_t *data_out, size_t *data_out_size);
bool parser_handle_cmd_tx(const uint8_t *cmd, size_t *cmd_size);
bool parser_handle_cmd_rx(const uint8_t *cmd, size_t *cmd_size, uint8_t *data_out, size_t *data_out_size);
bool parser_handle_cmd_baudrate_set(const uint8_t *cmd, size_t *cmd_size);
bool parser_get_bus(const uint8_t *cmd, size_t *cmd_size, uint8_t *bus);

#endif /* PARSER_INC_PARSER_H_ */
