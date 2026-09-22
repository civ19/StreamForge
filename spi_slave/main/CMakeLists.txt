idf_component_register(SRCS "main.c"
                            "spi_slave_pipeline/spi_slave.c"
                            "../../shared/utils/forge_log.c"
                            "../../shared/utils/dma_mgr.c"
                            "buffering/buf_mgr.c"
                    
                    INCLUDE_DIRS "."
                                 "spi_slave_pipeline"
                                 "../../shared/utils"
                    REQUIRES driver
                             esp_timer
                             esp_rom
                    )
