add_library(${TARGET} SHARED "src/QuickSleepWait.cpp")
target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TARGET} PUBLIC UE4SS)