#include "em_device.h"
#include "em_chip.h"
#include "uart.h"
#include "em_emu.h"
#include "efm32_utils.h"

// USB
#include "em_usb.h"
#include "cdc.h"
#include "descriptors.h"
#include "usbcdc.h"

// Parser
#include "parser.h"

static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);
void uart_init_to_default(void);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = CDC_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

uint8_t uart_rx_data[64];
size_t uart_rx_data_size;

int main(void)
{
	CHIP_Init();

    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
    CMU_HFRCOBandSet(cmuHFRCOBand_21MHz);
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
	CMU_ENABLE_LE_PERIPHERALS();

	uart_init_to_default();

	USBD_Init(&usbInitStruct);

    while (1)
    {
    	if (usbcdc_has_data) {
    		parser_cmd_buf_size += usbcdc_rx(parser_cmd_buf);
    		parser_parse_commands(parser_cmd_buf, parser_cmd_buf_size, uart_rx_data, &uart_rx_data_size);
    		parser_cmd_buf_size = 0;
    	}
    	EMU_EnterEM2(true);
    }
}


void uart_init_to_default(void)
{
	uint8_t i;

    uart_clock_init();

    for (i=0; i<NUM_UARTS; ++i) {
        uart_init(UART_TABLE[i], 115200);
    }

    debug_uart_clock_init(cmuSelect_LFRCO);
    debug_uart_init(DEBUG_UART, 9600);
}

static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  /* Call CDC driver state change event handler. */
  CDC_StateChangeEvent(oldState, newState);
}
