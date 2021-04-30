#include "em_device.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_usb.h"
#include "dmactrl.h"
#include "cdc.h"

#define CDC_BULK_EP_SIZE  (USB_FS_BULK_EP_MAXSIZE) /* This is the max. ep size.    */
#define CDC_USB_RX_BUF_SIZ  CDC_BULK_EP_SIZE /* Packet size when receiving on USB. */
#define CDC_USB_TX_BUF_SIZ  127    /* Packet size when transmitting on USB.  */

/* Calculate a timeout in ms corresponding to 5 char times on current     */
/* baudrate. Minimum timeout is set to 10 ms.                             */
#define CDC_RX_TIMEOUT    SL_MAX(10U, 50000 / (cdcLineCoding.dwDTERate))

/* The serial port LINE CODING data structure, used to carry information  */
/* about serial port baudrate, parity etc. between host and device.       */
typedef struct {
  uint32_t dwDTERate;               /** Baudrate                            */
  uint8_t  bCharFormat;             /** Stop bits, 0=1 1=1.5 2=2            */
  uint8_t  bParityType;             /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
  uint8_t  bDataBits;               /** 5, 6, 7, 8 or 16                    */
  uint8_t  dummy;                   /** To ensure size is a multiple of 4 bytes */
} __attribute__((packed)) cdcLineCoding_TypeDef;

/*** Function prototypes. ***/

static int  UsbDataReceived(USB_Status_TypeDef status, uint32_t xferred,
                            uint32_t remaining);
static int  LineCodingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining);

cdcLineCoding_TypeDef __attribute__((aligned(4))) cdcLineCoding =
{
  115200, 0, 0, 8, 0
};

// USB data buffers must be 32-bit aligned
uint8_t usbcdc_tx_buf[64] __attribute__((aligned(4)));
uint8_t usbcdc_rx_buf[64] __attribute__((aligned(4)));
uint8_t rx_buf_size = 0;
bool usbcdc_has_data = false;
uint8_t tx_buf_size = 0;

size_t usbcdc_rx(uint8_t *data)
{
	size_t bytes_copied;

	memcpy(data, usbcdc_rx_buf, rx_buf_size);
	bytes_copied = rx_buf_size;
	rx_buf_size = 0;

	usbcdc_has_data = false;

	return bytes_copied;
}


int CDC_SetupCmd(const USB_Setup_TypeDef *setup)
{
  int retVal = USB_STATUS_REQ_UNHANDLED;

  if ( (setup->Type         == USB_SETUP_TYPE_CLASS)
       && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)    ) {
    switch (setup->bRequest) {
      case USB_CDC_GETLINECODING:
        /********************/
        if ( (setup->wValue       == 0)
             && (setup->wIndex    == CDC_CTRL_INTERFACE_NO) /* Interface no. */
             && (setup->wLength   == 7)                     /* Length of cdcLineCoding. */
             && (setup->Direction == USB_SETUP_DIR_IN)    ) {
          /* Send current settings to USB host. */
          USBD_Write(0, (void*) &cdcLineCoding, 7, NULL);
          retVal = USB_STATUS_OK;
        }
        break;

      case USB_CDC_SETLINECODING:
        /********************/
        if ( (setup->wValue       == 0)
             && (setup->wIndex    == CDC_CTRL_INTERFACE_NO) /* Interface no. */
             && (setup->wLength   == 7)                     /* Length of cdcLineCoding. */
             && (setup->Direction != USB_SETUP_DIR_IN)    ) {
          /* Get new settings from USB host. */
          USBD_Read(0, (void*) &cdcLineCoding, 7, LineCodingReceived);
          retVal = USB_STATUS_OK;
        }
        break;

      case USB_CDC_SETCTRLLINESTATE:
        /********************/
        if ( (setup->wIndex     == CDC_CTRL_INTERFACE_NO)      /* Interface no.  */
             && (setup->wLength == 0)    ) {                /* No data.       */
          /* Do nothing ( Non compliant behaviour !! ) */
          retVal = USB_STATUS_OK;
        }
        break;
    }
  }

  return retVal;
}

void CDC_StateChangeEvent(USBD_State_TypeDef oldState,
                          USBD_State_TypeDef newState)
{
  if (newState == USBD_STATE_CONFIGURED) {
    /* We have been configured, start CDC functionality ! */

    if (oldState == USBD_STATE_SUSPENDED) { /* Resume ?   */
    }

    USBD_Read(CDC_EP_DATA_OUT, (void*) usbcdc_rx_buf,
              CDC_USB_RX_BUF_SIZ, UsbDataReceived);


    // USBTIMER_Start(CDC_TIMER_ID, CDC_RX_TIMEOUT, UartRxTimeout);
  } else if ((oldState == USBD_STATE_CONFIGURED)
             && (newState != USBD_STATE_SUSPENDED)) {
    /* We have been de-configured, stop CDC functionality. */
    USBTIMER_Stop(CDC_TIMER_ID);

  } else if (newState == USBD_STATE_SUSPENDED) {
    /* We have been suspended, stop CDC functionality. */
    /* Reduce current consumption to below 2.5 mA.     */
    USBTIMER_Stop(CDC_TIMER_ID);
  }
}

static int UsbDataReceived(USB_Status_TypeDef status,
                           uint32_t xferred,
                           uint32_t remaining)
{
  (void) remaining;            /* Unused parameter. */

  if ((status == USB_STATUS_OK) && (xferred > 0)) {

    /* Start a new USB receive transfer. */
    USBD_Read(CDC_EP_DATA_OUT, (void*) usbcdc_rx_buf,
                CDC_USB_RX_BUF_SIZ, UsbDataReceived);

    rx_buf_size = xferred;
    usbcdc_has_data = true;
    }

  return USB_STATUS_OK;
}

static int UsbDataTransmitted(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining)
{
  (void) xferred;              /* Unused parameter. */
  (void) remaining;            /* Unused parameter. */

  if (status == USB_STATUS_OK) {

      /* dmaRxActive = false means that a new UART Rx DMA can be started. */

      USBD_Write(CDC_EP_DATA_IN, (void*) usbcdc_tx_buf,
    		  tx_buf_size, UsbDataTransmitted);

      // USBTIMER_Start(CDC_TIMER_ID, CDC_RX_TIMEOUT, UartRxTimeout);

  }
  return USB_STATUS_OK;
}

static int LineCodingReceived(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining)
{
  (void) remaining;

  // We received new UART communication settings from the host, but we don't care since we're not using a UART
  if ((status == USB_STATUS_OK) && (xferred == 7)) {
    return USB_STATUS_OK;
  }

  return USB_STATUS_REQ_ERR;
}
