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
    easy_asn1_free_string(&cert->tbsCertificate.version);
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
        free(cert->tbsCertificate.issuer.names[i].rdn);
        cert->tbsCertificate.issuer.names[i].rdn = NULL;
    }
    cert->tbsCertificate.issuer.count = 0;
    free(cert->tbsCertificate.issuer.names);
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
        free(cert->tbsCertificate.subject.names[i].rdn);
        cert->tbsCertificate.subject.names[i].rdn = NULL;
    }
    cert->tbsCertificate.subject.count = 0;
    free(cert->tbsCertificate.subject.names);
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

    free(cert);
    cert = NULL;
}

SM2Certificate* sm2_cert_parse(const uint8_t* data, size_t len)
{
    SM2Certificate*    cert = NULL;
    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse(data, len, 0, 0, &tree);

    do
    {
        cert = (SM2Certificate*)malloc(sizeof(SM2Certificate));
        if (cert == NULL)
        {
            break;
        }
        sm2_cert_init(cert);

        if (tree)
        {
            // 解析证书主体
            if (tree->children_size > 0)
            {
                easy_asn1_tree_st* tbs = easy_asn1_get_tree_item(tree, 0);
                // 解析证书版本
                if (tbs->children_size > 0)
                {
                    easy_asn1_tree_st* versionNode = easy_asn1_get_tree_item(tbs, 0);
                    if (versionNode->children_size > 0)
                    {
                        easy_asn1_copy_string(&easy_asn1_get_tree_item(versionNode, 0)->value, &cert->tbsCertificate.version);
                    }
                }
                // 解析证书序列号
                if (tbs->children_size > 1)
                {
                    easy_asn1_tree_st* serialNumberNode = easy_asn1_get_tree_item(tbs, 1);
                    easy_asn1_copy_string(&serialNumberNode->value, &cert->tbsCertificate.serialNumber);
                }
                // 解析签名算法
                if (tbs->children_size > 2)
                {
                    easy_asn1_tree_st* signatureNode = easy_asn1_get_tree_item(tbs, 2);
                    if (signatureNode->children_size > 0)
                    {
                        easy_asn1_copy_string(&easy_asn1_get_tree_item(signatureNode, 0)->value, &cert->tbsCertificate.signature.algorithm);
                        if (signatureNode->children_size > 1)
                        {
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(signatureNode, 1)->value, &cert->tbsCertificate.signature.parameters);
                        }
                    }
                }
                // 解析颁发者
                if (tbs->children_size > 3)
                {
                    easy_asn1_tree_st* issuerNode     = easy_asn1_get_tree_item(tbs, 3);
                    cert->tbsCertificate.issuer.count = issuerNode->children_size;
                    cert->tbsCertificate.issuer.names = (RelativeDistinguishedName*)malloc(sizeof(RelativeDistinguishedName) * issuerNode->children_size);
                    if (cert->tbsCertificate.issuer.names == NULL)
                    {
                        break;
                    }
                    for (uint32_t i = 0; i < issuerNode->children_size; i++)
                    {
                        easy_asn1_tree_st* dnNode                  = easy_asn1_get_tree_item(issuerNode, i);
                        cert->tbsCertificate.issuer.names[i].count = dnNode->children_size;
                        cert->tbsCertificate.issuer.names[i].rdn   = (AttributeTypeAndValue*)malloc(sizeof(AttributeTypeAndValue) * dnNode->children_size);
                        if (cert->tbsCertificate.issuer.names[i].rdn == NULL)
                        {
                            break;
                        }
                        for (uint32_t j = 0; j < cert->tbsCertificate.issuer.names[i].count; j++)
                        {
                            easy_asn1_tree_st* rdnNode = easy_asn1_get_tree_item(dnNode, j);
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(rdnNode, 0)->value, &cert->tbsCertificate.issuer.names[i].rdn[j].type);
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(rdnNode, 1)->value, &cert->tbsCertificate.issuer.names[i].rdn[j].value);
                        }
                    }
                }
                // 解析有效期
                if (tbs->children_size > 4)
                {
                    easy_asn1_tree_st* validityNode = easy_asn1_get_tree_item(tbs, 4);
                    if (validityNode && validityNode->children_size > 1)
                    {
                        easy_asn1_copy_string(&easy_asn1_get_tree_item(validityNode, 0)->value, &cert->tbsCertificate.validity.notBefore);
                        easy_asn1_copy_string(&easy_asn1_get_tree_item(validityNode, 1)->value, &cert->tbsCertificate.validity.notAfter);
                    }
                }
                // 解析使用者
                if (tbs->children_size > 5)
                {
                    easy_asn1_tree_st* subjectNode     = easy_asn1_get_tree_item(tbs, 5);
                    cert->tbsCertificate.subject.count = subjectNode->children_size;
                    cert->tbsCertificate.subject.names = (RelativeDistinguishedName*)malloc(sizeof(RelativeDistinguishedName) * subjectNode->children_size);
                    if (cert->tbsCertificate.subject.names == NULL)
                    {
                        break;
                    }
                    for (uint32_t i = 0; i < subjectNode->children_size; i++)
                    {
                        easy_asn1_tree_st* dnNode                   = easy_asn1_get_tree_item(subjectNode, i);
                        cert->tbsCertificate.subject.names[i].count = dnNode->children_size;
                        cert->tbsCertificate.subject.names[i].rdn   = (AttributeTypeAndValue*)malloc(sizeof(AttributeTypeAndValue) * dnNode->children_size);
                        if (cert->tbsCertificate.subject.names[i].rdn == NULL)
                        {
                            break;
                        }
                        for (uint32_t j = 0; j < cert->tbsCertificate.subject.names[i].count; j++)
                        {
                            easy_asn1_tree_st* rdnNode = easy_asn1_get_tree_item(dnNode, j);
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(rdnNode, 0)->value, &cert->tbsCertificate.subject.names[i].rdn[j].type);
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(rdnNode, 1)->value, &cert->tbsCertificate.subject.names[i].rdn[j].value);
                        }
                    }
                }
                // 解析公钥信息
                if (tbs->children_size > 6)
                {
                    easy_asn1_tree_st* subjectPublicKeyInfoNode = easy_asn1_get_tree_item(tbs, 6);
                    if (subjectPublicKeyInfoNode->children_size > 0)
                    {
                        easy_asn1_tree_st* subjectPublicKeyInfoAlgorithmNode = easy_asn1_get_tree_item(subjectPublicKeyInfoNode, 0);
                        if (subjectPublicKeyInfoAlgorithmNode->children_size > 0)
                        {
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(subjectPublicKeyInfoAlgorithmNode, 0)->value, &cert->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm);
                        }
                        if (subjectPublicKeyInfoAlgorithmNode->children_size > 1)
                        {
                            easy_asn1_copy_string(&easy_asn1_get_tree_item(subjectPublicKeyInfoAlgorithmNode, 1)->value, &cert->tbsCertificate.subjectPublicKeyInfo.algorithm.parameters);
                        }
                    }
                    if (subjectPublicKeyInfoNode->children_size > 1)
                    {
                        easy_asn1_tree_st* subjectPublicKeyNode = easy_asn1_get_tree_item(subjectPublicKeyInfoNode, 1);
                        easy_asn1_copy_string(&subjectPublicKeyNode->value, &cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey);
                    }
                }
                // 解析颁发者唯一标识符（不使用）
                // 解析主体唯一标识符（不使用）
                // 解析扩展项
                if (tbs->children_size > 7)
                {
                    easy_asn1_tree_st* extensionsNode = easy_asn1_get_tree_item(tbs, 7);
                    easy_asn1_copy_string(&extensionsNode->value, &cert->tbsCertificate.extensions);
                }
            }

            // 解析算法标识
            if (tree->children_size > 1)
            {
                easy_asn1_tree_st* signatureAlgorithm = easy_asn1_get_tree_item(tree, 1);
                if (signatureAlgorithm->children_size > 0)
                {
                    easy_asn1_copy_string(&easy_asn1_get_tree_item(signatureAlgorithm, 0)->value, &cert->signatureAlgorithm.algorithm);
                }
                if (signatureAlgorithm->children_size > 1)
                {
                    easy_asn1_copy_string(&easy_asn1_get_tree_item(signatureAlgorithm, 1)->value, &cert->signatureAlgorithm.parameters);
                }
            }

            // 解析数字签名结果
            if (tree->children_size > 2)
            {
                easy_asn1_tree_st* signatureValue = easy_asn1_get_tree_item(tree, 2);
                easy_asn1_copy_string(&signatureValue->value, &cert->signatureValue);
            }
        }
    } while (0);

    easy_asn1_free_tree(tree);
    return cert;
}
