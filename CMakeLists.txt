# 要求最低Cmake版本
cmake_minimum_required(VERSION 3.15.0)

# 项目名称
set(UseProjectName "endecode")
project(${UseProjectName})

# C++标准
set(CMAKE_CXX_STANDARD 11)

# 条件编译
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options("/utf-8")
elseif()
endif()

# 开启使用文件夹功能，将所有默认目标放入到名为 CMakePredefinedTargets 的文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")

# 设置根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})

# 为多种构建类型设置输出目录
set(CONFIGURATION_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

foreach(CONFIGURATION_TYPE ${CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIGURATION_TYPE} TYPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/lib) # .lib and .a
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/lib) # .so and .dylib
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TYPE} ${BUILD_DIR}/bin) # .exe and .dll
endforeach()

# 后缀
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_MINSIZEREL_POSTFIX "m")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

# 选项
option(BUILD_SINGLE_LIBRARY "Build as a single shared library" OFF)
option(BUILD_TEST "Build test executable" OFF)

# vcpkg
include(${ROOT_DIR}/cmake/module_vcpkg.cmake)

# 算法库
include(${ROOT_DIR}/cmake/module.cmake)
add_subdirectory(src)

# 测试程序
if(BUILD_TEST)
    include(${ROOT_DIR}/cmake/module_gtest.cmake)
    add_subdirectory(test)
endif()

# 拷贝资源文件
add_custom_target(
    "CopyTask" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/resources" "${BUILD_DIR}/bin/"
    COMMENT "Copying resources"
)
set_property(TARGET "CopyTask" PROPERTY FOLDER "CMakePredefinedTargets")