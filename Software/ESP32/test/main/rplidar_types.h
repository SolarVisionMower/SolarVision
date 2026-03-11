#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float   angle_deg;     // [0, 360)
    float   distance_mm;   // 0 if invalid
    uint8_t quality;       // 0..63
    bool    start_flag;    // true when angle wraps (new 360)
} rplidar_point_t;

typedef struct {
    uint32_t points_total;
    uint32_t points_valid;
    uint32_t frames;
} rplidar_stats_t;

typedef struct {
    float             x_mm;
    float             y_mm;
    rplidar_point_t * pt;
    bool              valid;
} lidar_xy_point_t;

#ifdef __cplusplus
}
#endif
