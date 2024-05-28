#ifndef __ZUC_H__
#define __ZUC_H__

#include <stdint.h>

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief
 * @param[in] k        密钥
 * @param[in] iv       初始值
 */
ENDECODE_API  void Initialization(uint8_t* k, uint8_t* iv);

/**
 * @brief
 * @param[in] pKeystream
 * @param[in] KeystreamLen
 */
ENDECODE_API void GenerateKeystream(uint32_t* pKeystream, int KeystreamLen);

/**
 * @brief 
 * @param[in]     k              
 * @param[in]     iv             
 * @param[in]     ks             
 * @param[in]     len            
 */
ENDECODE_API void ZUC(uint8_t* k, uint8_t* iv, uint32_t* ks, int len);

#ifdef __cplusplus
}
#endif

#endif