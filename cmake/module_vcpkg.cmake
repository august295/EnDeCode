# 设置 vcpkg 相关配置
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
    # vcpkg install pkgconf
    set(PKG_CONFIG_EXECUTABLE "C:/dev/vcpkg/packages/pkgconf_x64-windows/tools/pkgconf/pkgconf.exe")
    # vcpkg install gmp:x64-windows-static
    set(ENV{PKG_CONFIG_PATH} "C:/dev/vcpkg/packages/gmp_x64-windows-static/lib/pkgconfig")
endif()

# {}_INCLUDE_DIRS
# {}_LINK_LIBRARIES
# {}_LDFLAGS_OTHER
# {}_CFLAGS_OTHER
# gmp
find_package(PkgConfig REQUIRED)
pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
