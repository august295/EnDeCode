#ifndef __ZUC_H__
#define __ZUC_H__

#include <stdint.h>

void Initialization(uint8_t* k, uint8_t* iv);

void GenerateKeystream(uint32_t* pKeystream, int KeystreamLen);

#endif