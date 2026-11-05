# 设置 vcpkg 配置
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-export/Windows")
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(VCPKG_TARGET_TRIPLET "x86-windows-v142")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-windows-v142")
    endif()
endif()
# 设置工具链文件
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
message(STATUS "VCPKG_ROOT:             ${VCPKG_ROOT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE:   ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET:   ${VCPKG_TARGET_TRIPLET}")

################################################################################
# gmp
################################################################################
macro(VCPKG_LOAD_3RDPARTY)
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(PKG_CONFIG_EXECUTABLE "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/tools/pkgconf/pkgconf.exe")
        set(PKG_CONFIG_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/pkgconfig")
    endif()

    message(STATUS "Loading 3rd party libraries via vcpkg...")
    if(BUILD_VCPKG)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    endif()
endmacro()
