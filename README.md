[TOC]



# 密码学

实现一些常见的加密算法。



## 1. 环境

- `Visual Studio 2022`
- `CMake3.15`



## 2. 下载及编译

使用 `GoogleTest` 作为测试工具，所以需要下载 `submodule`。

```
git clone --recursive https://github.com/august295/EnDeCode.git
```

工程使用 `CMake` 构建。

运行以 `test` 作为启动项即可，其他编译生成动态库。



## 3. 分类

### 3.1. 对称密码算法（Symmetric-key Algorithm）

- [ ] A5-1
- [ ] RC4
- [x] SNOW_3GPP
- [ ] ZUC
- [x] DES/3DES
- [ ] AES
- [ ] SM4(ECB, CBC, CFB, OFB, CTR)

### 3.2. 非对称密码算法（Asymmetric-key Algorithm）

- [ ] RSA
- [ ] ECC
- [ ] SM2
- [ ] SM9

### 3.3. 摘要算法（Digest Algorithm）

- [x] MD5
- [ ] SH!-1/SHA-2
- [ ] SHA-3
- [ ] SM3

### 3.4. 未分类

- [x] BASE64
- [x] CRC