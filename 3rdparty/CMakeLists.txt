set(PREFIX_FOLDER "3rdparty")

################################################################################
# googletest
################################################################################
if(BUILD_TEST)
    add_subdirectory(${ROOT_DIR}/${PREFIX_FOLDER}/googletest)

    set_property(TARGET "gtest" PROPERTY FOLDER "${PREFIX_FOLDER}/googletest")
    set_property(TARGET "gtest_main" PROPERTY FOLDER "${PREFIX_FOLDER}/googletest")
    set_property(TARGET "gmock" PROPERTY FOLDER "${PREFIX_FOLDER}/googletest")
    set_property(TARGET "gmock_main" PROPERTY FOLDER "${PREFIX_FOLDER}/googletest")
endif()

####################################################################################################
# benchmark
####################################################################################################
if(BUILD_TEST)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(${ROOT_DIR}/${PREFIX_FOLDER}/benchmark)

    set_property(TARGET "benchmark" PROPERTY FOLDER "${PREFIX_FOLDER}/benchmark")
    set_property(TARGET "benchmark_main" PROPERTY FOLDER "${PREFIX_FOLDER}/benchmark")
endif()

################################################################################
# cJSON
################################################################################
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
set(ENABLE_CJSON_UNINSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${ROOT_DIR}/${PREFIX_FOLDER}/cJSON)

set_property(TARGET "cjson" PROPERTY FOLDER "${PREFIX_FOLDER}/cJSON")
