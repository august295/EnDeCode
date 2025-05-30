#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include "endecode/base64/base64.h"

TEST(test_base64, str_ascii)
{
    char* plaintext     = (char*)"123456";
    int   plaintext_len = strlen(plaintext);

    char* ciphertext     = (char*)malloc(BASE64_ENCODE_OUT_SIZE(plaintext_len));
    int   ciphertext_len = 0;
    ciphertext_len       = base64_encode((unsigned char*)plaintext, plaintext_len, ciphertext);

    char* result     = (char*)malloc(plaintext_len);
    int   result_len = 0;
    result_len       = base64_decode(ciphertext, ciphertext_len, (unsigned char*)result);

    EXPECT_EQ(std::string(plaintext, plaintext_len), std::string(result, result_len));
    EXPECT_EQ(plaintext_len, result_len);

    free(ciphertext);
    free(result);
}

TEST(test_base64, str_gbk)
{
    char* plaintext     = (char*)"中文";
    int   plaintext_len = strlen(plaintext);

    char* ciphertext     = (char*)malloc(BASE64_ENCODE_OUT_SIZE(plaintext_len));
    int   ciphertext_len = 0;
    ciphertext_len       = base64_encode((unsigned char*)plaintext, plaintext_len, ciphertext);

    char* result     = (char*)malloc(plaintext_len);
    int   result_len = 0;
    result_len       = base64_decode(ciphertext, ciphertext_len, (unsigned char*)result);

    EXPECT_EQ(std::string(plaintext, plaintext_len), std::string(result, result_len));
    EXPECT_EQ(plaintext_len, result_len);

    free(ciphertext);
    free(result);
}
