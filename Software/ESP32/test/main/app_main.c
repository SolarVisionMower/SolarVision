#include "config.h"

#include "rplidar_task.h"
#include "rplidar_types.h"
#include "lidar_xy.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"


static const char *TAG = "app";

void app_main(void) {
    ESP_LOGI(TAG, "SolarVision ESP32 (RPLIDAR bringup + Milestone 1 XY)");

    ESP_ERROR_CHECK(rplidar_init_uart());

    QueueHandle_t q = rplidar_point_queue_start();
    if (!q) {
        ESP_LOGE(TAG, "Failed to start RPLIDAR task");
        return;
    }

    rplidar_point_t pt;
    lidar_xy_point_t xy_pt;
    uint32_t printed = 0;

    while (1) {
        if (xQueueReceive(q, &pt, pdMS_TO_TICKS(500)) == pdTRUE) {

            bool ok = lidar_polar_to_xy(&pt, &xy_pt);

            if ((printed++ % 200) == 0) {
                if (ok) {
                    ESP_LOGI(TAG,
                             "polar: ang=%.2f deg dist=%.1f mm q=%u start=%d -> xy: x=%.1f mm y=%.1f mm",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag,
                             xy_pt.x_mm,
                             xy_pt.y_mm);
                } else {
                    ESP_LOGW(TAG,
                             "invalid point: ang=%.2f deg dist=%.1f mm q=%u start=%d",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag);
                }
            }
        }
    }
}
