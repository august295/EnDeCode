﻿set(ProjectName "test")
CreateTarget(${ProjectName} "Exe" "test")
target_link_libraries(${ProjectName} gtest gtest_main)

if(BUILD_SINGLE_LIBRARY)
    target_link_libraries(${ProjectName} endecode)
else()
    # unknown
    target_link_libraries(${ProjectName} base64)
    target_link_libraries(${ProjectName} crc)

    # digest
    target_link_libraries(${ProjectName} md5)
    target_link_libraries(${ProjectName} sha1)
    target_link_libraries(${ProjectName} sha2)
    target_link_libraries(${ProjectName} sha3)
    target_link_libraries(${ProjectName} sm3)

    # symmetric_key
    target_link_libraries(${ProjectName} a5_1)
    target_link_libraries(${ProjectName} rc4)
    target_link_libraries(${ProjectName} snow_3g)
    target_link_libraries(${ProjectName} zuc)
    target_link_libraries(${ProjectName} sm4)
    target_link_libraries(${ProjectName} des)
    target_link_libraries(${ProjectName} aes)

    # asymmetric_key
    target_link_libraries(${ProjectName} rsa)
    target_link_libraries(${ProjectName} ecc)

    # certification
    target_link_libraries(${ProjectName} asn1)
endif()