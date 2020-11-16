/*-------------------------------------------------------
 Data Encryption Standard  56位密钥加密64位数据
 --------------------------------------------------------*/

#include "../src/des.h"
#include "../src/table.h"

int main() {
    int  i            = 0;
    int  j            = 0;
    char MesHex[16]   = {0}; // 16个字符数组用于存放 64位16进制的密文
    char MyKey[8]     = {0}; // 初始密钥 8字节*8
    char YourKey[8]   = {0}; // 输入的解密密钥 8字节*8
    char MyMessage[8] = {0}; // 初始明文
    bool MesBit[64]   = {0}; // 明文二进制存储空间 64位

    printf("Please input your Message(<=64 bit):\n");
    gets(MyMessage); // 明文
    printf("\n");
    // 计算明文长度
    while (MyMessage[j] != '\0') {
        j++;
    }

    // 大于8提示错误
    while (j > 8) {
        printf("Please input a correct  Message!\n");
        gets(MyMessage);
        printf("\n");
        j = 0;
        // 再次检测
        while (MyMessage[j] != '\0') {
            j++;
        }
    }

    printf("Please input your Secret Key（64bit）:\n");
    gets(MyKey); // 密钥
    printf("\n");

    // 计算密钥长度
    while (MyKey[i] != '\0') {
        i++;
    }
    // 不是8 提示错误
    while (i != 8) {
        printf("Please input a correct Secret Key!\n");
        gets(MyKey);
        printf("\n");
        i = 0;
        // 再次检测
        while (MyKey[i] != '\0') {
            i++;
        }
    }

    SetKey(MyKey);          // 设置密钥 得到子密钥Ki
    Des(MesHex, MyMessage); // 执行DES加密

    printf("Your Message is Encrypted!:\n"); // 信息已加密
    for (i = 0; i < 16; i++) {
        printf("%c ", MesHex[i]);
    }
    printf("\n\n");

    printf("Please input your Secret Key to Deciphering:\n"); // 请输入密钥以解密
    gets(YourKey);                                            // 得到密钥
    SetKey(YourKey);                                          // 设置密钥
    deDes(MyMessage, MesHex);                                 // 解密输出到MyMessage

    printf("Deciphering Over !!:\n"); // 解密结束
    for (i = 0; i < 8; i++) {
        printf("%c ", MyMessage[i]);
    }
}
