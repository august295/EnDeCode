include_directories(${ROOT_DIR}/3rdparty/CLI11/include)
include_directories(${ROOT_DIR}/3rdparty/filesystem/include)
include_directories(${ROOT_DIR}/3rdparty/spdlog/include)

set(ProjectName "endecodeTool")
CreateTarget(${ProjectName} "Exe" "tool")

# 链接库
target_link_libraries(${ProjectName} "endecode")
