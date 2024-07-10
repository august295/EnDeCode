#include "endecode/asn1/x509.h"

const char* x509_tag_name(int tag)
{
    switch (tag)
    {
    case V_X509_ASN1_SEQUENCE: return "SEQUENCE";
    case V_X509_ASN1_SET: return "SET";
    }
    return asn1_tag_name(tag);
}

struct asn1_tree_st* x509_parse_cert(const uint8_t* data, size_t len, int level)
{
    return asn1_parse(data, len, level);
}
