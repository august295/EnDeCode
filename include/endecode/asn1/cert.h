#ifndef __CERT_H__
#define __CERT_H__

#include "endecode/asn1/asn1.h"

#define PEM_CERT_BEGIN "-----BEGIN CERTIFICATE-----"
#define PEM_CERT_END   "-----END CERTIFICATE-----"

void convertUTCTimeToStandard(const char* utcTime, char* standardTime);

void convertGeneralizedTimeToStandard(const char* generalizedTime, char* standardTime);

void hex_to_ascii(const uint8_t* hex, int hex_len, int upper, uint8_t* ascii);

#endif