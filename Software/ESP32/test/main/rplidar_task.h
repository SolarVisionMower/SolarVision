#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize UART + driver state for the RPLIDAR
esp_err_t rplidar_init_uart(void);

// Start task that pushes rplidar_point_t into a queue, return queue handle
QueueHandle_t rplidar_point_queue_start(void);

#ifdef __cplusplus
}
#endif
