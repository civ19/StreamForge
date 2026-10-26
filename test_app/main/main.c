#include "unity.h"

extern void test_dma_alloc_returns_null_cleanly(void);


void app_main(void) {
    
    UNITY_BEGIN();
    RUN_TEST(test_dma_alloc_returns_null_cleanly);

    UNITY_END();
}