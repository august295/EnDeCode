#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <stdint.h>

bool my_equal_array_8bit(uint8_t* data, uint8_t* temp, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    return true;
}

bool my_equal_array_32bit(uint32_t* data, uint32_t* temp, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    return true;
}

bool my_equal_array_32bit_1bit(uint32_t* data, uint32_t* temp, uint32_t len_bit)
{
    int i   = 0;
    int    len = len_bit / 32;
    for (i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    if (len_bit % 32)
    {
        i++;
        int shift = 32 - (len_bit % 32);
        if ((data[i] >> shift) != (data[i] >> shift))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 通用转换模板函数：将任意类型整数转换为字节数组
 * @tparam T                     输入类型
 * @param[in]     input          nbit 输入
 * @param[out]    output         8bit 输出
 * @param[in]     bit8_num       8bit 输出长度
 * @param[in]     little         1-小端，0-大端
 */
template<typename T>
void bitn_to_bit8(T input, uint8_t* output, int bit8_num, int little)
{
    for (int i = 0; i < bit8_num; i++)
    {
        output[i] = little ? (uint8_t)((input >> (8 * i)) & 0xFF) : (uint8_t)((input >> (8 * (bit8_num - 1 - i))) & 0xFF);
    }
}

/**
 * @brief 通用转换模板函数：将字节数组转换为任意类型整数
 * @tparam T                     输出类型
 * @param[in]     input          8bit 输入
 * @param[in]     bit8_num       8bit 输入长度
 * @param[in]     little         1-小端，0-大端
 * @return T                     Tbit 输出
 */
template<typename T>
T bit8_to_bitn(const uint8_t* input, int bit8_num, int little)
{
    T result = 0;
    for (int i = 0; i < bit8_num; i++)
    {
        result |= little ? ((T)input[i] << (8 * i)) : ((T)input[i] << (8 * (bit8_num - 1 - i)));
    }
    return result;
}

/**
 * @brief 定义转换函数示例：将任意位数组转换为字节数组
 * @tparam InType                输入类型
 * @param[in]     input          nbit 输入
 * @param[in]     input_num      nbit 输入长度
 * @param[out]    output         8bit 输出
 * @param[in]     little         1-小端，0-大端
 */
template<typename InType>
void array_bitn_to_bit8(const InType* input, int input_num, uint8_t* output, int little)
{
    int bit8_num = sizeof(InType);
    for (int i = 0; i < input_num; ++i)
    {
        bitn_to_bit8<InType>(input[i], output + i * bit8_num, bit8_num, little);
    }
}

/**
 * @brief 定义转换函数示例：将字节数组转换为任意位数组
 * @tparam OutType               输出类型
 * @param[in]     input          8bit 输入
 * @param[out]    output         nbit 输出
 * @param[in]     output_num     nbit 输出长度
 * @param[in]     little         1-小端，0-大端
 */
template<typename OutType>
void array_bit8_to_bitn(const uint8_t* input, OutType* output, int output_num, int little)
{
    int bit8_num = sizeof(OutType);
    for (int i = 0; i < output_num; ++i)
    {
        output[i] = bit8_to_bitn<OutType>(input + i * bit8_num, bit8_num, little);
    }
}

/**
 * @brief 交换 16 位整数的字节顺序
 * @param[in]     value          64bit 整数
 * @return uint16_t              交换后整数
 */
uint16_t swap16(uint16_t value)
{
    return ((value & 0x00FFU) << 8) | ((value & 0xFF00U) >> 8);
}

/**
 * @brief 交换 32 位整数的字节顺序，复用 swap16
 * @param[in]     value          64bit 整数
 * @return uint32_t              交换后整数
 */
uint32_t swap32(uint32_t value)
{
    return ((uint32_t)swap16((uint16_t)(value & 0xFFFFU)) << 16) | (uint32_t)swap16((uint16_t)(value >> 16));
}

/**
 * @brief 交换 64 位整数的字节顺序，复用 swap32
 * @param[in]     value          64bit 整数
 * @return uint64_t              交换后整数
 */
uint64_t swap64(uint64_t value)
{
    return ((uint64_t)swap32((uint32_t)(value & 0xFFFFFFFFULL)) << 32) | (uint64_t)swap32((uint32_t)(value >> 32));
}

/**
 * @brief 16进制转字符串
 * @param[in]     hex            16进制
 * @param[in]     hex_len        16进制长度
 * @param[in]     upper          1-大写，0-小写
 * @param[out]    ascii          字符串
 */
void hex_to_ascii(const uint8_t* hex, int hex_len, int upper, uint8_t* ascii)
{
    uint8_t temp[2];
    for (int i = 0; i < hex_len; i++)
    {
        temp[0] = (hex[i] & 0xF0) >> 4;
        temp[1] = (hex[i] & 0x0F);
        for (int j = 0; j < 2; j++)
        {
            if (temp[j] < 10)
            {
                temp[j] += '0';
            }
            else
            {
                temp[j] = upper ? temp[j] - 10 + 'A' : temp[j] - 10 + 'a';
            }
            *ascii++ = temp[j];
        }
    }
    *ascii++ = '\0';
}

#endif