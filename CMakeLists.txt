cmake_minimum_required(VERSION 3.10)

set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)

project(ZenovaAPI CXX ASM_NASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(EXISTS "${PROJECT_SOURCE_DIR}/Common")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Common")
endif()

if(NOT DEFINED CMAKE_ASM_NASM_COMPILER)
    if(DEFINED ENV{ASM_NASM})
        set(CMAKE_ASM_NASM_COMPILER $ENV{ASM_NASM})
    elseif(DEFINED ENV{NASMPATH}) # https://github.com/ShiftMediaProject/VSNASM
        set(CMAKE_ASM_NASM_COMPILER $ENV{ASM_NASM})
    elseif(EXISTS "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\nasm.exe")
        set(CMAKE_ASM_NASM_COMPILER "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\nasm.exe")
    endif()
endif()

enable_language(ASM_NASM)

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

include_directories("inc")
include_directories("src")

file(GLOB_RECURSE API_INCLUDE_LIST
    "inc/*.h"
)

file(GLOB_RECURSE INCLUDE_LIST
    "src/*.h"
)

set(SOURCE_LIST 
    "src/initasm.asm"
    "src/Zenova/Common.cpp"
    "src/Zenova/Helper.cpp"
    "src/Zenova/Hook.cpp"
    "src/Zenova/JsonHelper.cpp"
    "src/Zenova/Log.cpp"
    "src/Zenova/MessageRedirection.cpp"
    "src/Zenova/Mod.cpp"
    "src/Zenova/PackManager.cpp"
    "src/Zenova/StorageResolver.cpp"
    "src/Zenova/Platform/Platform.cpp"
    "src/Zenova/Profile/Manager.cpp"
    "src/Zenova/Profile/ModInfo.cpp"
    "src/Zenova/Profile/ProfileInfo.cpp"
)

if(WIN32)
    list(APPEND SOURCE_LIST
        "src/dllmain.cpp"
        "src/Zenova/Platform/Windows.cpp"
    )
endif()

add_library(ZenovaAPI SHARED
    ${API_INCLUDE_LIST}
    ${INCLUDE_LIST}
    ${SOURCE_LIST}
)

target_link_libraries(ZenovaAPI 
    Common
    ${MinHook}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files\\" FILES ${API_INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files\\" FILES ${INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files\\" FILES ${SOURCE_LIST})