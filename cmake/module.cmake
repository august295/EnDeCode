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
    file(GLOB_RECURSE HEADER_FILES "${CURRENT_PATH}/*.h" "${CURRENT_PATH}/*.hpp")
    file(GLOB_RECURSE SOURCE_FILES "${CURRENT_PATH}/*.c" "${CURRENT_PATH}/*.cpp")

    # 文件分类
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        source_group(TREE ${CURRENT_PATH} PREFIX "Header Files" FILES ${HEADER_FILES})
        source_group(TREE ${CURRENT_PATH} PREFIX "Source Files" FILES ${SOURCE_FILES})
    elseif()
        source_group("Header Files" FILES ${HEADER_FILES})
        source_group("Source Files" FILES ${SOURCE_FILES})
    endif()

    # 包含头文件
    include_directories(${ROOT_DIR}/src)

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

    # 拷贝头文件
    if(NOT(${Group} STREQUAL "test"))
        set(TargetInclude "${ROOT_DIR}/include/${ProjectName}/")
        file(MAKE_DIRECTORY ${TargetInclude})
        add_custom_command(TARGET ${ProjectName} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${HEADER_FILES}
            ${TargetInclude})
    endif()
endmacro()
