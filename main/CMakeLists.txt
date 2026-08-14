idf_component_register(SRCS "main.c"
                            "spi_pipeline/master/spi_master.c"
                            "spi_pipeline/slave/spi_slave.c"
                            "utils/forge_log.c"
                    
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    )