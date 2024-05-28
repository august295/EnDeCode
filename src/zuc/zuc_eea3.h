#ifndef __ZUC_EEA3_H__
#define __ZUC_EEA3_H__

#include "zuc.h"

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 
 * @param[in] CK       
 * @param[in] COUNT    
 * @param[in] BEARER   
 * @param[in] DIRECTION
 * @param[in] LENGTH   
 * @param[in] M        
 * @param[in] C        
 */
ENDECODE_API void zuc_EEA3(uint8_t* CK, uint32_t COUNT, uint32_t BEARER, uint32_t DIRECTION, uint32_t LENGTH, uint32_t* M, uint32_t* C);

#ifdef __cplusplus
}
#endif

#endif