cmake_minimum_required(VERSION 3.10)

set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED CMAKE_ASM_NASM_COMPILER)
    if(DEFINED ENV{ASM_NASM})
        set(CMAKE_ASM_NASM_COMPILER $ENV{ASM_NASM})
    elseif(DEFINED ENV{NASMPATH}) # https://github.com/ShiftMediaProject/VSNASM
        set(CMAKE_ASM_NASM_COMPILER $ENV{NASMPATH})
    elseif(EXISTS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/nasm.exe")
        set(CMAKE_ASM_NASM_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/nasm.exe")
    # add more cases in the future (like the other visual studios)
    endif()

    if(DEFINED CMAKE_ASM_NASM_COMPILER)
        string(REPLACE "\\" "/" CMAKE_ASM_NASM_COMPILER ${CMAKE_ASM_NASM_COMPILER})
    endif()
endif()

project(ZenovaAPI CXX ASM_NASM)

enable_language(ASM_NASM)

if(MSVC)
    # ZenovaAPI can't be built as debug because it plays around with variables inside Minecraft's code
    # and debug changes the size of the variables (ex: std::string)
    set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "$ENV{ZENOVA_DATA}")
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

set(generated_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/generated")
if(NOT EXISTS ${generated_directory})
    file(MAKE_DIRECTORY ${generated_directory})
    file(TOUCH "${generated_directory}/initasm.asm")
    file(TOUCH "${generated_directory}/initcpp.cpp")
    file(TOUCH "${generated_directory}/initcpp.h")
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

file(GLOB_RECURSE SOURCE_LIST
    "src/*.cpp"
    "src/*.asm"
)

function (remove_srcs)
    file (GLOB_RECURSE to_remove ${ARGN})
    list (REMOVE_ITEM SOURCE_LIST ${to_remove})
    set (SOURCE_LIST ${SOURCE_LIST} PARENT_SCOPE)
endfunction()

list(APPEND SOURCE_LIST
    "src/ZenovaAPI.rc"
)

if(NOT WIN32)
    remove_srcs(
        "src/dllmain.cpp"
        "src/Zenova/Platform/Windows.cpp"
    )
elseif(NOT UNIX)
    remove_srcs(
        "src/Zenova/Platform/Linux.cpp"
    )
endif()

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x64-v140-mtd.lib")
    else()
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x86-v140-mtd.lib")
    endif()
endif()

add_library(ZenovaAPI SHARED
    ${API_INCLUDE_LIST}
    ${INCLUDE_LIST}
    ${SOURCE_LIST}
)

target_link_libraries(ZenovaAPI
    ${MinHook}
)

add_custom_command(TARGET ZenovaAPI
    PRE_BUILD
    COMMAND py -3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/process_headers.py -a x64 -p windows -d ${CMAKE_CURRENT_SOURCE_DIR}/src/generated ${CMAKE_CURRENT_SOURCE_DIR}/maps/windows/x64/*.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/process_headers.py
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files//" FILES ${API_INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files//" FILES ${INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files//" FILES ${SOURCE_LIST})
