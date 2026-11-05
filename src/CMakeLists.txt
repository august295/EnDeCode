set(ProjectName "endecode")
if(BUILD_SHARED_LIBS)
    CreateTarget(${ProjectName} "Dll" "core")
    target_compile_definitions(${ProjectName} PRIVATE -DENDECODE_EXPORT)
else()
    CreateTarget(${ProjectName} "Lib" "core")
    target_compile_definitions(${ProjectName} PUBLIC -DENDECODE_STATIC)
endif()

target_link_libraries(${ProjectName} vcpkg-export::gmp)

add_custom_command(TARGET ${ProjectName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GMP_ROOT}/bin/gmp-10.dll"
        "$<TARGET_FILE_DIR:${ProjectName}>"
    COMMENT "Copying gmp to output directory"
)