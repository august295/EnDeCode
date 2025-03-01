#include <gtest/gtest.h>

#include "test_a5_1.h"
#include "test_aes.h"
#include "test_des.h"
#include "test_rc4.h"
#include "test_sm4.h"
#include "test_snow_3g.h"
#include "test_zuc.h"

#include "test_md5.h"
#include "test_sha1.h"
#include "test_sha2.h"
#include "test_sha3.h"
#include "test_sm3.h"

#ifdef ENABLE_GMP
    #include "test_ecc.h"
    #include "test_rsa.h"
#endif

#include "test_base64.h"
#include "test_crc.h"

#include "test_asn1.h"

void printf_oid()
{
    for (size_t i = 0; i < sizeof(sm_oid_mapping_x500) / sizeof(SM_OID_MAPPING); i++)
    {
        uint8_t oid_bin[128];
        size_t  oid_bin_len = 0;
        string_to_oid(sm_oid_mapping_x500[i].oid_string, oid_bin, &oid_bin_len);
        char temp[128] = {0};
        replace_dot(sm_oid_mapping_x500[i].oid_string, temp);
        printf("static uint8_t OID_%s[] = {", temp);
        for (size_t j = 0; j < oid_bin_len; j++)
        {
            if (j == oid_bin_len - 1)
            {
                printf("0x%02X", oid_bin[j]);
            }
            else
            {
                printf("0x%02X, ", oid_bin[j]);
            }
        }
        printf("};\n");
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}