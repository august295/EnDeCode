#include <malloc.h>
#include <memory.h>

#include "endecode/asn1/cert_sm2.h"

void sm2_cert_init(SM2Certificate* cert)
{
    // tbsCertificate
    easy_asn1_init_string(&cert->tbsCertificate.serialNumber);
    easy_asn1_init_string(&cert->tbsCertificate.signature.algorithm);
    easy_asn1_init_string(&cert->tbsCertificate.signature.parameters);
    cert->tbsCertificate.issuer.count = 0;
    cert->tbsCertificate.issuer.names = NULL;
    easy_asn1_init_string(&cert->tbsCertificate.validity.notBefore);
    easy_asn1_init_string(&cert->tbsCertificate.validity.notAfter);
    cert->tbsCertificate.subject.count = 0;
    cert->tbsCertificate.subject.names = NULL;
    easy_asn1_init_string(&cert->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm);
    easy_asn1_init_string(&cert->tbsCertificate.subjectPublicKeyInfo.algorithm.parameters);
    easy_asn1_init_string(&cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey);
    easy_asn1_init_string(&cert->tbsCertificate.issuerUniqueId);
    easy_asn1_init_string(&cert->tbsCertificate.subjectUniqueId);
    easy_asn1_init_string(&cert->tbsCertificate.extensions);

    // signatureAlgorithm
    easy_asn1_init_string(&cert->signatureAlgorithm.algorithm);
    easy_asn1_init_string(&cert->signatureAlgorithm.parameters);

    // signatureValue
    easy_asn1_init_string(&cert->signatureValue);
}

void sm2_cert_free(SM2Certificate* cert)
{
    // tbsCertificate
    easy_asn1_free_string(&cert->tbsCertificate.serialNumber);
    easy_asn1_free_string(&cert->tbsCertificate.signature.algorithm);
    easy_asn1_free_string(&cert->tbsCertificate.signature.parameters);
    for (uint32_t i = 0; i < cert->tbsCertificate.issuer.count; i++)
    {
        for (uint32_t j = 0; j < cert->tbsCertificate.issuer.names[i].count; j++)
        {
            easy_asn1_free_string(&cert->tbsCertificate.issuer.names[i].rdn[j].type);
            easy_asn1_free_string(&cert->tbsCertificate.issuer.names[i].rdn[j].value);
        }
        cert->tbsCertificate.issuer.names[i].count = 0;
    }
    cert->tbsCertificate.issuer.count = 0;
    cert->tbsCertificate.issuer.names = NULL;
    easy_asn1_free_string(&cert->tbsCertificate.validity.notBefore);
    easy_asn1_free_string(&cert->tbsCertificate.validity.notAfter);
    for (uint32_t i = 0; i < cert->tbsCertificate.subject.count; i++)
    {
        for (uint32_t j = 0; j < cert->tbsCertificate.subject.names[i].count; j++)
        {
            easy_asn1_free_string(&cert->tbsCertificate.subject.names[i].rdn[j].type);
            easy_asn1_free_string(&cert->tbsCertificate.subject.names[i].rdn[j].value);
        }
        cert->tbsCertificate.subject.names[i].count = 0;
    }
    cert->tbsCertificate.subject.count = 0;
    cert->tbsCertificate.subject.names = NULL;
    easy_asn1_free_string(&cert->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm);
    easy_asn1_free_string(&cert->tbsCertificate.subjectPublicKeyInfo.algorithm.parameters);
    easy_asn1_free_string(&cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey);
    easy_asn1_free_string(&cert->tbsCertificate.issuerUniqueId);
    easy_asn1_free_string(&cert->tbsCertificate.subjectUniqueId);
    easy_asn1_free_string(&cert->tbsCertificate.extensions);

    // signatureAlgorithm
    easy_asn1_free_string(&cert->signatureAlgorithm.algorithm);
    easy_asn1_free_string(&cert->signatureAlgorithm.parameters);

    // signatureValue
    easy_asn1_free_string(&cert->signatureValue);
}

