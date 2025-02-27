# 预编译
add_compile_definitions(ENDECODE_EXPORT)
CreateTarget(asn1 "Dll" "asn")

if(BUILD_SINGLE_LIBRARY)
else()
	target_link_libraries(asn1 base64)
endif()