#include <gtest/gtest.h>

#include "endecode/sm2/sm2.h"

#include "test_util.h"

TEST(test_sm2, test1)
{
    std::string msg = "message digest";

    ecc_key_st st;
    sm2_generate_keypair(&st);
    mpz_t r, s;
    mpz_inits(r, s, NULL);

    sm2_sign((uint8_t*)msg.data(), msg.size(), (uint8_t*)"1234567812345678", 16, &st, r, s);
    auto ret = sm2_verify((uint8_t*)msg.data(), msg.size(), (uint8_t*)"1234567812345678", 16, &st, r, s);
    EXPECT_EQ(ret, 1);

    // 释放资源
    ecc_key_st_clear(&st);
    mpz_clears(r, s, NULL);
}

TEST(test_sm2, test2)
{
    // clang-format off
    // private key
    uint8_t d[] = {
        0x39, 0x45, 0x20, 0x8F, 0x7B, 0x21, 0x44, 0xB1, 0x3F, 0x36, 0xE3, 0x8A, 0xC6, 0xD3, 0x9F, 0x95,
        0x88, 0x93, 0x93, 0x69, 0x28, 0x60, 0xB5, 0x1A, 0x42, 0xFB, 0x81, 0xEF, 0x4D, 0xF7, 0xC5, 0xB8,
    };
    // public key
    uint8_t key[] = {
        0x09, 0xF9, 0xDF, 0x31, 0x1E, 0x54, 0x21, 0xA1, 0x50, 0xDD, 0x7D, 0x16, 0x1E, 0x4B, 0xC5, 0xC6,
        0x72, 0x17, 0x9F, 0xAD, 0x18, 0x33, 0xFC, 0x07, 0x6B, 0xB0, 0x8F, 0xF3, 0x56, 0xF3, 0x50, 0x20,
        0xCC, 0xEA, 0x49, 0x0C, 0xE2, 0x67, 0x75, 0xA5, 0x2D, 0xC6, 0xEA, 0x71, 0x8C, 0xC1, 0xAA, 0x60,
        0x0A, 0xED, 0x05, 0xFB, 0xF3, 0x5E, 0x08, 0x4A, 0x66, 0x32, 0xF6, 0x07, 0x2D, 0xA9, 0xAD, 0x13,
    };
    // clang-format on
    std::string msg = "message digest";

    ecc_key_st st;
    sm2_init_set(&st.curve, &st.G);
    ecc_point_init(&st.public_key);
    mpz_init(st.private_key);
    mpz_import(st.public_key.x, 32, 1, 1, 1, 0, key);
    mpz_import(st.public_key.y, 32, 1, 1, 1, 0, key + 32);
    mpz_import(st.private_key, 32, 1, 1, 1, 0, d);
    mpz_t r, s;
    mpz_inits(r, s, NULL);
    mpz_set_str(r, "F5A03B0648D2C4630EEAC513E1BB81A15944DA3827D5B74143AC7EACEEE720B3", 16);
    mpz_set_str(s, "B1B6AA29DF212FD8763182BC0D421CA1BB9038FD1F7F42D4840B69C485BBC1AA", 16);

    auto ret = sm2_verify((uint8_t*)msg.data(), msg.size(), (uint8_t*)"1234567812345678", 16, &st, r, s);
    EXPECT_EQ(1, ret);

    // 释放资源
    ecc_key_st_clear(&st);
    mpz_clears(r, s, NULL);
}
