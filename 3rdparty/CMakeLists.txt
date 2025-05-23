################################################################################
# googletest
################################################################################
add_subdirectory(${ROOT_DIR}/3rdparty/googletest)

set_property(TARGET "gtest" PROPERTY FOLDER "3rdparty/googletest")
set_property(TARGET "gtest_main" PROPERTY FOLDER "3rdparty/googletest")
set_property(TARGET "gmock" PROPERTY FOLDER "3rdparty/googletest")
set_property(TARGET "gmock_main" PROPERTY FOLDER "3rdparty/googletest")

################################################################################
# cJSON
################################################################################
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
set(ENABLE_CJSON_UNINSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${ROOT_DIR}/3rdparty/cJSON)

set_property(TARGET "cjson" PROPERTY FOLDER "3rdparty/cJSON")
