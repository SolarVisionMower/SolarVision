#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// RPLIDAR command bytes
#define RPLIDAR_CMD_SYNC      (0xA5)
#define RPLIDAR_CMD_SCAN      (0x20)
#define RPLIDAR_CMD_STOP      (0x25)

// A single "normal scan" measurement node is 5 bytes.
#define RPLIDAR_NODE_SIZE     (5)

// Try to parse one node from a rolling 5-byte window.
// Returns true if valid and fills:
//  - angle_deg [0..360)
//  - dist_mm (0 if invalid)
//  - quality 0..63
bool rplidar_parse_node_5b(const uint8_t node[RPLIDAR_NODE_SIZE],
                           uint8_t window_position,
                           float *angle_deg,
                           float *dist_mm,
                           uint8_t *quality);

#ifdef __cplusplus
}
#endif
