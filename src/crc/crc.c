#include <stdio.h>

#include "crc.h"

/**
 * @brief 8 bit反转，如1100 0101 反转后为1010 0011
 *
 * @param data      8 bit
 * @return uint8_t  反转后的 8 bit
 */
uint8_t reverse8(uint8_t data)
{
    uint8_t i;
    uint8_t temp = 0;
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
 * @param data      16 bit
 * @return uint16_t 反转后的 16 bit
 */
uint16_t reverse16(uint16_t data)
{
    uint8_t  i;
    uint16_t temp = 0;
    for (i = 0; i < 16; i++)
    {
        temp |= ((data >> i) & 0x0001) << (15 - i);
    }
    return temp;
}

/**
 * @brief 32 bit 反转
 *
 * @param data      32 bit
 * @return uint32_t 反转后的 16 bit
 */
uint32_t reverse32(uint32_t data)
{
    uint8_t  i;
    uint32_t temp = 0;
    for (i = 0; i < 32; i++)
    {
        temp |= ((data >> i) & 0x01) << (31 - i);
    }
    return temp;
}

/**
 * @brief CRC 校验，校验值为 8 位
 *
 * @param addr      数据首地址
 * @param num       数据长度（字节）
 * @param type      CRC8 的算法类型
 * @return uint8_t  8 位校验值
 */
uint8_t crc8(uint8_t* addr, int num, CRC_8 type)
{
    uint8_t data;
    uint8_t crc = type.InitValue; // 初始值
    int     i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == true)
        {
            data = reverse8(data);
        }
        // 与crc初始值异或
        crc = crc ^ data;
        for (i = 0; i < 8; i++)
        {
            // 左移移出的位为1，左移后与多项式异或
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
    // 满足条件，反转
    if (type.OutputReverse == true)
    {
        crc = reverse8(crc);
    }
    crc = crc ^ type.XorOut; // 最后返与结果异或值异或
    return (crc);            // 返回最终校验值
}

/**
 * @brief CRC 校验，校验值为 16 位
 *
 * @param addr      数据首地址
 * @param num       数据长度（字节）
 * @param type      CRC16 的算法类型
 * @return uint16_t 16 位校验值
 */
uint16_t crc16(uint8_t* addr, int num, CRC_16 type)
{
    uint8_t  data;
    uint16_t crc = type.InitValue; // 初始值
    int      i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == true)
        {
            data = reverse8(data);
        }
        // 与crc初始值高8位异或
        crc = crc ^ (data << 8);
        for (i = 0; i < 8; i++)
        {
            // 左移移出的位为1，左移后与多项式异或
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
    // 满足条件，反转
    if (type.OutputReverse == true)
    {
        crc = reverse16(crc);
    }
    crc = crc ^ type.XorOut; // 最后返与结果异或值异或
    return (crc);            // 返回最终校验值
}

/**
 * @brief CRC 校验，校验值为 32 位
 *
 * @param addr      数据首地址
 * @param num       数据长度（字节）
 * @param type      CRC32 的算法类型
 * @return uint32_t 32 位校验值
 */
uint32_t crc32(uint8_t* addr, int num, CRC_32 type)
{
    uint8_t  data;
    uint32_t crc = type.InitValue; // 初始值
    int      i;
    for (; num > 0; num--)
    {
        data = *addr++;
        if (type.InputReverse == true)
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
    // 满足条件，反转
    if (type.OutputReverse == true)
    {
        crc = reverse32(crc);
    }
    crc = crc ^ type.XorOut; // 最后返与结果异或值异或
    return (crc);            // 返回最终校验值
}
