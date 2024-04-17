//
// Implementation of DES coded by:
//     - David Wong, moi@davidwong.fr
//     - Jacques Monin, jacques.monin@u-bordeaux.fr
//     - Hugo Bonnin, hugo.bonnin@u-bordeaux.fr
//

#ifndef DES_H
#define DES_H

#define FIRSTBIT 0x8000000000000000

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 将 from 中的第 position_from 位添加到 block 中的第 position_to 位
 */
void addbit(uint64_t* block, uint64_t from, int position_from, int position_to);

/**
 * @brief 进行初始化和逆初始化
 * @param data         原始数据
 * @param initial      true-初始化，false-逆初始化
 */
void Permutation(uint64_t* data, bool initial);

/**
 * @brief 验证奇偶校验位是否正常
 */
bool key_parity_verify(uint64_t key);

// Key Schedule ( http://en.wikipedia.org/wiki/File:DES-key-schedule.png )
/**
 * @brief 密钥调度模块
 * @param key          [out] 密钥
 * @param next_key     是左键+右键的组合，将在下一轮的 key_chedule 中使用
 * @param round        循环次数
 */
void key_schedule(uint64_t* key, uint64_t* next_key, int round);

/**
 * @brief DES算法的加密和解密过程，首先对输入的数据进行扩展和异或操作，然后进行子置换、异或和行移位操作，最后进行位旋转和异或操作，得到新的数据。这个过程在DES算法中重复进行16次，以确保解密和加密的正确性。
 * @param data         数据
 * @param key          密钥
 */
void rounds(uint64_t* data, uint64_t key);

#include "des_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief 生成密钥
 * @param key          [out] 密钥
 */
DES_API void genkey(uint64_t* key);

/**
 * @brief 以 ECB(电子密码本) 模式和 0 填充方式进行加密和解密
 * @param input        明文
 * @param input_len    明文长度
 * @param encrypt      true-加密，false-解密
 * @param key          密钥
 * @param output       密文
 */
DES_API int des_ecb_zero(char* input, int input_len, bool encrypt, uint64_t key, char** output);

#ifdef __cplusplus
}
#endif

#endif
