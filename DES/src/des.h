#ifndef __DES_H__
#define __DES_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void BitsCopy(bool *DatOut, bool *DataIn, int Len);                                 // 数组复制
void ByteToBit(bool *DatOut, unsigned char *DataIn, int Num);                       // 字节到位
void BitToByte(unsigned char *DatOut, bool *DataIn, int Num);                       // 位到字节
void BitToHex(unsigned char *DatOut, bool *DataIn, int Num);                        // 二进制到十六进制
void HexToBit(bool *DatOut, unsigned char *DataIn, int Num);                        // 十六进制到二进制
void TablePermute(bool *DatOut, bool *DataIn, const unsigned char *Table, int Num); // 位表置换函数
void F_Change(bool DataIn[32], bool DatKi[48]);                                     // F函数
void S_Change(bool DatOut[32], bool DataIn[48]);                                    // S盒置换
void E_change(bool DataOut[48], bool DataIn[32]);                                   // E置换
void P_change(bool DataOut[32], bool DataIn[32]);                                   // P置换
void Xor(bool *DatA, bool *DatB, int Num);                                          // 异或函数
void LoopMove(bool *DataIn, int Len, int Num);                                      // 循环左移 Len长度 Num移动位数
void SetKey(unsigned char KeyIn[8]);                                                // 设置密钥
void Des(unsigned char MesOut[8], unsigned char MesIn[8]);                          // 执行DES加密
void deDes(unsigned char MesOut[8], unsigned char MesIn[8]);                        // 执行DES解密

#endif