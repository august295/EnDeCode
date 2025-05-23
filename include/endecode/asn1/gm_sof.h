#ifndef __GM_SOF_H__
#define __GM_SOF_H__

#include <string>
#include <map>

#include "endecode/asn1/cert_sm2.h"
#include "endecode/asn1/gm_type.h"

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   获取证书指定类型的信息
 * @param   Base64Cert              [IN]        Base64编码的证书字符串
 * @param   Type                    [IN]        证书解析标识，应符合 GM/T 0006
 * @return  证书信息                            成功，返回证书指定类型的信息
 *          空串                                失败或证书中不存在该项内容
 */
ENDECODE_API BSTR SOF_GetCertInfo(BSTR  Base64Cert,
                                  SHORT Type);

#ifdef __cplusplus
}
#endif

class GMSOF
{
public:
    static GMSOF& instance()
    {
        static GMSOF instance;
        return instance;
    }

    void InitX500Map();

    std::string GetX500Name(const std::string& name);
    std::string Capture(const std::string& str, const std::string& flag);

private:
    GMSOF();
    ~GMSOF();
    GMSOF(const GMSOF&)            = delete;
    GMSOF& operator=(const GMSOF&) = delete;
    GMSOF(GMSOF&&)                 = delete;
    GMSOF& operator=(GMSOF&&)      = delete;

private:
    struct GMSOFImpl;
    GMSOFImpl* m_impl;
};

#endif