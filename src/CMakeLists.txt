# 依赖三方库
set(LibraryList "googletest" "cJSON")
AddLibrary("${LibraryList}")

set(ProjectName "endecode")
if(BUILD_SHARED_LIBS)
    add_compile_definitions(ENDECODE_EXPORT)
    CreateTarget(${ProjectName} "Dll" "core")
else()
    add_compile_definitions(ENDECODE_STATIC)
    CreateTarget(${ProjectName} "Lib" "core")
endif()

target_link_libraries(${ProjectName} "gtest")
target_link_libraries(${ProjectName} "cjson")

if(BUILD_VCPKG)
    target_link_libraries(${ProjectName} PkgConfig::gmp)
endif()
