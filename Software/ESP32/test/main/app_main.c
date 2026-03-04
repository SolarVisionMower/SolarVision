#include "config.h"
#include "rplidar.h"
#include "rplidar_task.h"
#include "rplidar_types.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "app";

void app_main(void) {
    ESP_LOGI(TAG, "SolarVision ESP32 (RPLIDAR bringup)");

    ESP_ERROR_CHECK(rplidar_init_uart());

    QueueHandle_t q = rplidar_point_queue_start();
    if (!q) {
        ESP_LOGE(TAG, "Failed to start RPLIDAR task");
        return;
    }

    // Simple consumer example: print a point occasionally
    rplidar_point_t pt;
    uint32_t printed = 0;

    while (1) {
        if (xQueueReceive(q, &pt, pdMS_TO_TICKS(500))) {
            // Print every ~200 points so serial output doesn’t explode
            if ((printed++ % 200) == 0) {
                ESP_LOGI(TAG, "pt: ang=%.2f deg dist=%.1f mm q=%u start=%d",
                         pt.angle_deg, pt.distance_mm, pt.quality, (int)pt.start_flag);
            }
        }
    }
}