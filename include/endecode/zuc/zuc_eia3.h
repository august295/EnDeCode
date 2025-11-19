#ifndef __ZUC_EIA3_H__
#define __ZUC_EIA3_H__

#include "zuc.h"

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 祖冲之算法 128-EIA3 模式
 * @param[in]     IK             初始密钥
 * @param[in]     COUNT          计数器
 * @param[in]     DIRECTION      传输方向
 * @param[in]     BEARER         携带者身份
 * @param[in]     LENGTH         输入消息位数
 * @param[in]     M              消息
 * @param[out]    MAC            消息鉴别码
 */
ENDECODE_API void zuc_EIA3(uint8_t* IK, uint32_t COUNT, uint32_t DIRECTION, uint32_t BEARER, uint32_t LENGTH, uint32_t* M, uint32_t* MAC);

#ifdef __cplusplus
}
#endif

#endif