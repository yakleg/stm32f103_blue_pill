#include <stdio.h>
#include <string.h>

#include <stm32f1xx_hal.h>
#include <cmsis_os.h>
#include <main.h>



extern void blinkingTask(void const *argument);

void blinkingTask(void const *argument)
{
    uint32_t counter = 0;
    HAL_Delay(1000);

    while (1)
    {
        osDelay(50);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        printf("Hello World! Log # %lu\r\n", ++counter);
    }
}