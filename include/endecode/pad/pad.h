#ifndef __PAD_H__
#define __PAD_H__

#include "endecode/pad/pkcs.h"
#include "endecode/pad/zero.h"

typedef int (*_func_pad)(uint8_t* padded, size_t block_size, const uint8_t* message, size_t message_len);
typedef int (*_func_unpad)(uint8_t* message, size_t* message_len, const uint8_t* padded, size_t block_size);

#endif
