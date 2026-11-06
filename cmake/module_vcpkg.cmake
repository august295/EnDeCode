macro(VCPKG_BEFORE)
endmacro()

macro(VCPKG_AFTER)
    # 设置 vcpkg 配置
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            set(VCPKG_TARGET_TRIPLET "x86-windows-v142")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-windows-v142")
        endif()
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            set(VCPKG_TARGET_TRIPLET "arm64-linux" CACHE STRING "")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
            set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
        else()
            message(WARNING "Unsupported Linux architecture: ${CMAKE_SYSTEM_PROCESSOR}")
        endif()
    endif()
    # 设置工具链文件
    message(STATUS "VCPKG_TARGET_TRIPLET:   ${VCPKG_TARGET_TRIPLET}")
endmacro()

################################################################################
# gmp
################################################################################
macro(VCPKG_LOAD_3RDPARTY)
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(GMP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-export/Windows/installed/${VCPKG_TARGET_TRIPLET}")

        add_library(vcpkg-export::gmp SHARED IMPORTED)
        set_target_properties(vcpkg-export::gmp PROPERTIES
            IMPORTED_LOCATION "${GMP_ROOT}/bin/gmp-10.dll" # 运行时 DLL
            IMPORTED_IMPLIB "${GMP_ROOT}/lib/gmp.lib" # 链接用导入库
            INTERFACE_INCLUDE_DIRECTORIES "${GMP_ROOT}/include"
        )
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        set(GMP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg-export/Linux/installed/${VCPKG_TARGET_TRIPLET}")

        add_library(vcpkg-export::gmp SHARED IMPORTED)
        set_target_properties(vcpkg-export::gmp PROPERTIES
            IMPORTED_LOCATION "${GMP_ROOT}/lib/libgmp.a"
            INTERFACE_INCLUDE_DIRECTORIES "${GMP_ROOT}/include"
        )
    endif()
endmacro()
