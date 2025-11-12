# 设置 vcpkg 配置
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

# 路径输出
message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")

################################################################################
# gmp
#   vcpkg install gmp:x64-windows-static
################################################################################
function(VCPKG_LOAD_3RDPARTY)
    if(BUILD_VCPKG)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    endif()
endfunction()
