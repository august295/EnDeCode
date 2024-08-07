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

工程使用 `CMake` 构建。运行以 `test` 作为启动项即可，其他编译生成动态库。

### 2.1. Windows

#### 2.1.1. Visual Studio 2022

```cmd
cmake -S"." -B"build" -G"Visual Studio 17 2022"
```

如果只想生成单个动态库，添加参数即可

```cmd
cmake -S"." -B"build" -G"Visual Studio 17 2022" -D"BUILD_SINGLE_LIBRARY=ON"
```

#### 2.1.2. Qt Creator

`CMakeLists.txt` 以 `Qt Creator` 打开即可创建工程。

### 2.2. Linux

#### 2.2.1. Unix Makefiles

```bash
# 仅编译测试
cmake -S"." -B"build" -G"Unix Makefiles"
cmake --build build -j4
./build/bin/test
```



## 3. 分类

### 3.1. 密码

#### 3.1.1. 对称密码算法（Symmetric-key Algorithm）

- [x] A5-1
- [x] RC4
- [x] SNOW_3GPP
- [x] ZUC
- [x] DES
- [x] AES
- [x] SM4

#### 3.1.2. 非对称密码算法（Asymmetric-key Algorithm）

- [ ] RSA
- [ ] ECC
- [ ] SM2
- [ ] SM9

#### 3.1.3. 摘要算法（Digest Algorithm）

- [x] MD5
- [x] SHA1
- [x] SHA2(SHA2_256)
- [x] SHA3(SHA3_256)
- [x] SM3

#### 3.1.4. 未分类（Unknown Algorithm）

- [x] BASE64
- [x] CRC

### 3.2. 证书

- [x] ASN1
- [ ] X509

