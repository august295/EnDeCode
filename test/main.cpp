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

#include "test_base64.h"
#include "test_crc.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}