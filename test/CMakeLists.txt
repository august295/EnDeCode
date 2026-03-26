add_subdirectory(unit)
add_subdirectory(benchmark)

# 拷贝测试文件
add_custom_target(
    "${UseProjectName}TestCopyTask" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/test/resources" "${BUILD_DIR}/bin/"
    COMMENT "Copying test resources"
)
set_property(TARGET "${UseProjectName}TestCopyTask" PROPERTY FOLDER "CMakePredefinedTargets")