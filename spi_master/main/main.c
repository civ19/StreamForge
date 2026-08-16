#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "spi_pipeline/spi_master.h"

#include "utils/forge_err.h"



void app_main(void) {

    static const char *TAG = "MAIN_MASTER";
    
    esp_err_t err;

    CHECK_ERR(err = init_spi_bus(), return err);
    CHECK_ERR(err = init_spi_devs(), return err);
    
    CHECK_ERR(err = master_trasmit(), return err);

}