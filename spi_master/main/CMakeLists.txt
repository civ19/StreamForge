idf_component_register(SRCS "main.c"
                            "spi_pipeline/spi_master.c"
                            "../../shared/utils/forge_log.c"
                            "../../shared/utils/dma_master.c"
                    
                    INCLUDE_DIRS "."
                                 "spi_pipeline"
                                 "../../shared/utils"
                                 
                    REQUIRES driver
                             esp_driver_spi
                             
                    )