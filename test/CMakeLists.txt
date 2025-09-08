include_directories(${ROOT_DIR}/3rdparty/googletest/googletest/include)
include_directories(${ROOT_DIR}/3rdparty/googletest/googlemock/include)
include_directories(${ROOT_DIR}/3rdparty/benchmark/include)

set(ProjectName "test")
CreateTarget(${ProjectName} "Exe" "test")

# 链接库
target_link_libraries(${ProjectName} "benchmark" "benchmark_main")
target_link_libraries(${ProjectName} "gtest" "gtest_main")
target_link_libraries(${ProjectName} "endecode")

# 拷贝测试文件
add_custom_target(
    "${UseProjectName}TestCopyTask" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/test/resources" "${BUILD_DIR}/bin/"
    COMMENT "Copying test resources"
)
set_property(TARGET "${UseProjectName}TestCopyTask" PROPERTY FOLDER "CMakePredefinedTargets")