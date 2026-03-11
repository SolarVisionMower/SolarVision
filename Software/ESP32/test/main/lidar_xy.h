#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rplidar_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Convert a point from polar to cartesian
bool lidar_polar_to_xy(const rplidar_point_t *in, lidar_xy_point_t *out);

#ifdef __cplusplus
}
#endif
