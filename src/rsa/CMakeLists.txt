# 预编译
add_compile_definitions(ENDECODE_EXPORT)
CreateTarget(rsa "Dll" "asymmetric_key")

include_directories(${gmp_INCLUDE_DIRS})
target_link_libraries(rsa PkgConfig::gmp)