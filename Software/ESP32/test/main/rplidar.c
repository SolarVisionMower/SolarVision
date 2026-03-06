#include "rplidar.h"
#include "rplidar_protocol.h"   // <-- provides RPLIDAR_CMD_SYNC/STOP/SCAN
#include "config.h"

#include "driver/uart.h"
#include "esp_err.h"

static esp_err_t rplidar_uart_write_bytes(const uint8_t *data, int len)
{
    int w = uart_write_bytes(RPLIDAR_UART_NUM, (const char*)data, len);
    return (w == len) ? ESP_OK : ESP_FAIL;
}

esp_err_t rplidar_send_stop(void)
{
    const uint8_t cmd[2] = { RPLIDAR_CMD_SYNC, RPLIDAR_CMD_STOP };
    return rplidar_uart_write_bytes(cmd, sizeof(cmd));
}

esp_err_t rplidar_send_scan(void)
{
    const uint8_t cmd[2] = { RPLIDAR_CMD_SYNC, RPLIDAR_CMD_SCAN };
    return rplidar_uart_write_bytes(cmd, sizeof(cmd));
}
