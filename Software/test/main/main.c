#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define the pin number
#define BLINK_GPIO 4

void app_main(void)
{
    /* Reset the pin and set the direction */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        /* Turn the LED on (High level) */
        gpio_set_level(BLINK_GPIO, 1);
        printf("LED ON\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Turn the LED off (Low level) */
        gpio_set_level(BLINK_GPIO, 0);
        printf("LED OFF\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
