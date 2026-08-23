idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi_slave.c"
                            "../../shared/utils/forge_log.c"
<<<<<<< HEAD
                            "../../shared/utils/dma_master.c"
=======
                            "../../shared/utils/dma_mgr.c"
>>>>>>> v1.1.0
                            "buffering/buf_mgr.c"
                    
                    INCLUDE_DIRS "."
                                 "spi_pipeline"
                                 "../../shared/utils"
                    REQUIRES driver
                             esp_timer
                    )
