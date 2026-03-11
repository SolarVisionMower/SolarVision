#include "rplidar_protocol.h"

/*
Normal scan measurement node format (5 bytes) (common A1 mode):
  byte0: sync & quality
        bit0 = 1
        bit1 = 0  (inverse sync)
        bits2..7 = quality (0..63)
  byte1: angle_q6 low 7 bits in bits1..7, bit0 = checkbit (should be 1)
  byte2: angle_q6 high 8 bits
  byte3: distance_q2 low 8 bits
  byte4: distance_q2 high 8 bits

angle_deg = (angle_q6 / 64.0)
distance_mm = (distance_q2 / 4.0)

window_position provides the index for the node ring-buffer
*/

bool rplidar_parse_node_5b(const uint8_t node[5],
                           uint8_t window_position,
                           float *angle_deg,
                           float *dist_mm,
                           uint8_t *quality)
{
    // const uint8_t b0 = node[0];
    // const uint8_t b1 = node[1];
    // const uint8_t b2 = node[2];
    // const uint8_t b3 = node[3];
    // const uint8_t b4 = node[4];
    const uint8_t b0 = node[(window_position + 1) % 5];
    const uint8_t b1 = node[(window_position + 2) % 5];
    const uint8_t b2 = node[(window_position + 3) % 5];
    const uint8_t b3 = node[(window_position + 4) % 5];
    const uint8_t b4 = node[window_position];

    // sync bits must be 1 and 0 respectively
    const bool sync1 = (b0 & 0x01) != 0;
    const bool sync2 = (b0 & 0x02) != 0;
    if (!sync1 || sync2) return false;

    // checkbit in b1 bit0 should be 1
    if ((b1 & 0x01) == 0) return false;

    // 6-bit quality
    uint8_t q = (uint8_t)(b0 >> 2); 

    // angle_q6: (b1 bits1..7) are low 7 bits, b2 is high 8 bits
    uint16_t angle_q6 = (uint16_t)((b1 >> 1) | ((uint16_t)b2 << 7));

    // distance_q2: b3 low, b4 high
    uint16_t dist_q2 = (uint16_t)(b3 | ((uint16_t)b4 << 8));

    float ang = ((float)angle_q6) / 64.0f;
    float dist = (dist_q2 == 0) ? 0.0f : ((float)dist_q2) / 4.0f;

    if (angle_deg) *angle_deg = ang;
    if (dist_mm)   *dist_mm = dist;
    if (quality)   *quality = q;

    return true;
}
