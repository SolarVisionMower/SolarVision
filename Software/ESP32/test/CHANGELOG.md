# CHANGELOG

All notable changes to the Lidar portion of this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## March 2026

Objectives are to:

- Remove the 200 data point printing restriction (print every 200 data points)
- Read COM port and display 2D map of surroundings
- Begin identifying a good data structure format across which to transmit Lidar and UWB data to the STM

### TODO:

- Log all data points via UART to the COM port.

### Added

### Changed

- Moved definition of `lidar_xy_point_t` from `lidar_xy.h` to `rplidar_types.h`
- Replaced angle, distance, quality, and start flag in `lidar_xy_point_t`
    - This simplifies the code and reduces duplicates in memory
    - It will also let `lidar_polar_to_xy` run much faster (reduced copying via pointers).
- Converted `rplidar_parse_node_5b` window to a ring buffer *with index* (as new parameter).

### Removed