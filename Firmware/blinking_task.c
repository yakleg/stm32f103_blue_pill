#include <stm32f1xx_hal.h>
#include <main.h>
#include <usbd_cdc_if.h>
// #include <usart.h>

#include <string.h>

extern void blinkingTask(void const *argument);

void blinkingTask(void const *argument)
{
    int counter = 0;
    char buffer[100];

    while (1)
    {
        HAL_Delay(1000);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);

        sprintf(buffer, "Hello World! Log # %d\r\n", ++counter);
        CDC_Transmit_FS((uint8_t *)&buffer, strlen(buffer));
    }
}