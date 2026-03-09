idf_component_register(
    SRCS
        "app_main.c"
        "rplidar.c"
        "rplidar_protocol.c"
        "rplidar_task.c"
        "lidar_xy.c"
        "occupancy_grid.c"
	"obstacle_detection.c"
    INCLUDE_DIRS "."
)
