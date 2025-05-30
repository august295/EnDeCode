#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "endecode/asn1/asn1_helper.h"
#include "endecode/asn1/gm_sef.h"

void SEF_InitSeal(SEALINFO* seal)
{
    seal->certListInfo = NULL;
    seal->imageData    = NULL;
}

void SEF_FreeSeal(SEALINFO* seal)
{
    free(seal->certListInfo);
    free(seal->imageData);
    free(seal);
}

void SEF_InitSignatures(SIGNEDVALUEINFO* signatures)
{
    signatures->propertyInfo = NULL;
    signatures->timeStamp    = NULL;
}

void SEF_FreeSignatures(SIGNEDVALUEINFO* signatures)
{
    free(signatures->propertyInfo);
    free(signatures->timeStamp);
    free(signatures);
}

void copy_string(easy_asn1_string_st str, char* dest)
{
    snprintf(dest, str.length + 1, "%.*s", (int)str.length, str.value);
}

void copy_time(easy_asn1_string_st str, char* time)
{
    if (str.tag == EASY_ASN1_UTCTIME)
    {
        convertUTCTimeToStandard((char*)str.value, str.length, 8, time);
    }
    else if (str.tag == EASY_ASN1_GENERALIZEDTIME)
    {
        convertGeneralizedTimeToStandard((char*)str.value, str.length, 8, time);
    }
}

void SEF_ParseSeal(const easy_asn1_tree_st* tree, SEALINFO** seal)
{
    if (tree == NULL)
    {
        return;
    }

    // 创建新节点
    *seal = (SEALINFO*)malloc(sizeof(SEALINFO));
    if (!*seal)
    {
        return;
    }
    SEF_InitSeal(*seal);

    // SESeal = eSealInfo + cert + signAlgID + signedValue
    easy_asn1_tree_st* eSealInfoTree = tree->children[0];

    // eSealInfo
    easy_asn1_tree_st* headerTree   = eSealInfoTree->children[0];
    easy_asn1_tree_st* propertyTree = eSealInfoTree->children[2];
    easy_asn1_tree_st* pictureTree  = eSealInfoTree->children[3];

    // SES_Header = ID + version + Vid
    copy_string(headerTree->children[0]->value, (*seal)->sealID);
    char* version    = easy_asn1_print_integer(headerTree->children[1]->value.value, headerTree->children[1]->value.length);
    (*seal)->version = atoi(version);
    free(version);
    copy_string(headerTree->children[2]->value, (*seal)->verderID);

    // SES_ESPropertyInfo = type + name + certListType + certList + createDate + validStart + validEnd
    char* type    = easy_asn1_print_integer(propertyTree->children[0]->value.value, propertyTree->children[0]->value.length);
    (*seal)->type = atoi(type);
    free(type);
    copy_string(propertyTree->children[1]->value, (*seal)->name);
    char* certListType    = easy_asn1_print_integer(propertyTree->children[2]->value.value, propertyTree->children[2]->value.length);
    (*seal)->certListType = atoi(certListType);
    free(certListType);

    // certList
    easy_asn1_tree_st* certList = propertyTree->children[3];
    if ((*seal)->certListType == CLT_CERT)
    {
        (*seal)->certListInfoLen = certList->value.length;
        (*seal)->certListInfo    = (unsigned char*)malloc(sizeof(unsigned char) * certList->value.length);
        memcpy((*seal)->certListInfo, certList->value.value, certList->value.length);
    }
    else if ((*seal)->certListType == CLT_CERT_HASH)
    {
    }

    copy_time(propertyTree->children[4]->value, (*seal)->createDate);
    copy_time(propertyTree->children[5]->value, (*seal)->validStart);
    copy_time(propertyTree->children[6]->value, (*seal)->validEnd);

    // SES_ESPictureInfo = type + data + width + height
    copy_string(pictureTree->children[0]->value, (*seal)->imageType);
    (*seal)->imageDataLen = pictureTree->children[1]->value.length;
    (*seal)->imageData    = (unsigned char*)malloc(sizeof(unsigned char) * (*seal)->imageDataLen);
    memcpy((*seal)->imageData, pictureTree->children[1]->value.value, (*seal)->imageDataLen);
    char* width         = easy_asn1_print_integer(pictureTree->children[2]->value.value, pictureTree->children[2]->value.length);
    (*seal)->imageWidth = atoi(width);
    free(width);
    char* height         = easy_asn1_print_integer(pictureTree->children[3]->value.value, pictureTree->children[3]->value.length);
    (*seal)->imageHeight = atoi(height);
    free(height);

    easy_asn1_tree_st* certTree        = NULL;
    easy_asn1_tree_st* signAlgIDTree   = NULL;
    easy_asn1_tree_st* signedValueTree = NULL;
    // GB/T 38540
    if (tree->children_size > 2 && tree->children[1]->value.tag == EASY_ASN1_OCTET_STRING)
    {
        certTree        = tree->children[1];
        signAlgIDTree   = tree->children[2];
        signedValueTree = tree->children[3];
    }
    // GM/T 0031
    else if (tree->children_size == 2 && (tree->children[1]->value.tag & CONSTRUCTED))
    {
        certTree        = tree->children[1]->children[0];
        signAlgIDTree   = tree->children[1]->children[1];
        signedValueTree = tree->children[1]->children[2];
    }

    if (certTree)
    {
        (*seal)->makerCertLen = certTree->children[0]->value.length;
        memcpy((*seal)->makerCert, pictureTree->children[1]->value.value, (*seal)->makerCertLen);
    }
    if (signAlgIDTree)
    {
        oid_to_string(signAlgIDTree->value.value, signAlgIDTree->value.length, (*seal)->signatureAlgOID);
    }
    if (signedValueTree)
    {
    }
}

