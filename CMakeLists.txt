cmake_minimum_required(VERSION 3.10)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

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
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

add_compile_definitions(ZENOVA_API)

include_directories("src")

file(GLOB_RECURSE INCLUDE_LIST
    "src/*.h"
)

set(SOURCE_LIST 
    "src/Zenova.cpp"
    "src/Zenova/Common.cpp"
    "src/Zenova/Hook.cpp"
    "src/Zenova/Log.cpp"
    "src/Zenova/Mod.cpp"
    "src/Zenova/StorageResolver.cpp"
    "src/Zenova/Platform/Platform.cpp"
)

if(WIN32)
    list(APPEND SOURCE_LIST
        "src/dllmain.cpp"
        "src/Zenova/Platform/Windows.cpp"
    )
endif()

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