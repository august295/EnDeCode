# 设置 vcpkg 配置
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # 获取 VCPKG_ROOT
    if(NOT DEFINED VCPKG_ROOT)
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
        else()
            set(VCPKG_ROOT "C:/dev/vcpkg")
        endif()
    endif()

    # 设置工具链文件
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

    # 架构判断
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    else()
        set(VCPKG_TARGET_TRIPLET "x86-windows-static")
    endif()

    # 路径输出
    message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")

    # 加载工具链
    include(${CMAKE_TOOLCHAIN_FILE})
endif()

################################################################################
# gmp
#   vcpkg install gmp:x64-windows-static
################################################################################
if(BUILD_VCPKG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
endif()