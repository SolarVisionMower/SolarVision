#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lidar_xy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRID_WIDTH       200
#define GRID_HEIGHT      200
#define GRID_CELL_MM     50.0f

#define GRID_UNKNOWN     0
#define GRID_OCCUPIED    1

typedef struct {
    uint8_t cells[GRID_HEIGHT][GRID_WIDTH];
} occupancy_grid_t;

void occupancy_grid_init(occupancy_grid_t *grid);
void occupancy_grid_clear(occupancy_grid_t *grid);

bool occupancy_grid_xy_to_cell(float x_mm, float y_mm, int *grid_x, int *grid_y);
bool occupancy_grid_mark_xy(occupancy_grid_t *grid, const lidar_xy_point_t *pt);
uint8_t occupancy_grid_get_cell(const occupancy_grid_t *grid, int grid_x, int grid_y);

#ifdef __cplusplus
}
#endif
