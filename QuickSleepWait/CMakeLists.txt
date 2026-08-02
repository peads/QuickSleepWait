cmake_minimum_required(VERSION 3.18)

set(TARGET QuickSleepWait)
project(${TARGET})

add_library(${TARGET} SHARED "src/dllmain.cpp")
include_directories("../Syx-Memory")
#target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS)