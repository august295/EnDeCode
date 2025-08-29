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
    if (*seal == NULL)
    {
        return;
    }
    SEF_InitSeal(*seal);

    // SESeal = eSealInfo + cert + signAlgID + signedValue
    easy_asn1_tree_st* eSealInfoTree = easy_asn1_get_tree_item(tree, 0);

    // eSealInfo
    easy_asn1_tree_st* headerTree   = easy_asn1_get_tree_item(eSealInfoTree, 0);
    easy_asn1_tree_st* sealIDTree   = easy_asn1_get_tree_item(eSealInfoTree, 1);
    easy_asn1_tree_st* propertyTree = easy_asn1_get_tree_item(eSealInfoTree, 2);
    easy_asn1_tree_st* pictureTree  = easy_asn1_get_tree_item(eSealInfoTree, 3);

    // SES_Header = ID + version + Vid
    easy_asn1_tree_st* versionNode = easy_asn1_get_tree_item(headerTree, 1);
    char*              version     = easy_asn1_print_integer(versionNode->value.value, versionNode->value.length);
    (*seal)->version               = atoi(version);
    free(version);
    easy_asn1_tree_st* verderID = easy_asn1_get_tree_item(headerTree, 2);
    copy_string(verderID->value, (*seal)->verderID);

    // SES_esID
    easy_asn1_tree_st* esID = sealIDTree;
    copy_string(esID->value, (*seal)->sealID);

    // SES_ESPropertyInfo = type + name + certListType + certList + createDate + validStart + validEnd
    easy_asn1_tree_st* typeNode = easy_asn1_get_tree_item(propertyTree, 0);
    char*              type     = easy_asn1_print_integer(typeNode->value.value, typeNode->value.length);
    (*seal)->type               = atoi(type);
    free(type);
    easy_asn1_tree_st* name = easy_asn1_get_tree_item(propertyTree, 1);
    copy_string(name->value, (*seal)->name);
    easy_asn1_tree_st* certListTypeNode = easy_asn1_get_tree_item(propertyTree, 2);
    char*              certListType     = easy_asn1_print_integer(certListTypeNode->value.value, certListTypeNode->value.length);
    (*seal)->certListType               = atoi(certListType);
    free(certListType);

    // certList
    easy_asn1_tree_st* certList = easy_asn1_get_tree_item(propertyTree, 3);
    if ((*seal)->certListType == CLT_CERT)
    {
        (*seal)->certListInfoLen = certList->value.length;
        (*seal)->certListInfo    = (unsigned char*)malloc(sizeof(unsigned char) * certList->value.length);
        memcpy((*seal)->certListInfo, certList->value.value, certList->value.length);
    }
    else if ((*seal)->certListType == CLT_CERT_HASH)
    {
    }

    easy_asn1_tree_st* createDate = easy_asn1_get_tree_item(propertyTree, 4);
    copy_time(createDate->value, (*seal)->createDate);
    easy_asn1_tree_st* validStart = easy_asn1_get_tree_item(propertyTree, 5);
    copy_time(validStart->value, (*seal)->validStart);
    easy_asn1_tree_st* validEnd = easy_asn1_get_tree_item(propertyTree, 6);
    copy_time(validEnd->value, (*seal)->validEnd);

    // SES_ESPictureInfo = type + data + width + height
    easy_asn1_tree_st* imageType = easy_asn1_get_tree_item(pictureTree, 0);
    copy_string(imageType->value, (*seal)->imageType);
    easy_asn1_tree_st* imageData = easy_asn1_get_tree_item(pictureTree, 1);
    (*seal)->imageDataLen        = imageData->value.length;
    (*seal)->imageData           = (unsigned char*)malloc(sizeof(unsigned char) * (*seal)->imageDataLen);
    memcpy((*seal)->imageData, imageData->value.value, (*seal)->imageDataLen);
    easy_asn1_tree_st* widthNode = easy_asn1_get_tree_item(pictureTree, 2);
    char*              width     = easy_asn1_print_integer(widthNode->value.value, widthNode->value.length);
    (*seal)->imageWidth          = atoi(width);
    free(width);
    easy_asn1_tree_st* heightNode = easy_asn1_get_tree_item(pictureTree, 3);
    char*              height     = easy_asn1_print_integer(heightNode->value.value, heightNode->value.length);
    (*seal)->imageHeight          = atoi(height);
    free(height);

    easy_asn1_tree_st* certTree        = NULL;
    easy_asn1_tree_st* signAlgIDTree   = NULL;
    easy_asn1_tree_st* signedValueTree = NULL;
    easy_asn1_tree_st* tagNode         = easy_asn1_get_tree_item(tree, 1);
    // GB/T 38540
    if (tree->children_size > 2 && tagNode->value.tag == EASY_ASN1_OCTET_STRING)
    {
        certTree        = easy_asn1_get_tree_item(tree, 1);
        signAlgIDTree   = easy_asn1_get_tree_item(tree, 2);
        signedValueTree = easy_asn1_get_tree_item(tree, 3);
    }
    // GM/T 0031
    else if (tree->children_size == 2 && (tagNode->value.tag & CONSTRUCTED))
    {
        certTree        = easy_asn1_get_tree_item(tagNode, 0);
        signAlgIDTree   = easy_asn1_get_tree_item(tagNode, 1);
        signedValueTree = easy_asn1_get_tree_item(tagNode, 2);
    }

    if (certTree)
    {
        (*seal)->makerCertLen = easy_asn1_get_tree_item(certTree, 0)->value.length;
        memcpy((*seal)->makerCert, easy_asn1_get_tree_item(pictureTree, 1)->value.value, (*seal)->makerCertLen);
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
    easy_asn1_tree_st* TBS_Sign = easy_asn1_get_tree_item(tree, 0);

    easy_asn1_tree_st* versionNode = easy_asn1_get_tree_item(TBS_Sign, 0);
    char*              version     = easy_asn1_print_integer(versionNode->value.value, versionNode->value.length);
    (*signatures)->version         = atoi(version);
    free(version);
    /**
     * 如果包含 TLV 完整数据可以直接赋值
     * 如果不包含 TLV 完整数据，则需要重新序列化
     */
    easy_asn1_tree_st* sealDataLenNode  = easy_asn1_get_tree_item(TBS_Sign, 1);
    (*signatures)->sealDataLen          = easy_asn1_serialize_string(&sealDataLenNode->value, (*signatures)->sealData);
    easy_asn1_tree_st* signDateTimeNode = easy_asn1_get_tree_item(TBS_Sign, 2);
    copy_time(signDateTimeNode->value, (*signatures)->signDateTime);
    easy_asn1_tree_st* dataHashNode = easy_asn1_get_tree_item(TBS_Sign, 3);
    (*signatures)->dataHashLen      = dataHashNode->value.length;
    memcpy((*signatures)->dataHash, dataHashNode->value.value, (*signatures)->dataHashLen);
    easy_asn1_tree_st* propertyInfoNode = easy_asn1_get_tree_item(TBS_Sign, 4);
    (*signatures)->propertyInfoLen      = propertyInfoNode->value.length;
    (*signatures)->propertyInfo         = (unsigned char*)malloc((*signatures)->propertyInfoLen);
    memcpy((*signatures)->propertyInfo, propertyInfoNode->value.value, propertyInfoNode->value.length);

    // cert + signatureAlgorithm + signature + <timeStamp>
    easy_asn1_tree_st* secondNode             = easy_asn1_get_tree_item(tree, 1);
    easy_asn1_tree_st* certNode               = NULL;
    easy_asn1_tree_st* signatureAlgorithmNode = NULL;
    easy_asn1_tree_st* signatureNode          = NULL;
    if (secondNode->value.tag == EASY_ASN1_OCTET_STRING && tree->children_size > 2)
    {
        certNode               = secondNode;
        signatureAlgorithmNode = easy_asn1_get_tree_item(tree, 2);
        signatureNode          = easy_asn1_get_tree_item(tree, 3);

        // 自定义数据
        if (TBS_Sign->children_size > 5)
        {
            easy_asn1_tree_st* extDatasNode = easy_asn1_get_tree_item(tree, 5);
        }
        // 签名值时间戳
        if (tree->children_size > 4)
        {
            easy_asn1_tree_st* timeStampNode = easy_asn1_get_tree_item(tree, 4);
        }
    }
    else if (secondNode->value.tag == EASY_ASN1_BIT_STRING && tree->children_size == 2)
    {
        certNode               = easy_asn1_get_tree_item(TBS_Sign, 5);
        signatureAlgorithmNode = easy_asn1_get_tree_item(TBS_Sign, 6);
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