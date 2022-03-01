#include <stdio.h>

#include "crc.h"
#include "type.h"

/**
 * @brief 8 bit反转，如1100 0101 反转后为1010 0011
 *
 * @param data     8 bit
 * @return u8      反转后的 8 bit
 */
u8 reverse8(u8 data)
{
    u8 i;
    u8 temp = 0;
    // 字节反转
    for (i = 0; i < 8; i++)
    {
        temp |= ((data >> i) & 0x01) << (7 - i);
    }
    return temp;
}

/**
 * @brief 16 bit反转，如1100 0101 1110 0101反转后为1010 0111 1010 0011
 *
 * @param data     16 bit
 * @return u16     反转后的 16 bit
 */
u16 reverse16(u16 data)
{
    u8  i;
    u16 temp = 0;
    for (i = 0; i < 16; i++)
    {
        temp |= ((data >> i) & 0x0001) << (15 - i);
    }
    return temp;
}

/**
 * @brief 32 bit 反转
 *
 * @param data     32 bit
 * @return u32     反转后的 16 bit
 */
u32 reverse32(u32 data)
{
    u8  i;
    u32 temp = 0;
    for (i = 0; i < 32; i++)
    {
        temp |= ((data >> i) & 0x01) << (31 - i);
    }
    return temp;
}

/**
 * @brief CRC 校验，校验值为 8 位
 *
 * @param addr     数据首地址
 * @param num      数据长度（字节）
 * @param type     CRC8 的算法类型
 * @return u8      8 位校验值
 */
u8 crc8(u8* addr, int num, CRC_8 type)
{
    u8  data;
    u8  crc = type.InitValue; //初始值
    int i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == TRUE)
        {
            data = reverse8(data);
        }
        //与crc初始值异或
        crc = crc ^ data;
        for (i = 0; i < 8; i++)
        {
            //左移移出的位为1，左移后与多项式异或
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ type.Poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    //满足条件，反转
    if (type.OutputReverse == TRUE)
    {
        crc = reverse8(crc);
    }
    crc = crc ^ type.XorOut; //最后返与结果异或值异或
    return (crc);            //返回最终校验值
}

/**
 * @brief CRC 校验，校验值为 16 位
 *
 * @param addr     数据首地址
 * @param num      数据长度（字节）
 * @param type     CRC16 的算法类型
 * @return u16     16 位校验值
 */
u16 crc16(u8* addr, int num, CRC_16 type)
{
    u8  data;
    u16 crc = type.InitValue; //初始值
    int i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == TRUE)
        {
            data = reverse8(data);
        }
        //与crc初始值高8位异或
        crc = crc ^ (data << 8);
        for (i = 0; i < 8; i++)
        {
            //左移移出的位为1，左移后与多项式异或
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ type.Poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    //满足条件，反转
    if (type.OutputReverse == TRUE)
    {
        crc = reverse16(crc);
    }
    crc = crc ^ type.XorOut; //最后返与结果异或值异或
    return (crc);            //返回最终校验值
}

/**
 * @brief CRC 校验，校验值为 32 位
 *
 * @param addr     数据首地址
 * @param num      数据长度（字节）
 * @param type     CRC32 的算法类型
 * @return u32     32 位校验值
 */
u32 crc32(u8* addr, int num, CRC_32 type)
{
    u8  data;
    u32 crc = type.InitValue; //初始值
    int i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == TRUE)
        {
            data = reverse8(data);
        }
        // 与crc初始值高8位异或
        crc = crc ^ (data << 24);
        for (i = 0; i < 8; i++)
        {
            // 左移移出的位为1，左移后与多项式异或
            if (crc & 0x80000000)
            {
                crc = (crc << 1) ^ type.Poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    //满足条件，反转
    if (type.OutputReverse == TRUE)
    {
        crc = reverse32(crc);
    }
    crc = crc ^ type.XorOut; //最后返与结果异或值异或
    return (crc);            //返回最终校验值
}
