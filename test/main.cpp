#include <gtest/gtest.h>

#include "test_base64.h"
#include "test_crc.h"
#include "test_des.h"
#include "test_md5.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}