#include "des.h"
#include "table.h"

// 把 DataIn 开始的长度位 Len 位的二进制复制到 DatOut
void BitsCopy(bool *DatOut, bool *DataIn, int Len) {
    for (int i = 0; i < Len; i++) {
        DatOut[i] = DataIn[i];
    }
}
/**
 * 字节转换成位函数: 8byte 转成 64bit
 *  - 每8次换一个字节 每次向右移一位
 *  - 和1与取最后一位 共64位
 */
void ByteToBit(bool *DatOut, char *DataIn, int Num) {
    int i = 0;
    for (i = 0; i < Num; i++) {
        DatOut[i] = (DataIn[i / 8] >> (i % 8)) & 0x01;
    }
}
/**
 * 6位转换成字节函数: 4bit 转成 8byte
 *  - 字节数组每8次移一位
 *  - 位每次向左移 与上一次或
*/
void BitToByte(char *DatOut, bool *DataIn, int Num) {
    for (int i = 0; i < (Num / 8); i++) {
        DatOut[i] = 0;
    }
    for (int i = 0; i < Num; i++) {
        DatOut[i / 8] |= DataIn[i] << (i % 8);
    }
}
// 二进制密文转换为十六进制密文
void BitToHex(char *DatOut, bool *DataIn, int Num) {
    for (int i = 0; i < Num / 4; i++) {
        DatOut[i] = 0;
    }
    for (int i = 0; i < Num / 4; i++) {
        DatOut[i] = DataIn[i * 4] + (DataIn[i * 4 + 1] << 1) + (DataIn[i * 4 + 2] << 2) + (DataIn[i * 4 + 3] << 3); //二进制转十六进制
        if ((DatOut[i] % 16) > 9) {
            DatOut[i] = DatOut[i] % 16 + '7'; //  余数大于9 10-15转换为A-F ascii表中9与A中间相差7个字符
        } else {
            DatOut[i] = DatOut[i] % 16 + '0'; //  余数小于9，直接输出字符
        }
    }
}
// 十六进制字符转二进制
void HexToBit(bool *DatOut, char *DataIn, int Num) {
    // 16进制字符输入
    for (int i = 0; i < Num; i++) {
        //  大于9
        if ((DataIn[i / 4]) > '9') {
            DatOut[i] = ((DataIn[i / 4] - '7') >> (i % 4)) & 0x01;
        } else {
            DatOut[i] = ((DataIn[i / 4] - '0') >> (i % 4)) & 0x01;
        }
    }
}
// 表置换函数
void TablePermute(bool *DatOut, bool *DataIn, const char *Table, int Num) {
    static bool Temp[256] = {0};
    // Num为置换的长度
    for (int i = 0; i < Num; i++) {
        Temp[i] = DataIn[Table[i] - 1]; // 将数据按对应的表上的位置排列
    }
    BitsCopy(DatOut, Temp, Num); // 把缓存Temp的值输出
}
// 按位异或
void Xor(bool *DatA, bool *DatB, int Num) {
    for (int i = 0; i < Num; i++) {
        DatA[i] = DatA[i] ^ DatB[i]; // 异或
    }
}

/***********************************设置密钥 获取子密钥 Ki*****************************************/
/**
 *  1. PC1_Table 置换
 *  2. 循环左移
 *  3. PC2_Table 置换
 * Ki 存入全局变量 SubKey[16][48]
 */
void SetKey(char KeyIn[8]) {
    static bool  KeyBit[64] = {0};         // 密钥二进制存储空间
    static bool *KiL        = &KeyBit[0];  // 前28
    static bool *KiR        = &KeyBit[28]; // 后28共56，这只是获取空间地址大小，省略再额外分配空间

    // PC1: 把密钥转为二进制存入KeyBit, PC1_Table表置换 56 次
    ByteToBit(KeyBit, KeyIn, 64);
    TablePermute(KeyBit, KeyBit, PC1_Table, 56);
    // PC2
    for (int i = 0; i < 16; i++) {
        // 循环左移
        LoopMove(KiL, 28, Move_Table[i]);
        LoopMove(KiR, 28, Move_Table[i]);
        TablePermute(SubKey[i], KeyBit, PC2_Table, 48);
    }
}
/***************************************************************************                      
 * @brief : 子密钥的循环左移，BitsCopy第一个备份，第二个移位覆盖，第三个备份的取前面覆盖的填充到后面                                                                
 * @param :                                                                 
 *  DataIn : 数据首地址
 *  Len    : 数据长度
 *  Num    : 左移位数                                                              
 * @return: 
 ***************************************************************************/
