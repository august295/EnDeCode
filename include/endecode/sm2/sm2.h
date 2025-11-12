#ifndef __SM2_H__
#define __SM2_H__

#include <stdint.h>

#include <gmp.h>

#include "endecode/sm2/sm2_define.hpp"

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

ENDECODE_API void sm2_init_set(EllipticCurve* curve, ECPoint* G);

ENDECODE_API void sm2_generate_keypair(ecc_key_st* st);

ENDECODE_API void sm2_key_st_clear(ecc_key_st* st);

ENDECODE_API void sm2_sign(
    const uint8_t* msg,
    size_t         msg_len,
    const uint8_t* id,
    size_t         id_len,
    ecc_key_st*    st,
    mpz_t          r,
    mpz_t          s);

ENDECODE_API int sm2_verify(
    const uint8_t* msg,
    size_t         msg_len,
    const uint8_t* id,
    size_t         id_len,
    ecc_key_st*    st,
    mpz_t          r,
    mpz_t          s);

#ifdef __cplusplus
}
#endif

#endif