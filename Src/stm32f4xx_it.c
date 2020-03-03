#include "main.h"
#include "stm32f4xx_it.h"

extern HCD_HandleTypeDef hhcd;


void OTG_FS_IRQHandler(void)
{
  HAL_HCD_IRQHandler(&hhcd);
}

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

