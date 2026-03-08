#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rplidar_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x_mm;
    float y_mm;
    float angle_deg;
    float distance_mm;
    uint8_t quality;
    bool start_flag;
    bool valid;
} lidar_xy_point_t;

bool lidar_polar_to_xy(const rplidar_point_t *in, lidar_xy_point_t *out);

#ifdef __cplusplus
}
#endif
