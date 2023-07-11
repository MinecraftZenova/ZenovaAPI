cmake_minimum_required(VERSION 3.12)

set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo")
project(ZenovaAPI CXX ASM_NASM)

set(ZenovaData "$ENV{ZENOVA_DATA}")

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
endif()

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x64-v141-mtd.lib")
    else()
        set(MinHook "${CMAKE_CURRENT_SOURCE_DIR}/lib/libMinHook-x86-v141-mtd.lib")
    endif()
endif()

add_library(ZenovaAPI SHARED
    ${API_INCLUDE_LIST}
    ${INCLUDE_LIST}
    ${SOURCE_LIST}
    "src/ZenovaAPI.rc"

    # this is for VS solution explorer
    "tools/process_symbol_map.py"
)

target_link_libraries(ZenovaAPI
    ${MinHook}
)

if (NOT ZenovaData STREQUAL "")
	set_target_properties(ZenovaAPI PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "$<1:${ZenovaData}>"
		LIBRARY_OUTPUT_DIRECTORY "$<1:${ZenovaData}>"
		ARCHIVE_OUTPUT_DIRECTORY "$<1:${ZenovaData}/dev/lib>"
	)

	# this is to help test script changes through mods
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tools/process_symbol_map.py" "${ZenovaData}/dev/tools/process_symbol_map.py" COPYONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tools/demangle.py" "${ZenovaData}/dev/tools/demangle.py" COPYONLY)

	add_custom_command(TARGET ZenovaAPI
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/inc
		${ZenovaData}/dev/inc
		COMMENT "Headers copied to ${ZenovaData}/dev/inc"
	)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files//" FILES ${API_INCLUDE_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files//" FILES ${INCLUDE_LIST} ${SOURCE_LIST})

target_compile_options(ZenovaAPI 
    PRIVATE "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)
