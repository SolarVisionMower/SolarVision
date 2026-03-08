#include "lidar_xy.h"

#include <math.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool lidar_polar_to_xy(const rplidar_point_t *in, lidar_xy_point_t *out)
{
    if (in == NULL || out == NULL) {
        return false;
    }

    out->angle_deg = in->angle_deg;
    out->distance_mm = in->distance_mm;
    out->quality = in->quality;
    out->start_flag = in->start_flag;
    out->valid = false;
    out->x_mm = 0.0f;
    out->y_mm = 0.0f;

    // Reject invalid or zero-distance points
    if (in->distance_mm <= 0.0f) {
        return false;
    }

    float theta_rad = in->angle_deg * ((float)M_PI / 180.0f);

    out->x_mm = in->distance_mm * cosf(theta_rad);
    out->y_mm = in->distance_mm * sinf(theta_rad);
    out->valid = true;

    return true;
}


