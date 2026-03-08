#pragma once

#include "driver/uart.h"

// ---- UART wiring (change to your actual pins) ----
#define RPLIDAR_UART_NUM      UART_NUM_1
#define RPLIDAR_UART_TX_PIN   (17)   // ESP32 TX -> RPLIDAR RX
#define RPLIDAR_UART_RX_PIN   (18)   // ESP32 RX <- RPLIDAR TX

// Common baud rates: 115200 (many A1 kits), sometimes 256000
#define RPLIDAR_UART_BAUD     (115200)

#define RPLIDAR_UART_RX_BUF   (8 * 1024)
#define RPLIDAR_UART_TX_BUF   (1 * 1024)

// Queue/task sizing
#define RPLIDAR_POINT_QUEUE_LEN   (2048)
#define RPLIDAR_TASK_STACK        (4096)
#define RPLIDAR_TASK_PRIO         (10)
