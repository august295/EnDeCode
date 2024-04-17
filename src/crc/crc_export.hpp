#ifndef CRC_EXPORT_HPP
#define CRC_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
    #ifdef CRC_EXPORT
        #define CRC_API __declspec(dllexport)
    #else
        #define CRC_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define CRC_API __attribute__((visibility("default")))
#else
    #define CRC_API
#endif

#endif
