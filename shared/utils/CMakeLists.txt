# Production - Shared/CMakeLists.txt
idf_component_register(SRCS "forge_log.c"
                            "dma_mgr.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver)
