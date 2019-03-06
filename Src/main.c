#include "main.h"

USBH_HandleTypeDef hUSBHost;
CDC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id);
static void CDC_InitApplication(void);

static int requestForReceive = 0;

#define RX_BUFF_SIZE   0x400
static uint8_t CDC_RX_Buffer[RX_BUFF_SIZE];

int main(void)
{
  /* STM32F4xx HAL library initialization: - Configure the Flash prefetch,
   * instruction and Data caches - Configure the Systick to generate an
   * interrupt each 1 msec - Set NVIC Group Priority to 4 - Global MSP (MCU
   * Support Package) initialization */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  /* Init CDC Application */
  CDC_InitApplication();

  /* Init Host Library */
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);

  /* Add Supported Class */
  USBH_RegisterClass(&hUSBHost, USBH_CDC_CLASS);

  /* Start Host Process */
  USBH_Start(&hUSBHost);

  /* Run Application (Blocking mode) */
  while (1)
  {
    /* USB Host Background task */
    USBH_Process(&hUSBHost);

    /* CDC Menu Process */
    //CDC_MenuProcess();
  }
}

/**
  * @brief  CDC application Init.
  * @param  None
  * @retval None
  */
static void CDC_InitApplication(void)
{
  /* Start CDC Interface */
  USBH_UsrLog("Starting CDC Demo");

  Menu_Init();
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
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
    GetDefaultConfiguration();
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

/**
  * @brief  Toggles LEDs to show user input state.
  * @param  None
  * @retval None
  */
void Toggle_Leds(void)
{
  static uint32_t ticks;

  if (ticks++ == 100)
  {
//    BSP_LED_Toggle(LED1);
//    BSP_LED_Toggle(LED2);
//    BSP_LED_Toggle(LED3);
//    BSP_LED_Toggle(LED4);
    ticks = 0;
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    HAL_RCC_OscConfig (&RCC_OscInitStruct);
    
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
   * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file, 
   * line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
