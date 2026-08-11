cmake_minimum_required(VERSION 3.29)
set(TARGET QuickSleepWait)
project(${TARGET})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Git REQUIRED)
include(FetchContent)

FetchContent_Declare(
        Syx-Memory
        GIT_REPOSITORY https://github.com/SyxMem/Syx-Memory.git
        GIT_TAG 6c45caa9849f4719cc277ba4587c51a442157c0e # v1.3
        PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/resources/syx.patch" || ${CMAKE_COMMAND} -E true
)

FetchContent_MakeAvailable(Syx-Memory)
FetchContent_GetProperties(
        Syx-Memory
        SOURCE_DIR SYX_SRC_DIR
        BINARY_DIR SYX_BIN_DIR
        POPULATED SYX_IS_POPULATED
)

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

    add_library(${TARGET} SHARED "${CMAKE_SOURCE_DIR}/src/QuickSleepWait.cpp")
    target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} PUBLIC UE4SS)
endif()