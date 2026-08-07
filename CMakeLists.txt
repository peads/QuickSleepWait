cmake_minimum_required(VERSION 3.29)
set(TARGET QuickSleepWait)
project(${TARGET})

find_package(Git REQUIRED)
include(FetchContent)

FetchContent_Declare(
        Syx-Memory
        GIT_REPOSITORY https://github.com/SyxMem/Syx-Memory.git
        GIT_TAG 6c45caa9849f4719cc277ba4587c51a442157c0e # v1.3
)
FetchContent_Declare(
        RE-UE4SS
        GIT_REPOSITORY https://github.com/UE4SS-RE/RE-UE4SS.git
#        GIT_TAG d935b5b23bac03b65c14ae38382b02007204cc2e # v3.0.1
#        GIT_TAG f12f0bedc34a0e4fdb05f36953686a13dd10641b # experimental-latest
        GIT_TAG 662df91503379fc383bc745f7ade795d7b2ca215
        GIT_CONFIG "submodule.deps/first/Unreal.url=https://github.com/Re-UE4SS/UEPseudo.git"
)

FetchContent_MakeAvailable(Syx-Memory)
FetchContent_MakeAvailable(RE-UE4SS)

FetchContent_GetProperties(
        Syx-Memory
        SOURCE_DIR SYX_SRC_DIR
        BINARY_DIR SYX_BIN_DIR
        POPULATED SYX_IS_POPULATED
)
execute_process(
        COMMAND ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/QuickSleepWait/resources/syx.patch"
        WORKING_DIRECTORY "${SYX_SRC_DIR}"
        RESULT_VARIABLE patch_result
)

add_library(${TARGET} SHARED "src/QuickSleepWait.cpp")
target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS)
