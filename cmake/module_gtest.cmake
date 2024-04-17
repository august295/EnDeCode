add_subdirectory(3rdparty/googletest)
include_directories(3rdparty/googletest/googletest/include)
include_directories(3rdparty/googletest/googlemock/include)

set_property(TARGET gtest PROPERTY FOLDER googletest)
set_property(TARGET gtest_main PROPERTY FOLDER googletest)
set_property(TARGET gmock PROPERTY FOLDER googletest)
set_property(TARGET gmock_main PROPERTY FOLDER googletest)
