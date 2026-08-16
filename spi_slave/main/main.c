#include "spi_pipeline/spi_slave.h"
#include "esp_err.h"

#include "forge_err.h"


void app_main(void) {
    
    static const char* TAG = "MAIN_SLAVE";

    esp_err_t err;

    CHECK_ERR(err = init_slave_bus(), return err);
    
    CHECK_ERR(err = slave_transmit(), return err);

}