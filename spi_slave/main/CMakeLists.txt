idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi_slave.c"
                            "../../shared/utils/forge_log.c"
                            "../../shared/utils/dma_master.c"
                            "../../shared/buffering/buf_mgr.c"
                    
                    INCLUDE_DIRS "."
                                 "spi_pipeline"
                                 "../../shared/utils"
                                 "../../shared/buffering"
                    REQUIRES driver
                    )