#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lidar_xy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool front_blocked;
    bool left_blocked;
    bool right_blocked;
    bool stop_required;

    uint32_t front_hits;
    uint32_t left_hits;
    uint32_t right_hits;
    uint32_t stop_hits;
} obstacle_state_t;

void obstacle_state_reset(obstacle_state_t *state);
bool obstacle_process_point(obstacle_state_t *state, const lidar_xy_point_t *pt);

#ifdef __cplusplus
}
#endif