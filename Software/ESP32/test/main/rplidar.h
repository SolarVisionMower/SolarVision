#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Send STOP and SCAN commands
esp_err_t rplidar_send_stop(void);
esp_err_t rplidar_send_scan(void);

#ifdef __cplusplus
}
#endif
