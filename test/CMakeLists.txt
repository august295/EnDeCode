include_directories(${ROOT_DIR}/3rdparty/googletest/googletest/include)
include_directories(${ROOT_DIR}/3rdparty/googletest/googlemock/include)
include_directories(${ROOT_DIR}/3rdparty/benchmark/include)

set(ProjectName "test")
CreateTarget(${ProjectName} "Exe" "test")

if(BUILD_SHARED_LIBS)
else()
    add_compile_definitions(ENDECODE_STATIC)
endif()

# 链接库
target_link_libraries(${ProjectName} "benchmark" "benchmark_main")
target_link_libraries(${ProjectName} "gtest" "gtest_main")
target_link_libraries(${ProjectName} "endecode")
