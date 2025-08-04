set(ProjectName "endecode")
if(BUILD_SHARED_LIBS)
    CreateTarget(${ProjectName} "Dll" "core")
    target_compile_definitions(${ProjectName} PRIVATE -DENDECODE_EXPORT)
else()
    CreateTarget(${ProjectName} "Lib" "core")
    target_compile_definitions(${ProjectName} PUBLIC -DENDECODE_STATIC)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_link_libraries(${ProjectName} PUBLIC
            $<$<CONFIG:Debug>:msvcrtd>
            $<$<CONFIG:Release>:msvcrt>
        )
    endif()
endif()

if(BUILD_VCPKG)
    target_link_libraries(${ProjectName} PkgConfig::gmp)
endif()
