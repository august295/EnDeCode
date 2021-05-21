#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base64.h"

int main(int argc, char *argv[]) {
    char           str[256] = { 0 };
    char           encrypt  = 1;
    char *         encode_out;
    unsigned char *decode_out;
    int            out_len;

    int optc = 0;
    while ((optc = getopt(argc, argv, "eds:")) != -1) {
        switch (optc) {
        case 'e':
            encrypt = 1;
            break;
        case 'd':
            encrypt = 0;
            break;
        case 's':
            strcpy(str, optarg);
            break;
        default:
            printf("input error!!!");
            exit(1);
        }
    }

    if (encrypt) {
        encode_out = malloc(BASE64_ENCODE_OUT_SIZE(strlen(str)));
        out_len    = base64_encode(str, strlen(str), encode_out);
        printf("%s\n", encode_out);
    } else {
        decode_out = malloc(BASE64_DECODE_OUT_SIZE(strlen(str)));
        out_len    = base64_decode(str, strlen(str), decode_out);
        printf("%s\n", decode_out);
    }

    return 0;
}