idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi_slave.c"
                            "../../shared/utils/forge_log.c"
                    
                    INCLUDE_DIRS "."
                                 "spi_pipeline"
                                 "../../shared/utils"
                    REQUIRES driver
                    )