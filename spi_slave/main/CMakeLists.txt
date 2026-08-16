idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi_master.c"
                            "../../shared/utils/forge_log.c"
                    
                    INCLUDE_DIRS "."
                                 "../../shared/utils"
                    REQUIRES driver
                    )