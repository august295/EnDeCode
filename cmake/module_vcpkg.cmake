# 设置 vcpkg 配置
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(VCPKG_TARGET_TRIPLET "x86-windows-v142")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-windows-v142")
    endif()
endif()
# 设置工具链文件
message(STATUS "VCPKG_TARGET_TRIPLET:   ${VCPKG_TARGET_TRIPLET}")

################################################################################
# gmp
################################################################################
macro(VCPKG_LOAD_3RDPARTY)
    set(GMP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-export/Windows/installed/${VCPKG_TARGET_TRIPLET}")

    add_library(vcpkg-export::gmp SHARED IMPORTED)
    set_target_properties(vcpkg-export::gmp PROPERTIES
        IMPORTED_LOCATION "${GMP_ROOT}/bin/gmp-10.dll" # 运行时 DLL
        IMPORTED_IMPLIB "${GMP_ROOT}/lib/gmp.lib" # 链接用导入库
        INTERFACE_INCLUDE_DIRECTORIES "${GMP_ROOT}/include"
    )
endmacro()
