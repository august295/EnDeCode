#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/asn1_helper.h"
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
    else if (Type == SGD_CERT_ISSUER || Type == SGD_CERT_ISSUER_CN || Type == SGD_CERT_ISSUER_O || Type == SGD_CERT_ISSUER_OU)
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

        if (Type == SGD_CERT_ISSUER_CN)
        {
            info_str = GMSOF::instance().Capture(info_str, "CN=");
        }
        else if (Type == SGD_CERT_ISSUER_O)
        {
            info_str = GMSOF::instance().Capture(info_str, "O=");
        }
        else if (Type == SGD_CERT_ISSUER_OU)
        {
            info_str = GMSOF::instance().Capture(info_str, "OU=");
        }
        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_VALID_TIME || Type == SGD_CERT_NOTBEFORE_TIME || Type == SGD_CERT_NOTAFTER_TIME)
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

        if (Type == SGD_CERT_NOTBEFORE_TIME)
        {
            info_str = notBefore;
        }
        else if (Type == SGD_CERT_NOTAFTER_TIME)
        {
            info_str = notAfter;
        }
        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_SUBJECT || Type == SGD_CERT_SUBJECT_CN || Type == SGD_CERT_SUBJECT_O || Type == SGD_CERT_SUBJECT_OU)
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

        if (Type == SGD_CERT_SUBJECT_CN)
        {
            info_str = GMSOF::instance().Capture(info_str, "CN=");
        }
        else if (Type == SGD_CERT_SUBJECT_O)
        {
            info_str = GMSOF::instance().Capture(info_str, "O=");
        }
        else if (Type == SGD_CERT_SUBJECT_OU)
        {
            info_str = GMSOF::instance().Capture(info_str, "OU=");
        }
        infoLen = info_str.size();
        info    = (BSTR)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_DER_PUBLIC_KEY)
    {
        SubjectPublicKeyInfo subjectPublicKeyInfo = cert->tbsCertificate.subjectPublicKeyInfo;

        std::string info_str;
        char        temp[MAX_OID] = {0};
        char        tempHex[3]    = {0};
        oid_to_string(subjectPublicKeyInfo.algorithm.algorithm.value, subjectPublicKeyInfo.algorithm.algorithm.length, temp);
        info_str += temp;
        info_str += ";";
        for (size_t i = 0; i < subjectPublicKeyInfo.subjectPublicKey.length; i++)
        {
            sprintf(tempHex, "%02X", subjectPublicKeyInfo.subjectPublicKey.value[i]);
            if (i % 16)
            {
                info_str += std::string(tempHex) + " ";
            }
            else
            {
                if (i != 0)
                {
                    info_str += std::string(tempHex) + "\r\n";
                }
            }
        }
        infoLen = info_str.size();
        info    = (char*)calloc(1, infoLen + 1);
        memcpy(info, info_str.c_str(), info_str.size());
    }
    else if (Type == SGD_CERT_DER_EXTENSIONS)
    {
        infoLen = cert->tbsCertificate.extensions.length;
        info    = (char*)calloc(1, infoLen + 1);
        memcpy(info, cert->tbsCertificate.extensions.value, infoLen);
    }

    sm2_cert_free(cert);

    size_t infoBase64Len = BASE64_ENCODE_OUT_SIZE(infoLen);
    char*  infoBase64    = (char*)calloc(1, infoBase64Len + 1);
    base64_encode((uint8_t*)info, infoLen, infoBase64);
    return (BSTR)infoBase64;
}

struct GMSOF::GMSOFImpl
{
    std::map<std::string, OID_MAPPING> m_x500Map;
};

GMSOF::GMSOF()
    : m_impl(new GMSOFImpl())
{
    InitX500Map();
}

GMSOF::~GMSOF()
{
    m_impl->m_x500Map.clear();
}

void GMSOF::InitX500Map()
{
    std::string  filename        = "./oid.json";
    OID_MAPPING* oid_mapping     = NULL;
    size_t       oid_mapping_len = 0;
    ReadOid(filename.c_str(), &oid_mapping, &oid_mapping_len);
    for (size_t i = 0; i < oid_mapping_len; i++)
    {
        m_impl->m_x500Map[oid_mapping[i].oid_string] = oid_mapping[i];
    }
}

std::string GMSOF::GetX500Name(const std::string& name)
{
    return m_impl->m_x500Map[name].oid_name;
}

std::string GMSOF::Capture(const std::string& str, const std::string& flag)
{
    std::string res;
    size_t      start = str.find(flag);
    size_t      end   = 0;
    if (start != std::string::npos)
    {
        size_t temp = start + flag.size();
        end         = str.substr(temp).find(";");
        if (end != std::string::npos)
        {
            res = str.substr(temp, end);
        }
    }
    return res;
}
