set(ProjectName "test")
CreateTarget(${ProjectName} "Exe" "test")
target_link_libraries(${ProjectName} gtest gtest_main)

# unknown
target_link_libraries(${ProjectName} base64)
target_link_libraries(${ProjectName} crc)

# digest
target_link_libraries(${ProjectName} md5)

# symmetric_key 
target_link_libraries(${ProjectName} snow_3g)
target_link_libraries(${ProjectName} des)