# Production - Shared/CMakeLists.txt
idf_component_register(SRCS "buffering/buf_mgr.c"
                            "utils/forge_log.c"
                            "utils/forge_err.c"
<<<<<<< HEAD
                            "utils/dma_master.c"
=======
                            "utils/dma_mgr.c"
>>>>>>> v1.1.0
                       INCLUDE_DIRS "."
                       REQUIRES driver)
