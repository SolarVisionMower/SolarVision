#include "occupancy_grid.h"

#include <stddef.h>
#include <string.h>

#define GRID_CENTER_X   (GRID_WIDTH / 2)
#define GRID_CENTER_Y   (GRID_HEIGHT / 2)

void occupancy_grid_init(occupancy_grid_t *grid)
{
    if (grid == NULL) {
        return;
    }

    occupancy_grid_clear(grid);
}

void occupancy_grid_clear(occupancy_grid_t *grid)
{
    if (grid == NULL) {
        return;
    }

    memset(grid->cells, GRID_UNKNOWN, sizeof(grid->cells));
}

bool occupancy_grid_xy_to_cell(float x_mm, float y_mm, int *grid_x, int *grid_y)
{
    if (grid_x == NULL || grid_y == NULL) {
        return false;
    }

    int gx = (int)(x_mm / GRID_CELL_MM) + GRID_CENTER_X;
    int gy = (int)(y_mm / GRID_CELL_MM) + GRID_CENTER_Y;

    if (gx < 0 || gx >= GRID_WIDTH || gy < 0 || gy >= GRID_HEIGHT) {
        return false;
    }

    *grid_x = gx;
    *grid_y = gy;
    return true;
}

bool occupancy_grid_mark_xy(occupancy_grid_t *grid, const lidar_xy_point_t *pt)
{
    if (grid == NULL || pt == NULL || !pt->valid) {
        return false;
    }

    int gx = 0;
    int gy = 0;

    if (!occupancy_grid_xy_to_cell(pt->x_mm, pt->y_mm, &gx, &gy)) {
        return false;
    }

    grid->cells[gy][gx] = GRID_OCCUPIED;
    return true;
}

uint8_t occupancy_grid_get_cell(const occupancy_grid_t *grid, int grid_x, int grid_y)
{
    if (grid == NULL) {
        return GRID_UNKNOWN;
    }

    if (grid_x < 0 || grid_x >= GRID_WIDTH || grid_y < 0 || grid_y >= GRID_HEIGHT) {
        return GRID_UNKNOWN;
    }

    return grid->cells[grid_y][grid_x];
}
