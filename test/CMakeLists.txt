include_directories(${ROOT_DIR}/3rdparty/googletest/googletest/include)
include_directories(${ROOT_DIR}/3rdparty/googletest/googlemock/include)

set(ProjectName "test")
CreateTarget(${ProjectName} "Exe" "test")

# 链接库
target_link_libraries(${ProjectName} "gtest" "gtest_main")
target_link_libraries(${ProjectName} "endecode")