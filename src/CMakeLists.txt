# cpoy common
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/src/common/" "${ROOT_DIR}/include/common/")

# unknown
add_subdirectory(base64)
add_subdirectory(crc)

# digest
add_subdirectory(md5)

# symmetric_key
add_subdirectory(a5_1)
add_subdirectory(rc4)
add_subdirectory(snow_3g)
add_subdirectory(zuc)
add_subdirectory(sm4)
add_subdirectory(des)

# asymmetric_key