# 要求最低Cmake版本
cmake_minimum_required(VERSION 3.15.0)

# 项目名称
project(EnDeCode)

# C++标准
set(CMAKE_CXX_STANDARD 11)

# 条件编译
if(CMAKE_CXX_PLATFORM_ID MATCHES "Windows")
    add_compile_options("/utf-8")
    add_compile_definitions(_WIN32_WINNT=0x0601)
elseif(CMAKE_CXX_PLATFORM_ID MATCHES "MinGW")
    add_compile_options("-Wa,-mbig-obj")
elseif(CMAKE_CXX_PLATFORM_ID MATCHES "Linux")
    add_compile_options("-fPIC")
endif()

# 开启使用文件夹功能，将所有默认目标放入到名为 CMakePredefinedTargets 的文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")

# 设置根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${ROOT_DIR}/build)

# 输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/lib) # .lib and .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/lib) # .so and .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR}/bin) # .exe and .dll
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/lib) # .lib and .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/lib) # .so and .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR}/bin) # .exe and .dll

# 算法库
include(${ROOT_DIR}/cmake/module.cmake)
add_subdirectory(src)

# 测试程序
include(${ROOT_DIR}/cmake/module_gtest.cmake)
add_subdirectory(test)