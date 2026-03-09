#include "config.h"

#include "rplidar_task.h"
#include "rplidar_types.h"
#include "lidar_xy.h"
#include "occupancy_grid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "app";

// IMPORTANT: keep this out of the app_main stack
static occupancy_grid_t grid;

void app_main(void)
{
    ESP_LOGI(TAG, "SolarVision ESP32 (Milestone 2: Occupancy Grid)");

    // Give the LiDAR time to power up and begin spinning
    ESP_LOGI(TAG, "Waiting for LiDAR power/stabilization...");
    vTaskDelay(pdMS_TO_TICKS(3000));

    ESP_ERROR_CHECK(rplidar_init_uart());

    QueueHandle_t q = rplidar_point_queue_start();
    if (!q) {
        ESP_LOGE(TAG, "Failed to start RPLIDAR task");
        return;
    }

    occupancy_grid_init(&grid);

    rplidar_point_t pt;
    lidar_xy_point_t xy_pt;

    uint32_t printed = 0;
    uint32_t marked_points = 0;
    uint32_t out_of_grid_points = 0;
    uint32_t timeout_count = 0;

    while (1) {
        if (xQueueReceive(q, &pt, pdMS_TO_TICKS(500)) == pdTRUE) {

            bool xy_ok = lidar_polar_to_xy(&pt, &xy_pt);
            bool grid_ok = false;
            int gx = -1;
            int gy = -1;

            if (xy_ok) {
                grid_ok = occupancy_grid_mark_xy(&grid, &xy_pt);

                if (grid_ok) {
                    marked_points++;
                    occupancy_grid_xy_to_cell(xy_pt.x_mm, xy_pt.y_mm, &gx, &gy);
                } else {
                    out_of_grid_points++;
                }
            }

            // Keep logs light so the UART/reader task stays healthy
            if ((printed++ % 200) == 0) {
                if (xy_ok && grid_ok) {
                    ESP_LOGI(TAG,
                             "polar: ang=%.2f deg dist=%.1f mm q=%u start=%d -> xy: x=%.1f mm y=%.1f mm -> cell=(%d,%d) marked=%lu",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag,
                             xy_pt.x_mm,
                             xy_pt.y_mm,
                             gx,
                             gy,
                             (unsigned long)marked_points);
                } else if (xy_ok) {
                    ESP_LOGI(TAG,
                             "XY valid but outside grid: ang=%.2f dist=%.1f -> x=%.1f y=%.1f out_of_grid=%lu",
                             pt.angle_deg,
                             pt.distance_mm,
                             xy_pt.x_mm,
                             xy_pt.y_mm,
                             (unsigned long)out_of_grid_points);
                } else {
                    ESP_LOGW(TAG,
                             "Invalid point: ang=%.2f deg dist=%.1f mm q=%u start=%d",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag);
                }
            }
        } else {
            timeout_count++;
            if ((timeout_count % 5) == 0) {
                ESP_LOGW(TAG,
                         "Queue timeout: no lidar points received (timeouts=%lu)",
                         (unsigned long)timeout_count);
            }
        }

        // Important: let the scheduler run other tasks smoothly
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}