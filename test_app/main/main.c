#include "unity.h"

extern void test_dma_alloc_returns_null_cleanly(void);
extern void test_wrong_magic_byte_drops(void);

void app_main(void) {
    
    UNITY_BEGIN();
    RUN_TEST(test_dma_alloc_returns_null_cleanly);
    RUN_TEST(test_wrong_magic_byte_drops);

    UNITY_END();
}