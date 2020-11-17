#include <string.h>

#include "../src/des.h"
#include "../src/table.h"

void padding(unsigned char *str, int num) {
    const unsigned char *pad = "00000000";
    strncat(str, pad, 8 - num);
}

void displayChar(unsigned char *str, int num) {
    for (int i = 0; i < num; i++) {
        printf("%c", str[i]);
    }
    printf("\n");
}

void displayBool(bool *str, int num) {
    for (int i = 0; i < num; i++) {
        printf("%d", str[i]);
    }
    printf("\n");
}

int main() {
    unsigned char  PlainText[8]  = {0}; // 明文 8 byte. 8*8=64
    unsigned char  Key[8]        = {0}; // 密钥 8 byte,
    unsigned char  CipherText[8] = {0}; // 密文 8 byte,
    unsigned char  Hex[16]       = {0}; // 16 进制 64 位，4*16 =64
    bool           Bit[64]       = {0}; // 2  进制 64 位
    unsigned char *msg           = (unsigned char *)calloc(64, sizeof(unsigned char));

    printf("Please Input Message:\n");
    scanf("%[^\n]", msg);
    int len = strlen(msg);
    while (len > 0) {
        if (len > 8) {
            strncpy(PlainText, msg, 8);
            msg = msg + 8;
            len = len - 8;
        } else {
            padding(msg, len);
            strncpy(PlainText, msg, 8);
            break;
        }
    }

    memset(msg, '\0', sizeof(unsigned char) * 64);
    printf("Please Input Key(8 byte):\n");
    scanf("\n%[^\n]", msg);
    while (strlen(msg) != 8) {
        printf("key is not 8 byte, please again...\n");
        scanf("\n%[^\n]", msg);
    }
    strncpy(Key, msg, 8);

    SetKey(Key);
    displayChar(PlainText, 8);
    Des(CipherText, PlainText);
    displayChar(CipherText, 8);

    ByteToBit(Bit, CipherText, 64);
    displayBool(Bit, 64);

    BitToHex(Hex, Bit, 64);
    displayChar(Hex, 16);

    deDes(PlainText, CipherText);
    displayChar(PlainText, 8);
}
