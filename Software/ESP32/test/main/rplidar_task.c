#include "rplidar_task.h"
#include "rplidar_types.h"
#include "rplidar_protocol.h"
#include "rplidar.h"
#include "config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "rplidar_task";

static QueueHandle_t s_q = NULL;

esp_err_t rplidar_init_uart(void)
{
    const uart_config_t cfg = {
        .baud_rate = RPLIDAR_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(RPLIDAR_UART_NUM,
                                        RPLIDAR_UART_RX_BUF,
                                        RPLIDAR_UART_TX_BUF,
                                        0,
                                        NULL,
                                        0));

    ESP_ERROR_CHECK(uart_param_config(RPLIDAR_UART_NUM, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(RPLIDAR_UART_NUM,
                                 RPLIDAR_UART_TX_PIN,
                                 RPLIDAR_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_flush(RPLIDAR_UART_NUM));

    ESP_LOGI(TAG, "UART initialized: uart=%d baud=%d tx=%d rx=%d",
             RPLIDAR_UART_NUM,
             RPLIDAR_UART_BAUD,
             RPLIDAR_UART_TX_PIN,
             RPLIDAR_UART_RX_PIN);

    return ESP_OK;
}

static void rplidar_reader_task(void *arg)
{
    (void)arg;

    ESP_LOGI(TAG, "Reader task entered");

    rplidar_send_stop();
    vTaskDelay(pdMS_TO_TICKS(50));
    rplidar_send_scan();
    ESP_LOGI(TAG, "STOP+SCAN commands sent");

    uint8_t win[RPLIDAR_NODE_SIZE] = {0};
    int win_fill = 0;

    float prev_angle = 0.0f;
    bool have_prev = false;

    uint32_t bytes_seen = 0;
    uint32_t valid_nodes = 0;
    uint32_t queue_sends = 0;
    uint32_t read_timeouts = 0;

    while (1) {
        uint8_t b = 0;
        int n = uart_read_bytes(RPLIDAR_UART_NUM, &b, 1, pdMS_TO_TICKS(1000));

        if (n <= 0) {
            read_timeouts++;
            if ((read_timeouts % 5) == 0) {
                ESP_LOGW(TAG, "No UART bytes received yet (timeouts=%lu)",
                         (unsigned long)read_timeouts);
            }
            continue;
        }

        bytes_seen++;

        if ((bytes_seen % 500) == 0) {
            ESP_LOGI(TAG, "UART bytes seen=%lu valid_nodes=%lu queue_sends=%lu",
                     (unsigned long)bytes_seen,
                     (unsigned long)valid_nodes,
                     (unsigned long)queue_sends);
        }

        if (win_fill < RPLIDAR_NODE_SIZE) {
            win[win_fill++] = b;
            if (win_fill < RPLIDAR_NODE_SIZE) {
                continue;
            }
        } else {
            for (int i = 0; i < RPLIDAR_NODE_SIZE - 1; i++) {
                win[i] = win[i + 1];
            }
            win[RPLIDAR_NODE_SIZE - 1] = b;
        }

        float ang = 0.0f;
        float dist = 0.0f;
        uint8_t q = 0;

        if (!rplidar_parse_node_5b(win, &ang, &dist, &q)) {
            continue;
        }

        valid_nodes++;

        rplidar_point_t pt = {0};
        pt.angle_deg = ang;
        pt.distance_mm = dist;
        pt.quality = q;

        if (have_prev && ang < prev_angle) {
            pt.start_flag = true;
        }

        prev_angle = ang;
        have_prev = true;

        if (s_q) {
            if (xQueueSend(s_q, &pt, 0) == pdTRUE) {
                queue_sends++;
            }
        }

        if ((valid_nodes % 100) == 0) {
            ESP_LOGI(TAG,
                     "Valid node #%lu ang=%.2f dist=%.1f q=%u start=%d queue_sends=%lu",
                     (unsigned long)valid_nodes,
                     pt.angle_deg,
                     pt.distance_mm,
                     (unsigned)pt.quality,
                     (int)pt.start_flag,
                     (unsigned long)queue_sends);
        }
    }
}

QueueHandle_t rplidar_point_queue_start(void)
{
    if (s_q) {
        return s_q;
    }

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
