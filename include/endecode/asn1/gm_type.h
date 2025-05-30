#ifndef __GM_TYPE_H__
#define __GM_TYPE_H__

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
// SKF
typedef int8_t   INT8;
typedef int16_t  INT16;
typedef int32_t  INT32;
typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

typedef UINT32 BOOL;
typedef UINT8  BYTE;
typedef char   CHAR;
typedef INT16  SHORT;
typedef UINT16 USHORT;
typedef INT32  LONG;
typedef UINT32 ULONG;
typedef UINT32 UINT;
typedef UINT16 WORD;
typedef UINT32 DWORD;
typedef UINT32 FLAGS;
typedef CHAR*  LPSTR;
// SOF
typedef CHAR* BSTR;
#endif

#endif