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

void app_main(void)
{
    ESP_LOGI(TAG, "SolarVision ESP32 (Milestone 2: Occupancy Grid)");

    ESP_ERROR_CHECK(rplidar_init_uart());

    QueueHandle_t q = rplidar_point_queue_start();
    if (!q) {
        ESP_LOGE(TAG, "Failed to start RPLIDAR task");
        return;
    }

    occupancy_grid_t grid;
    occupancy_grid_init(&grid);

    rplidar_point_t pt;
    lidar_xy_point_t xy_pt;

    uint32_t printed = 0;
    uint32_t marked_points = 0;

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
                }
            }

            if ((printed++ % 200) == 0) {
                if (xy_ok && grid_ok) {
                    ESP_LOGI(TAG,
                             "polar: ang=%.2f deg dist=%.1f mm q=%u start=%d -> xy: x=%.1f y=%.1f -> cell=(%d,%d) marked=%lu",
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
                    ESP_LOGW(TAG,
                             "XY valid but out of grid: ang=%.2f dist=%.1f -> x=%.1f y=%.1f",
                             pt.angle_deg,
                             pt.distance_mm,
                             xy_pt.x_mm,
                             xy_pt.y_mm);
                } else {
                    ESP_LOGW(TAG,
                             "Invalid point: ang=%.2f deg dist=%.1f mm q=%u start=%d",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag);
                }
            }
        }
    }
}
