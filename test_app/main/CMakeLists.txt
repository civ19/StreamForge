# 1. 🏆 THE COMPATIBLE MOCK GENERATOR (The Official ESP-IDF Way):
# Pass the explicit header files you want CMock to generate.
# This macro handles the code generation AND registers the component under the hood.
idf_component_mock(
    MOCK_HEADER_FILES 
        "C:/esp/v5.5.5/esp-idf/components/freertos/include/freertos/queue.h"
    INCLUDE_DIRS
        "."
    REQUIRES 
        spi_master_pipeline 
        spi_slave_pipeline 
        buffering 
        utils 
        unity
)


idf_component_register(
    SRCS 
        "main.c"
        "test_crc.c"
    INCLUDE_DIRS 
        "."
    PRIV_REQUIRES
        # Links your main app runner against the generated mocks
        main
)
