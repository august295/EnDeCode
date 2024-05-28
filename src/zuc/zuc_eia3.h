#ifndef __ZUC_EIA3_H__
#define __ZUC_EIA3_H__

#include "zuc.h"

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 
 * @param[in]     IK             
 * @param[in]     COUNT          
 * @param[in]     DIRECTION      
 * @param[in]     BEARER         
 * @param[in]     LENGTH         
 * @param[in]     M              
 * @param[in]     MAC            
 */
ENDECODE_API void zuc_EIA3(uint8_t* IK, uint32_t COUNT, uint32_t DIRECTION, uint32_t BEARER, uint32_t LENGTH, uint32_t* M, uint32_t* MAC);

#ifdef __cplusplus
}
#endif

#endif