# SM3



## 1. 简介

`SM3`（商密3）是中华人民共和国政府采用的一种密码散列函数标准，前身为 `SCH4` 杂凑算法，由国家密码管理局于2010年12月17日发布，相关标准为“GM/T 0004-2012 《SM3密码杂凑算法》”。2016年，成为中国国家密码标准（GB/T 32905-2016）。

在商用密码体系中，`SM3` 主要用于数字签名及验证、消息认证码生成及验证、随机数生成等，其算法公开，安全性及效率与 `SHA-256` 相当。



## 2. 算法实现[^1]

### 2.1. 符号

| 符号         | 含义                                 |
| ------------ | ------------------------------------ |
| $ABCDEFGH$   | 8 个字寄存器或它们的值的串联         |
| $B^{(i)}$    | 第 i 个消息分组                      |
| $CF$         | 压缩函数                             |
| $FF_{j}$     | 布尔函数，随j的变化取不同的表达式    |
| $GG_{j}$     | 布尔函数，随j的变化取不同的表达式    |
| $IV$         | 初始值，用于确定压缩函数寄存器的初态 |
| $P_{0}$      | 压缩函数中的置换函数                 |
| $P_{1}$      | 消息扩展中的置换函数                 |
| $T_{j}$      | 常量，随j的变化取不同的值            |
| $m$          | 消息                                 |
| $m'$         | 填充后的消息                         |
| $mod$        | 模运算                               |
| $\wedge$     | 32比特与运算                         |
| $\vee$       | 32比特或运算                         |
| $\oplus$     | 32比特异或运算                       |
| $\neg$       | 32比特非运算                         |
| $+$          | $mod2^{32}$ 算术加运算               |
| $\lll k$     | 循环左移k比特运算                    |
| $\leftarrow$ | 左向赋值运算符                       |

### 2.2. 常数与函数

#### 2.2.1. 初始值


$$
\begin{aligned}
    IV_{0} &= 0x7380166F \\
    IV_{1} &= 0x4914B2B9 \\
    IV_{2} &= 0x172442D7 \\
    IV_{3} &= 0xDA8A0600 \\
    IV_{4} &= 0xA96F30BC \\
    IV_{5} &= 0x163138AA \\
    IV_{6} &= 0xE38DEE4D \\
    IV_{7} &= 0xB0FB0E4E \\
\end{aligned}
$$

#### 2.2.2. 常量


$$
T_{j} = 
\begin{cases}
    0x79cc4519 \qquad (0 \leq j \leq 15) \\
    0x7a879d8a \qquad (16 \leq j \leq 63)
\end{cases}
$$

#### 2.2.3. 布尔函数


$$
\begin{aligned}
    FF_{j}(X,Y,Z) &=
    \begin{cases}
        X \oplus Y \oplus Z \qquad &(0 \leq j \leq 15) \\
        (X \wedge Y) \vee (X \wedge Z) \vee (Y \wedge Z) \qquad &(16 \leq j \leq 63)
    \end{cases} \\
    GG_{j}(X,Y,Z) &=
    \begin{cases}
        X \oplus Y \oplus Z \qquad &(0 \leq j \leq 15) \\
        (X \wedge Y) \vee (X \neg Z) \qquad &(16 \leq j \leq 63)
    \end{cases}
\end{aligned}
$$

其中 X,Y,Z 均为 32bit。

#### 2.2.4. 置换函数


$$
\begin{aligned}
    P_{0}(X) &= X \oplus (X \lll 9) \oplus (X \lll 17) \\
    P_{1}(X) &= X \oplus (X \lll 15) \oplus (X \lll 23)
\end{aligned}
$$

### 2.3. 算法描述

#### 2.3.1. 概述

对长度为 l(l < $2^{64}$) 比特的消息 m，SM3 杂凑算法经过填充和迭代压缩，生成杂凑值，杂凑值长度为 256 比特。

#### 2.3.2. 填充

假设消息m 的长度为l 比特。首先将比特 `1` 添加到消息的末尾，再添加 k 个 `0`，k 是满足 $l + 1 + k \equiv 448 mod 512$ 的最小的非负整数。然后再添加一个64位比特串，该比特串是长度l的二进制表示。填充后的消息 m' 的比特长度为 512 的倍数。

例如：对消息01100001 01100010 01100011，其长度l=24，经填充得到比特串：

$$
\begin{array}{ccc}
    &423bit &64bit \\
    01100001 \quad 01100010 \quad 01100011 \quad 1 &\overbrace{00...00} &\underbrace{\overbrace{00...011000}} \\
    &&长度
\end{array}
$$

#### 2.3.3. 迭代压缩

##### 2.3.3.1. 消息扩展

将消息分组 $B^{(i)}$ 按以下方法扩展生成132个字W0, W1, ··· , W67, W'0, W'1, ··· , W'63，用于压缩函数 CF

$$
\begin{aligned}
    W_{j} &=
    \begin{cases}
        (B[j*4] \ll 24) \mid (B[j*4+1] \ll 16) \mid (B[j*4+2] \ll 8) \mid (B[j*4+3]) \qquad &(0 \leq j \leq 15) \\
        P_{1}(W_{j-16} \oplus W_{j-9} \oplus (W_{j-3} \lll 15)) \oplus (W_{j-13} \lll 7) \oplus W_{j-6} &(16 \leq j \leq 67)
    \end{cases} \\
    W'_{j} &= W_{j} \oplus W_{j+4} \qquad (0 \leq j \leq 63)
\end{aligned}
$$


##### 2.3.3.2. 压缩函数

令 A,B,C,D,E,F,G,H 为字寄存器,SS1,SS2,TT1,TT2 为中间变量,压缩函数 Vi+1 = CF(V(i), B(i)), 0 ≤i ≤ n−1。计算过程描述如下：

$$
ABCDEFGH = V^{(i)}
$$

$$
(0 \leq j \leq 63)
\begin{cases}
    SS_{1} &= ((A \lll 12) + E + (E_{j} \lll j)) \lll 7 \\
    SS_{2} &= SS_{1} \oplus (A \lll 12) \\
    TT_{1} &= FF_{j}(A,B,C) + D + SS_{2} + W'_{j} \\
    TT_{2} &= GG_{j}(E,F,G) + H + SS_{1} + W_{j} \\
    D &= C \\
    C &= B \lll 9 \\
    B &= A \\
    A &= TT_{1} \\
    H &= G \\
    G &= F \lll 19 \\
    F &= E \\
    E &= P_{0}(TT_2)
\end{cases}
$$

$$
V^{(i+1)} = ABCDEFGH \oplus V^{(i)}
$$

<font color=red>其中 32bit 存储为大端(big-endian)格式。</font>

##### 2.3.3.3. 迭代过程

将填充后的消息 m' 按 512 比特进行分组：m' = B(0)B(1) ··· B(n−1)，其中 n=(l+k+65)/512。

对 m' 按下列方式迭代：

$$
\begin{aligned}
    V^{(i+1)} = CF(V^{(i)},B^{(i)}) \qquad (0 \leq i \leq n-1)
\end{aligned}
$$

其中CF是压缩函数，V(0)为256比特初始值IV，B(i)为填充后的消息分组，迭代压缩的结果为V(n)。



# 参考

[^1]: [SM3 密码杂凑算法](http://www.gmbz.org.cn/main/viewfile/20180108023812835219.html)
