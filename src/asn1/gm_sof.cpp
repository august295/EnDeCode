#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/gm_sof.h"

BSTR SOF_GetCertInfo(BSTR Base64Cert, SHORT Type)
{
    size_t   Base64CertLen = strlen(Base64Cert);
    size_t   BinCertLen    = BASE64_DECODE_OUT_SIZE(Base64CertLen);
    uint8_t* BinCert       = (uint8_t*)calloc(1, BinCertLen + 1);
    base64_decode(Base64Cert, Base64CertLen, BinCert);

    SM2Certificate* cert    = sm2_cert_parse(BinCert, BinCertLen);
    BSTR            info    = NULL;
    size_t          infoLen = 0;
    if (Type == SGD_CERT_VERSION)
    {
        infoLen = cert->tbsCertificate.version.length + 1;
        info    = (BSTR)calloc(1, infoLen);
        snprintf(info, infoLen, "%d", cert->tbsCertificate.version.value[0]);
    }
    else if (Type == SGD_CERT_SERIAL)
    {
        infoLen = cert->tbsCertificate.serialNumber.length * 2 + 1;
        info    = (BSTR)calloc(1, infoLen);
        for (size_t i = 0; i < cert->tbsCertificate.serialNumber.length; i++)
        {
            sprintf(info + i * 2, "%02X", cert->tbsCertificate.serialNumber.value[i]);
        }
    }
    else if (Type == SGD_CERT_SIGNATURE_ALGORITHM)
    {
        info = (BSTR)calloc(1, MAX_OID);
        oid_to_string(cert->tbsCertificate.signature.algorithm.value, cert->tbsCertificate.signature.algorithm.length, info);
        infoLen = strlen(info);
    }
    else if (Type == SGD_CERT_ISSUER)
    {
        Name        issuer = cert->tbsCertificate.issuer;
        std::string info_str;
        char        temp[MAX_OID] = {0};
        for (size_t i = 0; i < issuer.count; i++)
        {
            for (size_t j = 0; j < issuer.names[i].count; j++)
            {
                AttributeTypeAndValue rdn = issuer.names[i].rdn[j];
                oid_to_string(rdn.type.value, rdn.type.length, temp);
                auto typeName = GMSOF::instance().GetX500Name(temp);
                info_str += typeName;
                info_str += "=";
                info_str += std::string((char*)rdn.value.value, rdn.value.length);
                info_str += ";";
            }
        }
        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_VALID_TIME)
    {
        Validity    time          = cert->tbsCertificate.validity;
        char        notBefore[20] = {0};
        char        notAfter[20]  = {0};
        size_t      utcOffset     = 8;
        std::string info_str;
        if (time.notBefore.tag == EASY_ASN1_UTCTIME)
        {
            convertUTCTimeToStandard((char*)time.notBefore.value, utcOffset, notBefore);
        }
        else if (time.notBefore.tag == EASY_ASN1_GENERALIZEDTIME)
        {
            convertGeneralizedTimeToStandard((char*)time.notBefore.value, utcOffset, notBefore);
        }
        info_str += notBefore;
        info_str += ";";
        if (time.notAfter.tag == EASY_ASN1_UTCTIME)
        {
            convertUTCTimeToStandard((char*)time.notAfter.value, utcOffset, notAfter);
        }
        else if (time.notAfter.tag == EASY_ASN1_GENERALIZEDTIME)
        {
            convertGeneralizedTimeToStandard((char*)time.notAfter.value, utcOffset, notAfter);
        }
        info_str += notAfter;

        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_SUBJECT)
    {
        Name        subject = cert->tbsCertificate.subject;
        std::string info_str;
        char        temp[MAX_OID] = {0};
        for (size_t i = 0; i < subject.count; i++)
        {
            for (size_t j = 0; j < subject.names[i].count; j++)
            {
                AttributeTypeAndValue rdn = subject.names[i].rdn[j];
                oid_to_string(rdn.type.value, rdn.type.length, temp);
                auto typeName = GMSOF::instance().GetX500Name(temp);
                info_str += typeName;
                info_str += "=";
                info_str += std::string((char*)rdn.value.value, rdn.value.length);
                info_str += ";";
            }
        }
        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_DER_PUBLIC_KEY)
    {
    }
    else if (Type == SGD_CERT_DER_EXTENSIONS)
    {
    }

    sm2_cert_free(cert);

    return info;
}

struct GMSOF::GMSOFImpl
{
    std::map<std::string, SM_OID_MAPPING> m_x500Map;
};

GMSOF::GMSOF()
    : m_impl(new GMSOFImpl())
{
    InitX500Map();
}

void GMSOF::InitX500Map()
{
    for (size_t i = 0; i < sizeof(sm_oid_mapping_x500) / sizeof(SM_OID_MAPPING); i++)
    {
        m_impl->m_x500Map[sm_oid_mapping_x500[i].oid_string] = sm_oid_mapping_x500[i];
    }
}

std::string GMSOF::GetX500Name(const std::string& name)
{
    return m_impl->m_x500Map[name].oid_name;
}

GMSOF::~GMSOF()
{
    m_impl->m_x500Map.clear();
}
