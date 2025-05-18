include_directories(${ROOT_DIR}/3rdparty/cJSON)

# 预编译
set(ProjectName "asn1")
add_compile_definitions(ENDECODE_EXPORT)
CreateTarget(${ProjectName} "Dll" "asn")

if(BUILD_SINGLE_LIBRARY)
else()
    target_link_libraries(${ProjectName} base64)
    target_link_libraries(${ProjectName} cjson)
endif()