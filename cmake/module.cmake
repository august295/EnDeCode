# CreateTarget  宏名称
# ProjectName   项目名称
# Type          项目类型
# Group         项目分组
macro(CreateTarget ProjectName Type Group)
    # 项目名称
    message(STATUS ${ProjectName})
    project(${ProjectName})

    # 将当前目录下所有源码文件添加到变量
    set(CURRENT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    set(HEADER_FILES "")
    set(SOURCE_FILES "")

    # 搜集源文件
    if(NOT ${Type} STREQUAL "Exe")
        file(GLOB_RECURSE HEADER_FILES "${ROOT_DIR}/include/${UseProjectName}/*.h" "${ROOT_DIR}/include/${UseProjectName}/*.hpp")
    else()
        file(GLOB_RECURSE HEADER_FILES "${CURRENT_PATH}/*.h" "${CURRENT_PATH}/*.hpp")
    endif()
    file(GLOB_RECURSE SOURCE_FILES "${CURRENT_PATH}/*.c" "${CURRENT_PATH}/*.cpp")

    # 过滤文件
    if(NOT BUILD_VCPKG)
        list(FILTER HEADER_FILES EXCLUDE REGEX ".*(rsa|ecc).*")
        list(FILTER SOURCE_FILES EXCLUDE REGEX ".*(rsa|ecc).*")
    endif()

    # 文件分类
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(NOT ${Type} STREQUAL "Exe")
            source_group(TREE ${ROOT_DIR}/include/${UseProjectName}/ PREFIX "Header Files" FILES ${HEADER_FILES})
        else()
            source_group(TREE ${CURRENT_PATH} PREFIX "Header Files" FILES ${HEADER_FILES})
        endif()
        source_group(TREE ${CURRENT_PATH} PREFIX "Source Files" FILES ${SOURCE_FILES})
    elseif()
        source_group("Header Files" FILES ${HEADER_FILES})
        source_group("Source Files" FILES ${SOURCE_FILES})
    endif()

    # 包含头文件
    include_directories(${ROOT_DIR}/include)

    # 生成项目
    if(${Type} STREQUAL "Exe")
        # 生成可执行文件
        add_executable(${ProjectName}
            ${HEADER_FILES} ${SOURCE_FILES}
        )
        set_target_properties(${ProjectName} PROPERTIES
            DEBUG_POSTFIX "d"
            VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    else()
        # 生成链接库
        if(${Type} STREQUAL "Lib")
            add_library(${ProjectName} STATIC ${HEADER_FILES} ${SOURCE_FILES})
        elseif(${Type} STREQUAL "Dll")
            add_library(${ProjectName} SHARED ${HEADER_FILES} ${SOURCE_FILES})
        endif()
    endif()

    # 项目分组
    set_property(TARGET ${ProjectName} PROPERTY FOLDER ${Group})
endmacro()

# 添加头文件和链接库路径
macro(AddLibrary LibraryList)
    foreach(library ${LibraryList})
        # 添加 include
        include_directories(${ROOT_DIR}/3rdparty/${library})
        include_directories(${ROOT_DIR}/3rdparty/${library}/include)
    endforeach()
endmacro()