void SEF_ParseSignatures(const easy_asn1_tree_st* tree, SIGNEDVALUEINFO** signatures)
{
    if (tree == NULL)
    {
        return;
    }

    // 创建新节点
    *signatures = (SIGNEDVALUEINFO*)malloc(sizeof(SIGNEDVALUEINFO));
    if (!*signatures)
    {
        return;
    }
    SEF_InitSignatures(*signatures);

    // TBS_Sign = version + eseal + timeInnfo + dataHash + propertyInfo + [cert + signatureAlgorithm]
    easy_asn1_tree_st* TBS_Sign = tree->children[0];
    char*              version  = easy_asn1_print_integer(TBS_Sign->children[0]->value.value, TBS_Sign->children[0]->value.length);
    (*signatures)->version      = atoi(version);
    free(version);
    /**
     * 如果包含 TLV 完整数据可以直接赋值
     * 如果不包含 TLV 完整数据，则需要重新序列化
     */
    (*signatures)->sealDataLen = easy_asn1_serialize_string(&TBS_Sign->children[1]->value, (*signatures)->sealData);
    copy_time(TBS_Sign->children[2]->value, (*signatures)->signDateTime);
    (*signatures)->dataHashLen = TBS_Sign->children[3]->value.length;
    memcpy((*signatures)->dataHash, TBS_Sign->children[3]->value.value, (*signatures)->dataHashLen);
    (*signatures)->propertyInfoLen = TBS_Sign->children[4]->value.length;
    (*signatures)->propertyInfo    = (unsigned char*)malloc((*signatures)->propertyInfoLen);
    memcpy((*signatures)->propertyInfo, TBS_Sign->children[4]->value.value, TBS_Sign->children[4]->value.length);

    // cert + signatureAlgorithm + signature + <timeStamp>
    easy_asn1_tree_st* secondNode             = tree->children[1];
    easy_asn1_tree_st* certNode               = NULL;
    easy_asn1_tree_st* signatureAlgorithmNode = NULL;
    easy_asn1_tree_st* signatureNode          = NULL;
    if (secondNode->value.tag == EASY_ASN1_OCTET_STRING && tree->children_size > 2)
    {
        certNode               = secondNode;
        signatureAlgorithmNode = tree->children[2];
        signatureNode          = tree->children[3];

        // 自定义数据
        if (TBS_Sign->children_size > 5)
        {
            easy_asn1_tree_st* extDatasNode = tree->children[5];
        }
        // 签名值时间戳
        if (tree->children_size > 4)
        {
            easy_asn1_tree_st* timeStampNode = tree->children[3];
        }
    }
    else if (secondNode->value.tag == EASY_ASN1_BIT_STRING && tree->children_size == 2)
    {
        certNode               = TBS_Sign->children[5];
        signatureAlgorithmNode = TBS_Sign->children[6];
        signatureNode          = secondNode;
    }

    if (certNode)
    {
        (*signatures)->signerCertLen = certNode->value.length;
        memcpy((*signatures)->signerCert, certNode->value.value, certNode->value.length);
    }
    if (signatureAlgorithmNode)
    {
        oid_to_string(signatureAlgorithmNode->value.value, signatureAlgorithmNode->value.length, (*signatures)->signatureAlgOID);
    }
    if (signatureNode)
    {
    }
}