
# ESP32-S3 + RPLIDAR Bringup (SolarVision Project)

## Overview
This document explains how the ESP32-S3 firmware communicates with the RPLIDAR sensor and how LiDAR scan data flows through the system.

The firmware performs the following tasks:

- Initializes UART communication with the RPLIDAR
- Sends commands to start and stop LiDAR scanning
- Reads raw measurement bytes from the sensor
- Parses packets into usable distance and angle measurements
- Streams measurements into a FreeRTOS queue
- Allows other tasks (mapping, navigation, obstacle detection) to consume LiDAR data

This stage is known as **sensor bringup**, meaning the hardware interface and communication pipeline are verified and working.

---

# System Architecture

High level data flow:

RPLIDAR Sensor  
↓ UART byte stream  
ESP32 UART Driver  
↓  
rplidar_task (FreeRTOS task)  
↓  
Packet Parser  
↓  
FreeRTOS Queue  
↓  
Application Layer (app_main)  

---

# Project Folder Structure

Typical ESP-IDF layout:

project/

├── CMakeLists.txt  
├── main/  
│   ├── CMakeLists.txt  
│   ├── app_main.c  
│   ├── config.h  
│   ├── rplidar.c  
│   ├── rplidar.h  
│   ├── rplidar_protocol.c  
│   ├── rplidar_protocol.h  
│   ├── rplidar_task.c  
│   ├── rplidar_task.h  
│   └── rplidar_types.h  

Two important build files exist:

Top-level CMakeLists.txt  
Starts the ESP-IDF build system and defines the project.

main/CMakeLists.txt  
Defines the component source files compiled into firmware.

---

# Core Modules

## app_main.c
Entry point of the firmware.

Responsibilities:
- Initialize the RPLIDAR UART interface
- Start the LiDAR reading task
- Consume scan points from the queue
- Print sample measurements for debugging

Example workflow:

1. Initialize UART
2. Start LiDAR task
3. Wait for scan points
4. Process measurements

---

## rplidar_task.c
Runs a background FreeRTOS task that:

- Creates a queue for scan points
- Starts LiDAR scanning
- Continuously reads UART bytes
- Assembles measurement packets
- Pushes decoded measurements into the queue

---

## rplidar_protocol.c
Handles decoding LiDAR packets.

Each packet contains:
- Angle measurement
- Distance measurement
- Signal quality
- Scan synchronization information

The parser converts raw bytes into usable data structures.

---

## rplidar.c
Handles communication commands sent to the LiDAR sensor.

Commands include:

STOP – stop scanning  
SCAN – begin continuous scanning

---

## rplidar_types.h
Defines shared data structures used across the project.

Main structure:

rplidar_point_t

Fields:
- angle_deg
- distance_mm
- quality
- start_flag

This structure represents a single LiDAR measurement point.

---

# Why a FreeRTOS Queue is Used

The LiDAR produces measurements continuously at a high rate.

Using a queue allows:

Producer Task (rplidar_task)  
→ quickly push measurements

Consumer Task (app_main)  
→ process measurements when ready

This prevents UART buffer overflow and improves system reliability.

---

# Expected Output

When the firmware works correctly, the console will show messages like:

pt: ang=45.3 deg dist=850.2 mm q=35 start=0  
pt: ang=46.0 deg dist=848.1 mm q=33 start=0  

Angle changes as the LiDAR rotates and distance corresponds to detected objects.

---

# Hardware Configuration

Example wiring:

ESP32 TX → RPLIDAR RX  
ESP32 RX → RPLIDAR TX  

Common baud rate: 115200

Pin configuration is defined in `config.h`.

---

# Troubleshooting

Common issues during bringup:

Incorrect UART wiring  
Wrong baud rate  
LiDAR motor not spinning  
Parser not synchronized with packet stream  

Symptoms may include:

No measurements printed  
Distance always zero  
Garbage values in output  

---

# Next Development Steps

With the LiDAR pipeline working, the next modules for SolarVision include:

1. Polar → Cartesian coordinate conversion
2. 2D occupancy grid mapping
3. Obstacle detection
4. Path planning and navigation

---

# Summary

This firmware establishes the LiDAR sensor interface layer for the SolarVision autonomous mower system.

The ESP32:

- Controls the RPLIDAR
- Streams measurement data
- Parses packets
- Provides structured scan data to higher-level robotics modules
