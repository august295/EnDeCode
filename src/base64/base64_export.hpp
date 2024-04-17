#ifndef BASE64_EXPORT_HPP
#define BASE64_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef BASE64_EXPORT
        #define BASE64_API __declspec(dllexport)
    #else
        #define BASE64_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define BASE64_API __attribute__((visibility("default")))
#else
    #define BASE64_API
#endif

#endif
