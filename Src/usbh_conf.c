#include "stm32f4xx_hal.h"
#include "usbh_core.h"


HCD_HandleTypeDef hhcd;


USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef * phost)
{
    /* Set the LL driver parameters */
    hhcd.Instance = USB_OTG_FS;
    hhcd.Init.Host_channels = 11;
    hhcd.Init.dma_enable = 0;
    hhcd.Init.low_power_enable = 0;
    hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;
    hhcd.Init.Sof_enable = 0;
    hhcd.Init.vbus_sensing_enable = 0;
    hhcd.Init.use_external_vbus = 0;
    hhcd.Init.speed = HCD_SPEED_FULL;
    /* Link the driver to the stack */
    hhcd.pData = phost;
    phost->pData = &hhcd;
    /* Initialize the LL Driver */
    HAL_HCD_Init(&hhcd);

    USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));

    return USBH_OK;
}