void LoopMove(bool *DataIn, int Len, int Num) {
    static bool Temp[256] = {0};               // 缓存备份
    BitsCopy(Temp, DataIn, Num);               // 将数据最左边的Num位(被移出去的)存入Temp
    BitsCopy(DataIn, DataIn + Num, Len - Num); // 将数据左边开始的第Num移入原来的空间
    BitsCopy(DataIn + Len - Num, Temp, Num);   // 将缓存中移出去的数据加到最右边
}
/*************************************************************************************************/

/**********************************************F 函数*********************************************/
/**
 * 1. 根据输入的 R(32bit)，扩展
 * 2. 和子密钥异或
 * 3. S 盒置换
 * 4. P 盒置换
 */
void F_Change(bool DataIn[32], bool DatKi[48]) {
    static bool MiR[48] = {0}; // 输入 32 位通过E选位变为48位
    E_change(MiR, DataIn);     // E 扩展置换
    Xor(MiR, DatKi, 48);       // 和子密钥异或
    S_Change(DataIn, MiR);     // S 盒置换
    P_change(DataIn, DataIn);  // P 盒置换
}
void E_change(bool DataOut[48], bool DataIn[32]) {
    TablePermute(DataOut, DataIn, E_Table, 48);
}
void S_Change(bool DatOut[32], bool DataIn[48]) {
    int X = 0;
    int Y = 0;
    // i为8个S盒, 每执行一次,输入数据偏移6位, 每执行一次,输出数据偏移4位
    for (int i = 0, Y = 0, X = 0; i < 8; i++, DataIn += 6, DatOut += 4) {
        Y = (DataIn[0] << 1) + DataIn[5];                                       // 05   代表第几行
        X = (DataIn[1] << 3) + (DataIn[2] << 2) + (DataIn[3] << 1) + DataIn[4]; // 1234 代表第几列
        ByteToBit(DatOut, &S_Box[i][Y][X], 4);                                  // 把找到的点数据换为二进制
    }
}
void P_change(bool DataOut[32], bool DataIn[32]) {
    TablePermute(DataIn, DataIn, P_Table, 32);
}
/*************************************************************************************************/

// 执行DES加密
// 字节输入 Bin运算 Hex输出
void Des(char MesOut[8], char MesIn[8]) {
    static bool  MesBit[64] = {0};         // 明文二进制存储空间 64位
    static bool  Temp[32]   = {0};         // 临时存储
    static bool *MiL        = &MesBit[0];  // 前32位
    static bool *MiR        = &MesBit[32]; //  后32位

    ByteToBit(MesBit, MesIn, 64);               // 把明文换成二进制存入MesBit
    TablePermute(MesBit, MesBit, IP_Table, 64); // IP置换
    for (int i = 0; i < 16; i++) {
        BitsCopy(Temp, MiR, 32);  // 临时存储
        F_Change(MiR, SubKey[i]); // F函数变换
        Xor(MiR, MiL, 32);        // 得到Ri
        BitsCopy(MiL, Temp, 32);  // 得到Li
    }
    TablePermute(MesBit, MesBit, IIP_Table, 64); // IP反置换
    BitToHex(MesOut, MesBit, 64);
}

// 执行DES解密
// Hex输入 Bin运算 字节输出
void deDes(char MesOut[8], char MesIn[8]) {
    static bool  MesBit[64] = {0};
    static bool  Temp[32]   = {0};
    static bool *MiL        = &MesBit[0];
    static bool *MiR        = &MesBit[32];

    HexToBit(MesBit, MesIn, 64);
    TablePermute(MesBit, MesBit, IP_Table, 64);
    for (int i = 15; i >= 0; i--) {
        BitsCopy(Temp, MiL, 32);
        F_Change(MiL, SubKey[i]);
        Xor(MiL, MiR, 32);
        BitsCopy(MiR, Temp, 32);
    }
    TablePermute(MesBit, MesBit, IIP_Table, 64);
    BitToByte(MesOut, MesBit, 64);
}