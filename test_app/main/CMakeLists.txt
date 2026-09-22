get_filename_component(MASTER_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../spi_master/main" ABSOLUTE)
get_filename_component(SLAVE_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../spi_slave/main" ABSOLUTE)
get_filename_component(SHARED_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../shared/utils" ABSOLUTE)

idf_component_register(
    SRCS "main.c"
         "test_crc.c"
         "${MASTER_MAIN_DIR}/spi_master_pipeline/spi_master.c"
         "${SLAVE_MAIN_DIR}/spi_slave_pipeline/spi_slave.c"
         "${SLAVE_MAIN_DIR}/buffering/buf_mgr.c"                 # Slave Buffer Manager
         "${SHARED_UTILS_DIR}/dma_mgr.c"                         # Shared Utilities
         "${SHARED_UTILS_DIR}/forge_log.c"
    INCLUDE_DIRS "."
                 "${MASTER_MAIN_DIR}/spi_master_pipeline"               # Exposes headers
                 "${SLAVE_MAIN_DIR}/spi_slave_pipeline"
                 "${SLAVE_MAIN_DIR}/buffering"
                 "${SLAVE_MAIN_DIR}"
                 "${SHARED_UTILS_DIR}"  
    PRIV_REQUIRES esp_rom
                  unity
                  esp_timer
                  driver
                  esp_driver_spi
)