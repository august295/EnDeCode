#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "endecode/asn1/asn1_helper.h"
#include "endecode/asn1/gm_sef.h"

void copy_string(easy_asn1_string_st str, char* dest)
{
    snprintf(dest, str.length + 1, "%.*s", (int)str.length, str.value);
}

void copy_time(easy_asn1_string_st str, char* time)
{
    if (str.tag == EASY_ASN1_UTCTIME)
    {
        convertUTCTimeToStandard((char*)str.value, 8, time);
    }
    else if (str.tag == EASY_ASN1_GENERALIZEDTIME)
    {
        convertGeneralizedTimeToStandard((char*)str.value, 8, time);
    }
}

ENDECODE_API void SEF_ParseSeal(const uint8_t* data, size_t len, SEALINFO** seal)
{
    // 创建新节点
    *seal = (SEALINFO*)calloc(1, sizeof(SEALINFO));
    if (!*seal)
    {
        return;
    }

    easy_asn1_tree_st* tree = NULL;
    easy_asn1_parse(data, len, 0, 0, &tree);

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
}