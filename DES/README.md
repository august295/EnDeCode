# DES

## 1. 简介







##  2. 使用

```shell
 -d, --decrypt     decrypt DES from input file
 -e, --encrypt     encrypt DES from input file
 -o, --output=FILE write result to FILE
 -h, --help        display this help
```

**例如**

```shell
生成密钥程序 
make genkey

生成密钥
genkey
(.\genkey.exe)

生成主程序
make

加密
des -k "0101110011111111000101110101100111000101111001110100110100001001" -e a.txt -o b.txt
(.\des.exe)
解密
des -k "0101110011111111000101110101100111000101111001110100110100001001" -d b.txt -o c.txt
(.\des.exe)
```



## 3. 原理

