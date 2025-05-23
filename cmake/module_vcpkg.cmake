# 设置 vcpkg 配置
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # 获取 VCPKG_ROOT
    if(NOT DEFINED VCPKG_ROOT)
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
        else()
            set(VCPKG_ROOT "C:/dev/vcpkg")
            message(STATUS "Using default VCPKG_ROOT: ${VCPKG_ROOT}")
        endif()
    endif()

    # 设置工具链文件
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

    # 架构判断
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(VCPKG_TRIPLET "x64-windows-static")
    else()
        set(VCPKG_TRIPLET "x86-windows-static")
    endif()

    # 设置 pkgconf 路径和 pkgconfig 路径
    set(ENV{PKG_CONFIG} "${VCPKG_ROOT}/installed/${VCPKG_TRIPLET}/tools/pkgconf/pkgconf.exe")
    set(ENV{PKG_CONFIG_PATH} "${VCPKG_ROOT}/installed/${VCPKG_TRIPLET}/lib/pkgconfig")
    message(STATUS "Using PKG_CONFIG: $ENV{PKG_CONFIG}")
    message(STATUS "Using PKG_CONFIG_PATH: $ENV{PKG_CONFIG_PATH}")
endif()

################################################################################
# gmp
################################################################################
if(BUILD_VCPKG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    include_directories(${gmp_INCLUDE_DIRS})
endif()