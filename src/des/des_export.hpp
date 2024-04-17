#ifndef DES_EXPORT_HPP
#define DES_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef DES_EXPORT
        #define DES_API __declspec(dllexport)
    #else
        #define DES_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define DES_API __attribute__((visibility("default")))
#else
    #define DES_API
#endif

#endif
