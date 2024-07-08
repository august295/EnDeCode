#ifndef __ZUC_EEA3_H__
#define __ZUC_EEA3_H__

#include "zuc.h"

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 祖冲之算法 128-EEA3 模式
 * @param[in]     CK             初始密钥
 * @param[in]     COUNT          计数器
 * @param[in]     BEARER         携带者身份
 * @param[in]     DIRECTION      传输方向
 * @param[in]     LENGTH         输入消息位数
 * @param[in]     M              消息
 * @param[out]    C              加密消息
 */
ENDECODE_API void zuc_EEA3(uint8_t* CK, uint32_t COUNT, uint32_t BEARER, uint32_t DIRECTION, uint32_t LENGTH, uint32_t* M, uint32_t* C);

#ifdef __cplusplus
}
#endif

#endif