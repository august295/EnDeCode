# 要求最低Cmake版本
cmake_minimum_required(VERSION 3.15.0)

# 项目名称
set(UseProjectName "endecode")
project(${UseProjectName}
    VERSION 0.0.0.1
    LANGUAGES CXX
)

# C++标准
set(CMAKE_CXX_STANDARD 11)

# 条件编译
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options("/utf-8")
    add_compile_definitions(OLE2ANSI)
elseif()
endif()

# 开启使用文件夹功能，将所有默认目标放入到名为 CMakePredefinedTargets 的文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")

# 设置根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})

# 默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# 为多种构建类型设置输出目录
set(CONFIGURATION_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

foreach(CONFIGURATION_TYPE ${CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIGURATION_TYPE} TYPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/lib) # .lib and .a
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/bin) # .so and .dylib
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/bin) # .exe and .dll
endforeach()

# 后缀
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_MINSIZEREL_POSTFIX "m")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

# 选项
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
option(BUILD_VCPKG "Build use vcpkg" OFF)
option(BUILD_TEST "Build test executable" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# vcpkg
if(BUILD_VCPKG)
    include(${ROOT_DIR}/cmake/module_vcpkg.cmake)
    add_compile_definitions(ENABLE_GMP)
endif(BUILD_VCPKG)

# 三方库
add_subdirectory(3rdparty)

# 算法库
include(${ROOT_DIR}/cmake/module.cmake)
add_subdirectory(src)

# 测试程序
if(BUILD_TEST)
    add_subdirectory(test)
endif()

# 拷贝资源文件
add_custom_target(
    "CopyTask" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/resources" "${BUILD_DIR}/bin/"
    COMMENT "Copying resources"
)
set_property(TARGET "CopyTask" PROPERTY FOLDER "CMakePredefinedTargets")

# 安装
# 头文件
install(DIRECTORY ${ROOT_DIR}/include/${UseProjectName} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# 库文件
install(TARGETS ${UseProjectName}
    EXPORT ${UseProjectName}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
# 导出文件
install(EXPORT ${UseProjectName}Targets
    FILE ${UseProjectName}Targets.cmake
    NAMESPACE ${UseProjectName}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${UseProjectName}
)
# 配置文件
if(BUILD_TEST)
    set(GTEST_DEPENDENCY_CODE "find_dependency(GTest REQUIRED)")
    set(BENCHMARK_DEPENDENCY_CODE "find_dependency(benchmark REQUIRED)")
else()
    set(GTEST_DEPENDENCY_CODE "")
    set(BENCHMARK_DEPENDENCY_CODE "")
endif()
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${ROOT_DIR}/cmake/${UseProjectName}Config.cmake.in
    ${BUILD_DIR}/${UseProjectName}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${UseProjectName}
)
write_basic_package_version_file(
    ${BUILD_DIR}/${UseProjectName}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
    ${BUILD_DIR}/${UseProjectName}Config.cmake
    ${BUILD_DIR}/${UseProjectName}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${UseProjectName}
)
