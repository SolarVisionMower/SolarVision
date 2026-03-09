#include "obstacle_detection.h"

#include <stddef.h>

// All units are in millimeters

// Front zone:
// x > 0 means in front of the mower
// |y| <= half width means roughly centered in front
#define FRONT_MIN_X_MM       100.0f
#define FRONT_MAX_X_MM      1000.0f
#define FRONT_HALF_WIDTH_MM  400.0f

// Left zone:
// y > 0 means left side
#define SIDE_MIN_X_MM          0.0f
#define SIDE_MAX_X_MM        800.0f
#define SIDE_MIN_Y_MM        200.0f
#define SIDE_MAX_Y_MM        800.0f

// Right zone:
// y < 0 means right side
#define RIGHT_MIN_Y_MM      -800.0f
#define RIGHT_MAX_Y_MM      -200.0f

// Emergency stop zone: very close obstacle directly ahead
#define STOP_MIN_X_MM         50.0f
#define STOP_MAX_X_MM        300.0f
#define STOP_HALF_WIDTH_MM   250.0f

void obstacle_state_reset(obstacle_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->front_blocked = false;
    state->left_blocked = false;
    state->right_blocked = false;
    state->stop_required = false;

    state->front_hits = 0;
    state->left_hits = 0;
    state->right_hits = 0;
    state->stop_hits = 0;
}

bool obstacle_process_point(obstacle_state_t *state, const lidar_xy_point_t *pt)
{
    if (state == NULL || pt == NULL || !pt->valid) {
        return false;
    }

    float x = pt->x_mm;
    float y = pt->y_mm;
    bool hit = false;

    // Ignore poor quality or zero distance style junk if desired
    if (pt->distance_mm <= 0.0f) {
        return false;
    }

    // Front zone
    if (x >= FRONT_MIN_X_MM &&
        x <= FRONT_MAX_X_MM &&
        y >= -FRONT_HALF_WIDTH_MM &&
        y <= FRONT_HALF_WIDTH_MM) {
        state->front_blocked = true;
        state->front_hits++;
        hit = true;
    }

    // Left zone
    if (x >= SIDE_MIN_X_MM &&
        x <= SIDE_MAX_X_MM &&
        y >= SIDE_MIN_Y_MM &&
        y <= SIDE_MAX_Y_MM) {
        state->left_blocked = true;
        state->left_hits++;
        hit = true;
    }

    // Right zone
    if (x >= SIDE_MIN_X_MM &&
        x <= SIDE_MAX_X_MM &&
        y >= RIGHT_MIN_Y_MM &&
        y <= RIGHT_MAX_Y_MM) {
        state->right_blocked = true;
        state->right_hits++;
        hit = true;
    }

    // Emergency stop zone
    if (x >= STOP_MIN_X_MM &&
        x <= STOP_MAX_X_MM &&
        y >= -STOP_HALF_WIDTH_MM &&
        y <= STOP_HALF_WIDTH_MM) {
        state->stop_required = true;
        state->stop_hits++;
        hit = true;
    }

    return hit;
}