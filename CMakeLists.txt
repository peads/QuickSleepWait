cmake_minimum_required(VERSION 3.18)
find_package(Git REQUIRED)

project(QuickSleepWait)

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/syx.git.patch"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Syx-Memory"
    RESULT_VARIABLE patch_result
)

if(NOT patch_result EQUAL 0)
    message(WARNING "Failed to apply patch file. Has it already been applied?")
endif()

add_subdirectory(RE-UE4SS)
add_subdirectory(QuickSleepWait)

