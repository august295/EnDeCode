#include "../src/md5.h"

int main(int argc, char* argv[]) {
    char*    msg;
    uint64_t len;
    uint8_t  result[16];

    if (argc < 2) {
        printf("请输入加密字符串：");
        scanf("%[^\n]", msg);
    } else if (argc == 2) {
        msg = argv[1];
    } else {
        printf("please input ('md5.exe' and next 'str') or (md5.exe 'str')");
        exit(0);
    }

    len = strlen(msg);
    md5((uint8_t*)msg, len, result);

    /**
     * type1: 直接打印
     * type2: 转换成字符串打印 
     */
    // for (int i = 0; i < 16; i++) {
    //     printf("%.2X", result[i]);
    // }
    // printf("\n");

    // md5，32个字符
    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * 32);
    // 16进制，16组，32个字符
    HexToAscii(result, str, 16);
    printf("%s\n", str);

    return 0;
}