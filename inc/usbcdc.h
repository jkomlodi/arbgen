
#include <stdbool.h>
#include <stdint.h>

#ifndef INC_USBCDC_H_
#define INC_USBCDC_H_

extern uint8_t usbcdc_tx_buf[64] __attribute__((aligned(4)));
extern uint8_t usbcdc_rx_buf[64] __attribute__((aligned(4)));
extern uint8_t rx_buf_size;
extern bool usbcdc_has_data;
extern uint8_t tx_buf_size;

size_t usbcdc_rx(uint8_t *data);

#endif /* INC_USBCDC_H_ */
