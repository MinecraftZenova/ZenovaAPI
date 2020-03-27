cmake_minimum_required(VERSION 3.10)

#project(ZenovaAPI)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x64-v140-mtd.lib")
    else()
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x86-v140-mtd.lib")
    endif()
endif()

if(MSVC)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{APPDATA}/Zenova/")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{APPDATA}/Zenova/")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{APPDATA}/Zenova/")
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

add_compile_definitions(ZENOVA_API)

include_directories("src")

file(GLOB_RECURSE INCLUDE_LIST
    "*.h"
)

file(GLOB_RECURSE SOURCE_LIST
    "*.cpp"
)

add_library(ZenovaAPI SHARED
    ${INCLUDE_LIST}
    ${SOURCE_LIST}
)

target_link_libraries(ZenovaAPI 
    Common
    ${MinHook}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files\\" FILES ${INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files\\" FILES ${SOURCE_LIST})