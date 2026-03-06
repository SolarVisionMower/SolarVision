#include "rplidar_task.h"
#include "rplidar_types.h"
#include "rplidar_protocol.h"
#include "rplidar.h"
#include "config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *TAG = "rplidar_task";

static QueueHandle_t s_q = NULL;

esp_err_t rplidar_init_uart(void)
{
    const uart_config_t cfg = {
        .baud_rate = RPLIDAR_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(RPLIDAR_UART_NUM, RPLIDAR_UART_RX_BUF, RPLIDAR_UART_TX_BUF, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(RPLIDAR_UART_NUM, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(RPLIDAR_UART_NUM, RPLIDAR_UART_TX_PIN, RPLIDAR_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    uart_flush(RPLIDAR_UART_NUM);

    return ESP_OK;
}

static void rplidar_reader_task(void *arg)
{
    (void)arg;

    // Make sure lidar is stopped, then start scanning
    rplidar_send_stop();
    vTaskDelay(pdMS_TO_TICKS(50));
    rplidar_send_scan();

    // Rolling 5-byte window
    uint8_t win[RPLIDAR_NODE_SIZE] = {0};
    int win_fill = 0;

    float prev_angle = 0.0f;
    bool have_prev = false;

    while (1) {
        uint8_t b;
        int n = uart_read_bytes(RPLIDAR_UART_NUM, &b, 1, pdMS_TO_TICKS(1000));
        if (n <= 0) continue;

        if (win_fill < RPLIDAR_NODE_SIZE) {
            win[win_fill++] = b;
            if (win_fill < RPLIDAR_NODE_SIZE) continue;
        } else {
            // shift left by 1, append new byte
            for (int i = 0; i < RPLIDAR_NODE_SIZE - 1; i++) win[i] = win[i + 1];
            win[RPLIDAR_NODE_SIZE - 1] = b;
        }

        float ang, dist;
        uint8_t q;

        if (!rplidar_parse_node_5b(win, &ang, &dist, &q)) {
            continue;
        }

        rplidar_point_t pt = {0};
        pt.angle_deg = ang;
        pt.distance_mm = dist;
        pt.quality = q;

        // Start-of-scan heuristic: angle wrapped around
        if (have_prev && ang < prev_angle) pt.start_flag = true;
        prev_angle = ang;
        have_prev = true;

        if (s_q) {
            (void)xQueueSend(s_q, &pt, 0);
        }
    }
}

QueueHandle_t rplidar_point_queue_start(void)
{
    if (s_q) return s_q;

    s_q = xQueueCreate(RPLIDAR_POINT_QUEUE_LEN, sizeof(rplidar_point_t));
    if (!s_q) {
        ESP_LOGE(TAG, "Failed to create point queue");
        return NULL;
    }

    BaseType_t ok = xTaskCreate(rplidar_reader_task,
                               "rplidar_reader",
                               RPLIDAR_TASK_STACK,
                               NULL,
                               RPLIDAR_TASK_PRIO,
                               NULL);

    if (ok != pdPASS) {
        ESP_LOGE(TAG, "Failed to create reader task");
        vQueueDelete(s_q);
        s_q = NULL;
        return NULL;
    }

    ESP_LOGI(TAG, "RPLIDAR reader task started");
    return s_q;
}
