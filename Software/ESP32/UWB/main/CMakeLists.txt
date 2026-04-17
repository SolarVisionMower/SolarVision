idf_component_register(
    SRCS
        "app_main.c"
        "config.c"
        "ss_init_run.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES deca_driver
)
