# Production - Shared/CMakeLists.txt
idf_component_register(SRCS "buffering/buf_mgr.c"
                            "utils/forge_log.c"
                            "utils/forge_err.c"
                            "utils/dma_master.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver)
