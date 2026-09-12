cmake_minimum_required(VERSION 3.29)
set(TARGET QuickSleepWait)
project(${TARGET})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if (NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MSVC)
        add_compile_options(/clang:-m64 /clang:-O3 /clang:-march=x86-64-v3 /WX- /Gy /clang:-flto /clang:-fomit-frame-pointer)
    elseif (MSVC)
        add_compile_options(/O2 /Ox /Ob2 /Ot /Oy /Oi /WX- /GL /Gy /Gm- /MP)
    endif ()
    add_link_options(/OPT:REF /OPT:ICF=3 /LTCG /DEBUG:NONE /EMITPOGOPHASEINFO)
    add_compile_definitions(OPTIMIZATIONS_ON)
endif()

find_package(Git REQUIRED)
include(FetchContent)

FetchContent_Declare(
        PMO
        GIT_REPOSITORY https://github.com/peads/pmo.git
        GIT_TAG v1.0.1
)

FetchContent_MakeAvailable(PMO)
FetchContent_GetProperties(
        PMO
        SOURCE_DIR PMO_SRC_DIR
        BINARY_DIR PMO_BIN_DIR
        POPULATED PMO_IS_POPULATED
)

add_library(${TARGET} SHARED "${CMAKE_SOURCE_DIR}/src/QuickSleepWait.cpp")
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${TARGET} PRIVATE ${PMO_SRC_DIR}/include)

if(DEFINED GENERATE_RELEASE)
    include(resources/generate_release/generate_release.cmake)
else()
    FetchContent_Declare(
            RE-UE4SS
            GIT_REPOSITORY https://github.com/UE4SS-RE/RE-UE4SS.git
            GIT_TAG main
            GIT_CONFIG "submodule.deps/first/Unreal.url=https://github.com/Re-UE4SS/UEPseudo.git"
    )
    FetchContent_MakeAvailable(RE-UE4SS)
    FetchContent_GetProperties(
            RE-UE4SS
            SOURCE_DIR UE4SS_SRC_DIR
            BINARY_DIR UE4SS_BIN_DIR
            POPULATED UE4SS_IS_POP
    )
    target_link_libraries(${TARGET} PUBLIC UE4SS)
endif()