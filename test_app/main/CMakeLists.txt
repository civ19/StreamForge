
idf_component_register(
    SRCS "main.c"
         "test_crc.c"

    INCLUDE_DIRS "."
            
    REQUIRES 
        spi_master_pipeline 
        spi_slave_pipeline 
        buffering 
        utils 
        unity 
        cmock
)

cmock_prepare_mock("C:/esp/v5.5.5/esp-idf/components/freertos/include/freertos/queue.h")