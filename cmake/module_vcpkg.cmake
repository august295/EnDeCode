# 设置 vcpkg 配置
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    if(NOT DEFINED VCPKG_ROOT)
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
        else()
            set(VCPKG_ROOT "C:/dev/vcpkg")
        endif()
    endif()

    # 设置工具链文件
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    set(PKG_CONFIG_EXECUTABLE "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/tools/pkgconf/pkgconf.exe")
    set(PKG_CONFIG_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/pkgconfig")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    if(NOT DEFINED VCPKG_ROOT)
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
        else()
            set(VCPKG_ROOT "$ENV{HOME}/vcpkg")
        endif()
    endif()

    # 设置工具链文件
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-linux")
    set(PKG_CONFIG_EXECUTABLE "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/tools/pkgconf/pkgconf")
    set(PKG_CONFIG_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/pkgconfig")
endif()

################################################################################
# gmp
#   vcpkg install gmp:x64-windows-static
################################################################################
macro(VCPKG_LOAD_3RDPARTY)
    message(STATUS "Loading 3rd party libraries via vcpkg...")
    if(BUILD_VCPKG)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    endif()
endmacro()
