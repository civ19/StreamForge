idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi.c"
                            "utils/forge_log.c"
                    
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    )