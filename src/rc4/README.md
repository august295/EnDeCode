# RC4



## 1. 简介

- 可变密钥长度、面向字节操作的序列密码，一个明文字节与一个密钥字节相异或产生一个密文字节。
- 典型的基于非线性数组变换的序列密码。它以一个足够大的数组S为基础，对其进行非线性变换，产生非线性的密钥流序列。
- 特点：算法简单，运行速度快，可达DES加密的10倍左右，且具有很高级别的非线性性。
- 电子信息领域加密的技术手段，用于**无线通信网络**，只有经过授权（缴纳相应费用）的用户才能享受该服务。（WLAN标准IEEE802. 11）



## 2. 原理[^1][^2]

### 2.1. 密钥调度算法

Key-scheduling algorithm (KSA)

```mathematica
for i from 0 to 255
    S[i] := i
endfor
j := 0
for i from 0 to 255
    j := (j + S[i] + key[i mod keylength]) mod 256
    swap values of S[i] and S[j]
endfor
```

### 2.2. 伪随机生成算法

Pseudo-random generation algorithm (PRGA)

```mathematica
i := 0
j := 0
while GeneratingOutput:
    i := (i + 1) mod 256
    j := (j + S[i]) mod 256
    swap values of S[i] and S[j]
    t := (S[i] + S[j]) mod 256
    K := S[t]
    output K
endwhile
```



# 参考

[^1]: [RC4 - 维基百科](https://zh.wikipedia.org/wiki/RC4)
[^2]: [cipher Directory Reference](https://www.oryx-embedded.com/doc/dir_2cb2aeee2eddef4bebd8163a7172f1a3.html)