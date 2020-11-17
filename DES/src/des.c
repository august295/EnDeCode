#include "des.h"
#include "table.h"

/*****************************************位操作**************************************************/
// 把 DataIn 开始的长度位 Len 位的二进制复制到 DatOut
void BitsCopy(bool *DatOut, bool *DataIn, int Len) {
    for (int i = 0; i < Len; i++) {
        DatOut[i] = DataIn[i];
    }
}
// 字节转换成位函数: 8byte 转成 64bit
void ByteToBit(bool *DatOut, unsigned char *DataIn, int Num) {
    for (int i = 0; i < Num; i++) {
        DatOut[i] = (DataIn[i / 8] >> (i % 8)) & 0x01;
    }
}
// 位转换成字节函数: 64bit 转成 8byte
void BitToByte(unsigned char *DatOut, bool *DataIn, int Num) {
    for (int i = 0; i < (Num / 8); i++) {
        DatOut[i] = 0;
    }
    for (int i = 0; i < Num; i++) {
        DatOut[i / 8] |= DataIn[i] << (8 - (i % 8));
    }
}
// 二进制密文转换为十六进制密文
void BitToHex(unsigned char *DatOut, bool *DataIn, int Num) {
    for (int i = 0; i < Num / 4; i++) {
        DatOut[i] = 0;
    }
    for (int i = 0; i < Num / 4; i++) {
        DatOut[i] = (DataIn[i * 4] << 3) + (DataIn[i * 4 + 1] << 2) + (DataIn[i * 4 + 2] << 1) + (DataIn[i * 4 + 3]); //二进制转十六进制
        if ((DatOut[i] % 16) > 9) {
            DatOut[i] = DatOut[i] % 16 + '7'; //  余数大于9 10-15转换为A-F ascii表中9与A中间相差7个字符
        } else {
            DatOut[i] = DatOut[i] % 16 + '0'; //  余数小于9，直接输出字符
        }
    }
}
// 十六进制字符转二进制
void HexToBit(bool *DatOut, unsigned char *DataIn, int Num) {
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
/*****************************************位操作**************************************************/

/***********************************设置密钥 获取子密钥 Ki*****************************************/
void SetKey(unsigned char KeyIn[8]) {
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
void LoopMove(bool *DataIn, int Len, int Num) {
    static bool Temp[64] = {0};                // 缓存备份
    BitsCopy(Temp, DataIn, Num);               // 将数据最左边的Num位(被移出去的)存入Temp
    BitsCopy(DataIn, DataIn + Num, Len - Num); // 将数据左边开始的第Num移入原来的空间
    BitsCopy(DataIn + Len - Num, Temp, Num);   // 将缓存中移出去的数据加到最右边
}
/***********************************设置密钥 获取子密钥 Ki*****************************************/

/*******************************************F 函数************************************************/
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
    for (int i = 0, X = 0, Y = 0; i < 8; i++, DataIn += 6, DatOut += 4) {
        X = (DataIn[0] << 1) + DataIn[5];                                       // 05   代表第几行
        Y = (DataIn[1] << 3) + (DataIn[2] << 2) + (DataIn[3] << 1) + DataIn[4]; // 1234 代表第几列
        ByteToBit(DatOut, &S_Box[i][X][Y], 4);                                  // 把找到的点数据换为二进制
    }
}
void P_change(bool DataOut[32], bool DataIn[32]) {
    TablePermute(DataIn, DataIn, P_Table, 32);
}
/*******************************************F 函数************************************************/

// 表置换函数
void TablePermute(bool *DatOut, bool *DataIn, const unsigned char *Table, int Num) {
    // Num为置换的长度
    for (int i = 0; i < Num; i++) {
        DatOut[i] = DataIn[Table[i] - 1]; // 将数据按对应的表上的位置排列
    }
}
// 按位异或
void Xor(bool *DataA, bool *DataB, int Num) {
    for (int i = 0; i < Num; i++) {
        DataA[i] = DataA[i] ^ DataB[i]; // 异或
    }
}

// IP置换
void IP_change(bool DataOut[64], bool DataIn[64]) {
    TablePermute(DataOut, DataIn, IP_Table, 64);
}
// 反IP置换
void IIP_change(bool DataOut[64], bool DataIn[64]) {
    TablePermute(DataOut, DataIn, IIP_Table, 64);
}
// 执行DES加密, unsigned char 输入, bit 输出
void Des(unsigned char MesOut[8], unsigned char MesIn[8]) {
    static bool  MesBit[64] = {0};         // 明文二进制存储空间 64位
    static bool  Temp[32]   = {0};         // 临时存储
    static bool *MiL        = &MesBit[0];  // 前32位
    static bool *MiR        = &MesBit[32]; //  后32位

    ByteToBit(MesBit, MesIn, 64); // 把明文换成二进制存入MesBit
    IP_change(MesBit, MesBit);    // IP置换
    for (int i = 0; i < 16; i++) {
        BitsCopy(Temp, MiR, 32);  // 临时存储
        F_Change(MiR, SubKey[i]); // F函数变换
        Xor(MiR, MiL, 32);        // 得到Ri
        BitsCopy(MiL, Temp, 32);  // 得到Li
    }
    IIP_change(MesBit, MesBit); // IP反置换
    BitToByte(MesOut, MesBit, 64);
}

// 执行DES解密, unsigned char 输入, unsigned char 输出
void deDes(unsigned char MesOut[8], unsigned char MesIn[8]) {
    static bool  MesBit[64] = {0};
    static bool  Temp[32]   = {0};
    static bool *MiL        = &MesBit[0];
    static bool *MiR        = &MesBit[32];

    ByteToBit(MesBit, MesIn, 64);
    IP_change(MesBit, MesBit);
    TablePermute(MesBit, MesBit, IP_Table, 64);
    for (int i = 15; i >= 0; i--) {
        BitsCopy(Temp, MiL, 32);
        F_Change(MiL, SubKey[i]);
        Xor(MiL, MiR, 32);
        BitsCopy(MiR, Temp, 32);
    }
    IIP_change(MesBit, MesBit);
    BitToByte(MesOut, MesBit, 64);
}