#include "config.h"

#include "rplidar_task.h"
#include "rplidar_types.h"
#include "lidar_xy.h"
#include "occupancy_grid.h"
#include "obstacle_detection.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "app";

static occupancy_grid_t grid;
static obstacle_state_t obstacles;

void app_main(void)
{
    ESP_LOGI(TAG, "SolarVision ESP32 (Milestone 3: Obstacle Detection)");

    ESP_LOGI(TAG, "Waiting for LiDAR power/stabilization...");
    vTaskDelay(pdMS_TO_TICKS(3000));

    ESP_ERROR_CHECK(rplidar_init_uart());

    QueueHandle_t q = rplidar_point_queue_start();
    if (!q) {
        ESP_LOGE(TAG, "Failed to start RPLIDAR task");
        return;
    }

    occupancy_grid_init(&grid);
    obstacle_state_reset(&obstacles);

    rplidar_point_t pt;
    lidar_xy_point_t xy_pt;

    uint32_t printed = 0;
    uint32_t marked_points = 0;
    uint32_t out_of_grid_points = 0;
    uint32_t frame_count = 0;
    uint32_t timeout_count = 0;

    while (1) {
        if (xQueueReceive(q, &pt, pdMS_TO_TICKS(500)) == pdTRUE) {

            // When a new scan starts, report the last frame's obstacle status
            if (pt.start_flag) {
                frame_count++;

                if (frame_count > 1) {
                    ESP_LOGI(TAG,
                             "frame=%lu front=%d left=%d right=%d stop=%d hits[F=%lu L=%lu R=%lu S=%lu]",
                             (unsigned long)frame_count,
                             (int)obstacles.front_blocked,
                             (int)obstacles.left_blocked,
                             (int)obstacles.right_blocked,
                             (int)obstacles.stop_required,
                             (unsigned long)obstacles.front_hits,
                             (unsigned long)obstacles.left_hits,
                             (unsigned long)obstacles.right_hits,
                             (unsigned long)obstacles.stop_hits);
                }

                obstacle_state_reset(&obstacles);
            }

            bool xy_ok = lidar_polar_to_xy(&pt, &xy_pt);
            bool grid_ok = false;
            bool obstacle_hit = false;
            int gx = -1;
            int gy = -1;

            if (xy_ok) {
                grid_ok = occupancy_grid_mark_xy(&grid, &xy_pt);
                obstacle_hit = obstacle_process_point(&obstacles, &xy_pt);

                if (grid_ok) {
                    marked_points++;
                    occupancy_grid_xy_to_cell(xy_pt.x_mm, xy_pt.y_mm, &gx, &gy);
                } else {
                    out_of_grid_points++;
                }
            }

            if ((printed++ % 200) == 0) {
                if (xy_ok && grid_ok) {
                    ESP_LOGI(TAG,
                             "polar: ang=%.2f deg dist=%.1f mm q=%u start=%d -> xy: x=%.1f mm y=%.1f mm -> cell=(%d,%d) hit=%d marked=%lu",
                             pt.angle_deg,
                             pt.distance_mm,
                             (unsigned)pt.quality,
                             (int)pt.start_flag,
                             xy_pt.x_mm,
                             xy_pt.y_mm,
                             gx,
                             gy,
                             (int)obstacle_hit,
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

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}