SM2Certificate* sm2_cert_parse(const uint8_t* data, size_t len)
{
    SM2Certificate*    cert = NULL;
    easy_asn1_tree_st* tree = NULL;
    easy_asn1_parse(data, len, 0, &tree);

    do
    {
        cert = (SM2Certificate*)calloc(1, sizeof(SM2Certificate));
        if (cert == NULL)
        {
            break;
        }
        if (tree)
        {
            // 解析证书主体
            if (tree->children_size > 0)
            {
                easy_asn1_tree_st* tbs = tree->children[0];
                // 解析证书版本
                if (tbs->children_size > 0)
                {
                    easy_asn1_tree_st* versionNode = tbs->children[0];
                    if (versionNode->children_size > 0)
                    {
                        easy_asn1_copy_string(&versionNode->children[0]->value, &cert->tbsCertificate.version);
                    }
                }
                // 解析证书序列号
                if (tbs->children_size > 1)
                {
                    easy_asn1_tree_st* serialNumberNode = tbs->children[1];
                    easy_asn1_copy_string(&serialNumberNode->value, &cert->tbsCertificate.serialNumber);
                }
                // 解析签名算法
                if (tbs->children_size > 2)
                {
                    easy_asn1_tree_st* signatureNode = tbs->children[2];
                    if (signatureNode->children)
                    {
                        easy_asn1_copy_string(&signatureNode->children[0]->value, &cert->tbsCertificate.signature.algorithm);
                        if (signatureNode->children_size > 1)
                        {
                            easy_asn1_copy_string(&signatureNode->children[1]->value, &cert->tbsCertificate.signature.parameters);
                        }
                    }
                }
                // 解析颁发者
                if (tbs->children_size > 3)
                {
                    easy_asn1_tree_st* issuerNode     = tbs->children[3];
                    cert->tbsCertificate.issuer.count = issuerNode->children_size;
                    cert->tbsCertificate.issuer.names = (RelativeDistinguishedName*)calloc(1, sizeof(RelativeDistinguishedName) * issuerNode->children_size);
                    if (cert->tbsCertificate.issuer.names == NULL)
                    {
                        break;
                    }
                    for (uint32_t i = 0; i < issuerNode->children_size; i++)
                    {
                        easy_asn1_tree_st* dnNode                  = issuerNode->children[i];
                        cert->tbsCertificate.issuer.names[i].count = dnNode->children_size;
                        cert->tbsCertificate.issuer.names[i].rdn   = (AttributeTypeAndValue*)calloc(1, sizeof(AttributeTypeAndValue) * dnNode->children_size);
                        if (cert->tbsCertificate.issuer.names[i].rdn == NULL)
                        {
                            break;
                        }
                        for (uint32_t j = 0; j < cert->tbsCertificate.issuer.names[i].count; j++)
                        {
                            easy_asn1_tree_st* rdnNode = dnNode->children[j];
                            easy_asn1_copy_string(&rdnNode->children[0]->value, &cert->tbsCertificate.issuer.names[i].rdn[j].type);
                            easy_asn1_copy_string(&rdnNode->children[1]->value, &cert->tbsCertificate.issuer.names[i].rdn[j].value);
                        }
                    }
                }
                // 解析有效期
                if (tbs->children_size > 4)
                {
                    easy_asn1_tree_st* validityNode = tbs->children[4];
                    if (validityNode && validityNode->children_size > 1)
                    {
                        easy_asn1_copy_string(&validityNode->children[0]->value, &cert->tbsCertificate.validity.notBefore);
                        easy_asn1_copy_string(&validityNode->children[1]->value, &cert->tbsCertificate.validity.notAfter);
                    }
                }
                // 解析使用者
                if (tbs->children_size > 5)
                {
                    easy_asn1_tree_st* subjectNode     = tbs->children[5];
                    cert->tbsCertificate.subject.count = subjectNode->children_size;
                    cert->tbsCertificate.subject.names = (RelativeDistinguishedName*)calloc(1, sizeof(RelativeDistinguishedName) * subjectNode->children_size);
                    if (cert->tbsCertificate.subject.names == NULL)
                    {
                        break;
                    }
                    for (uint32_t i = 0; i < subjectNode->children_size; i++)
                    {
                        easy_asn1_tree_st* dnNode                   = subjectNode->children[i];
                        cert->tbsCertificate.subject.names[i].count = dnNode->children_size;
                        cert->tbsCertificate.subject.names[i].rdn   = (AttributeTypeAndValue*)calloc(1, sizeof(AttributeTypeAndValue) * dnNode->children_size);
                        if (cert->tbsCertificate.subject.names[i].rdn == NULL)
                        {
                            break;
                        }
                        for (uint32_t j = 0; j < cert->tbsCertificate.subject.names[i].count; j++)
                        {
                            easy_asn1_tree_st* rdnNode = dnNode->children[j];
                            easy_asn1_copy_string(&rdnNode->children[0]->value, &cert->tbsCertificate.subject.names[i].rdn[j].type);
                            easy_asn1_copy_string(&rdnNode->children[1]->value, &cert->tbsCertificate.subject.names[i].rdn[j].value);
                        }
                    }
                }
                // 解析公钥信息
                if (tbs->children_size > 6)
                {
                    easy_asn1_tree_st* subjectPublicKeyInfoNode = tbs->children[6];
                    if (subjectPublicKeyInfoNode->children_size > 0)
                    {
                        if (subjectPublicKeyInfoNode->children[0]->children_size > 0)
                        {
                            easy_asn1_copy_string(&subjectPublicKeyInfoNode->children[0]->children[0]->value, &cert->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm);
                        }
                        if (subjectPublicKeyInfoNode->children[0]->children_size > 1)
                        {
                            easy_asn1_copy_string(&subjectPublicKeyInfoNode->children[0]->children[1]->value, &cert->tbsCertificate.subjectPublicKeyInfo.algorithm.parameters);
                        }
                    }
                    if (subjectPublicKeyInfoNode->children_size > 1)
                    {
                        easy_asn1_copy_string(&subjectPublicKeyInfoNode->children[1]->value, &cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey);
                    }
                }
                // 解析颁发者唯一标识符（不使用）
                // 解析主体唯一标识符（不使用）
                // 解析扩展项
                if (tbs->children_size > 7)
                {
                    easy_asn1_tree_st* extensionsNode = tbs->children[7];
                    easy_asn1_copy_string(&extensionsNode->value, &cert->tbsCertificate.extensions);
                }
            }

            // 解析算法标识
            if (tree->children_size > 1)
            {
                easy_asn1_tree_st* signatureAlgorithm = tree->children[1];
                if (signatureAlgorithm->children_size > 0)
                {
                    easy_asn1_copy_string(&signatureAlgorithm->children[0]->value, &cert->signatureAlgorithm.algorithm);
                }
                if (signatureAlgorithm->children_size > 1)
                {
                    easy_asn1_copy_string(&signatureAlgorithm->children[1]->value, &cert->signatureAlgorithm.parameters);
                }
            }

            // 解析数字签名结果
            if (tree->children_size > 2)
            {
                easy_asn1_tree_st* signatureValue = tree->children[2];
                easy_asn1_copy_string(&signatureValue->value, &cert->signatureValue);
            }
        }
    } while (0);

    easy_asn1_free_tree(tree);
    return cert;
}
