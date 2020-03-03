#include "main.h"


USBH_HandleTypeDef hUSBHost;
CDC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id);

static int requestForReceive = 0;

#define RX_BUFF_SIZE   0x400
static uint8_t CDC_RX_Buffer[RX_BUFF_SIZE];

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  USBH_RegisterClass(&hUSBHost, USBH_CDC_CLASS);

  USBH_Start(&hUSBHost);

  while (1)
  {
    USBH_Process(&hUSBHost);
  }
}

static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
  switch (id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
    break;

  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_DISCONNECT;

    USBH_CDC_Stop(&hUSBHost);
    requestForReceive = 0;
  
    break;

  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
  
    if(requestForReceive == 0)
    {
        memset(CDC_RX_Buffer, 0, RX_BUFF_SIZE);
        USBH_CDC_Receive(&hUSBHost, CDC_RX_Buffer, RX_BUFF_SIZE);
        requestForReceive = 1;
    }
  
    break;

  case HOST_USER_CONNECTION:
    Appli_state = APPLICATION_START;
    break;

  default:
    break;
  }
}

static void DumpReceivedData(void)
{
    volatile uint16_t size = USBH_CDC_GetLastReceivedDataSize(&hUSBHost);
    
    volatile uint8_t *buffer = CDC_RX_Buffer;
    
    buffer[0] = buffer[0];
    
    size = size;
}


void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef * phost)
{
  DumpReceivedData();
  USBH_CDC_Receive(&hUSBHost, CDC_RX_Buffer, RX_BUFF_SIZE);
}
