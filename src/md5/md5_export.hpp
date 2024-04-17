#ifndef MD5_EXPORT_HPP
#define MD5_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef MD5_EXPORT
        #define MD5_API __declspec(dllexport)
    #else
        #define MD5_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define MD5_API __attribute__((visibility("default")))
#else
    #define MD5_API
#endif

#endif
