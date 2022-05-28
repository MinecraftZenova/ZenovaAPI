cmake_minimum_required(VERSION 3.12)

set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(ZenovaAPI CXX ASM_NASM)

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

set(MAPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/maps")

file(GLOB_RECURSE SYMMAPS
    "${MAPS_DIR}/*.json"
)

add_library(ZenovaAPI SHARED
    ${API_INCLUDE_LIST}
    ${INCLUDE_LIST}
    ${SOURCE_LIST}
    "src/ZenovaAPI.rc"

    # this is for VS solution explorer
    "tools/process_symbol_map.py"
    ${SYMMAPS}
)

target_link_libraries(ZenovaAPI
    ${MinHook}
)

# todo: handle other platforms/archs
set(SYM_PLATFORM windows)
set(SYM_ARCH x64)
set(SYMMAPS_DIR "${MAPS_DIR}/${SYM_PLATFORM}-${SYM_ARCH}")

add_custom_command(
    OUTPUT "${generated_directory}/initcpp.cpp" "${generated_directory}/initasm.asm" "${generated_directory}/initcpp.h"
    COMMAND py -3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/process_symbol_map.py -a ${SYM_ARCH} -p ${SYM_PLATFORM} -d ${generated_directory} ${SYMMAPS_DIR}/*.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/process_symbol_map.py ${SYMMAPS_DIR}/*.json
)

# this is to help test script changes through mods
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tools/process_symbol_map.py" "$ENV{ZENOVA_DATA}/dev/tools/process_symbol_map.py" COPYONLY)

source_group(TREE ${MAPS_DIR} PREFIX "Symbol Maps//" FILES ${SYMMAPS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files//" FILES ${API_INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files//" FILES ${INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files//" FILES ${SOURCE_LIST})
