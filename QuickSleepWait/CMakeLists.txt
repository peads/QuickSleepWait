find_package(Git REQUIRED)
include(FetchContent)

FetchContent_Declare(
        Syx-Memory
        GIT_REPOSITORY https://github.com/SyxMem/Syx-Memory.git
        GIT_TAG 6c45caa9849f4719cc277ba4587c51a442157c0e # v1.3
)
FetchContent_MakeAvailable(Syx-Memory)
FetchContent_GetProperties(
        Syx-Memory
        SOURCE_DIR SYX_SRC_DIR
        BINARY_DIR SYX_BIN_DIR
        POPULATED SYX_IS_POPULATED
)
execute_process(
        COMMAND ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/resources/syx.patch"
        WORKING_DIRECTORY "${SYX_SRC_DIR}"
        RESULT_VARIABLE patch_result
)
add_library(${TARGET} SHARED "src/QuickSleepWait.cpp")
target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS)