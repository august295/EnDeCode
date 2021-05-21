#include "base64.h"

#define BASE64_PAD     '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST  'z'

/* BASE 64 encode table */
static const char base64en[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
    /*nul, soh, stx,  etx,  eot,  enq,  ack,  bel, */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*bs,  ht,  nl,   vt,   np,   cr,   so,   si,  */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*dle, dc1, dc2,  dc3,  dc4,  nak,  syn,  etb, */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*can, em,  sub,  esc,  fs,   gs,   rs,   us,  */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*sp,  '!', '"',  '#',  '$',  '%',  '&',  ''', */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*'(', ')', '*',  '+',  ',',  '-',  '.',  '/', */
    0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    /*'0', '1', '2',  '3',  '4',  '5',  '6',  '7', */
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
    /*'8', '9', ':',  ';',  '<',  '=',  '>',  '?', */
    0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*'@', 'A', 'B',  'C',  'D',  'E',  'F',  'G', */
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    /*'H', 'I', 'J',  'K',  'L',  'M',  'N',  'O', */
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    /*'P', 'Q', 'R',  'S',  'T',  'U',  'V',  'W', */
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    /*'X', 'Y', 'Z',  '[',  '\',  ']',  '^',  '_', */
    0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*'`', 'a', 'b',  'c',  'd',  'e',  'f',  'g', */
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    /*'h', 'i', 'j',  'k',  'l',  'm',  'n',  'o', */
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    /*'p', 'q', 'r',  's',  't',  'u',  'v',  'w', */
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    /*'x', 'y', 'z',  '{',  '|',  '}',  '~',  del, */
    0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out) {
    int           s;
    unsigned int  i;
    unsigned int  j;
    unsigned char c;
    unsigned char l;

    s = 0;
    l = 0;
    for (i = j = 0; i < inlen; i++) {
        c = in[i];

        switch (s) {
        case 0:
            s        = 1;
            out[j++] = base64en[(c >> 2) & 0x3F];
            break;
        case 1:
            s        = 2;
            out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
            break;
        case 2:
            s        = 0;
            out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
            out[j++] = base64en[c & 0x3F];
            break;
        }
        l = c;
    }

    switch (s) {
    case 1:
        out[j++] = base64en[(l & 0x3) << 4];
        out[j++] = BASE64_PAD;
        out[j++] = BASE64_PAD;
        break;
    case 2:
        out[j++] = base64en[(l & 0xF) << 2];
        out[j++] = BASE64_PAD;
        break;
    }

    out[j] = 0;

    return j;
}

unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out) {
    unsigned int  i;
    unsigned int  j;
    unsigned char c;

    if (inlen & 0x3) {
        return 0;
    }

    for (i = j = 0; i < inlen; i++) {
        if (in[i] == BASE64_PAD) {
            break;
        }
        if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST) {
            return 0;
        }

        c = base64de[(unsigned char)in[i]];
        if (c == 0xFF) {
            return 0;
        }

        switch (i & 0x3) {
        case 0:
            out[j] = (c << 2) & 0xFF;
            break;
        case 1:
            out[j++] |= (c >> 4) & 0x3;
            out[j] = (c & 0xF) << 4;
            break;
        case 2:
            out[j++] |= (c >> 2) & 0xF;
            out[j] = (c & 0x3) << 6;
            break;
        case 3:
            out[j++] |= c;
            break;
        }
    }

    return j;
}