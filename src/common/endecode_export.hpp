#ifndef ENDECODE_EXPORT_HPP
#define ENDECODE_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef ENDECODE_EXPORT
        #define ENDECODE_API __declspec(dllexport)
    #else
        #define ENDECODE_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define ENDECODE_API __attribute__((visibility("default")))
#else
    #define ENDECODE_API
#endif

#endif
