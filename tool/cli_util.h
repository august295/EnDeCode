#pragma once

#include <string>
#include <fstream>
#include <sstream>

inline std::string ReadFile(const std::string& filename)
{
    std::ifstream      file(filename, std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

    return content;
}
