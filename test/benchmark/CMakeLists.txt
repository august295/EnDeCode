include_directories(${ROOT_DIR}/3rdparty/benchmark/include)

set(ProjectName "test_benchmark")
CreateTarget(${ProjectName} "Exe" "test")

# 链接库
target_link_libraries(${ProjectName} "benchmark" "benchmark_main")
target_link_libraries(${ProjectName} "endecode")
