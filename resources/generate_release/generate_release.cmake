#if(MSVC)
#    add_compile_options("/Zc:__cplusplus")
#endif()

FetchContent_Declare(ImGui
        GIT_REPOSITORY git@github.com:ocornut/imgui.git
        GIT_TAG c6e0284ac58b3f205c95365478888f7b53b077e2 #v1.89.9
        GIT_SHALLOW TRUE
        CONFIGURE_COMMAND ""
)
FetchContent_Declare(ImGuiTextEdit
        GIT_REPOSITORY git@github.com:UE4SS-RE/ImGuiColorTextEdit.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        CONFIGURE_COMMAND ""
)
FetchContent_Declare(PolyHook_2
        GIT_REPOSITORY git@github.com:stevemk14ebr/PolyHook_2_0.git
        GIT_TAG fd2a88f09c8ae89440858fc52573656141013c7f
)
FetchContent_Declare(
        RE-UE4SS-LIB
        URL https://github.com/UE4SS-RE/RE-UE4SS/releases/download/v3.0.1/zDEV-UE4SS_v3.0.1.zip
        URL_HASH MD5=cf6e1a7c0cacec9d3455b324f4465cf1
)
# TODO: re-enable when we find compiled version that supports UE4SSProgram::find_mod_by_name
#FetchContent_Declare(
#        fmtlib
#        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#        GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21 # 11.2.0
#        GIT_SHALLOW TRUE
#)
add_compile_definitions(CMAKE_BUILD_TYPE=Game__Shipping__Win64)
add_compile_definitions(UE_BUILD_SHIPPING=1)
add_compile_definitions(UBT_COMPILED_PLATFORM=Windows)
add_compile_definitions(PLATFORM_WINDOWS=1)

FetchContent_Populate(RE-UE4SS)
FetchContent_MakeAvailable(ImGui)
FetchContent_MakeAvailable(ImGuiTextEdit)
# TODO: re-enable when we find compiled version that supports UE4SSProgram::find_mod_by_name
#FetchContent_MakeAvailable(fmtlib)
FetchContent_MakeAvailable(RE-UE4SS-LIB)

set(TMP_MSVC ${MSVC})
unset(MSVC)
FetchContent_MakeAvailable(PolyHook_2)
# re-enable 'install' and reset 'MSVC' since we're done adding PolyHook
set(MSVC ${TMP_MSVC})
unset(TMP_MSVC)

FetchContent_GetProperties(
        RE-UE4SS
        SOURCE_DIR UE4SS_SRC_DIR
        BINARY_DIR UE4SS_BIN_DIR
        POPULATED UE4SS_IS_POPULATED
)
FetchContent_GetProperties(
        ImGui
        SOURCE_DIR IMGUI_SRC_DIR
        BINARY_DIR IMGUI_BIN_DIR
        POPULATED IMGUI_IS_POPULATED
)
FetchContent_GetProperties(
        ImGuiTextEdit
        SOURCE_DIR IGTE_SRC_DIR
        BINARY_DIR IGTE_BIN_DIR
        POPULATED IGTE_IS_POPULATED
)
FetchContent_GetProperties(
        POLYHOOK2
        SOURCE_DIR PH2_SRC_DIR
        BINARY_DIR PH2_BIN_DIR
        POPULATED PH2_IS_POPULATED
)
FetchContent_GetProperties(
        RE-UE4SS-LIB
        SOURCE_DIR RUL_SRC_DIR
        BINARY_DIR RUL_BIN_DIR
        POPULATED RUL_IS_POPULATED
)
# TODO: re-enable when we find compiled version that supports UE4SSProgram::find_mod_by_name
#FetchContent_GetProperties(
#        fmtlib
#        SOURCE_DIR FMT_SRC_DIR
#        BINARY_DIR FMT_BIN_DIR
#        POPULATED FMT_IS_POPULATED
#)

find_program(BASH_EXECUTABLE NAMES bash git-bash HINTS "[HKLM/SOFTWARE/Microsoft/Windows/CurrentVersion;ProgramFilesDir]/Git/usr/bin")
message(STATUS "${BASH_EXECUTABLE}")
cmake_path(GET BASH_EXECUTABLE PARENT_PATH GIT_BASH_USR_BIN)

#TODO: When cmake 4.4 becomes more common move this ugly garbage to the ENVIRONMENT option
set(OLD_PATH "$ENV{PATH}")
set(ENV{PATH} "${GIT_BASH_USR_BIN};$ENV{PATH}")
execute_process(
        COMMAND "${BASH_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/resources/generate_release/generate_libs.sh"
        WORKING_DIRECTORY ${RUL_SRC_DIR}
        COMMAND_ECHO STDOUT
        #TODO e.g.,
        #ENVIRONMENT PATH="${GIT_BASH_USR_BIN};$ENV{PATH}"
)
set(ENV{PATH} "${OLD_PATH}")

add_library(${TARGET} SHARED src/QuickSleepWait.cpp)
add_library(RE-UE4SS-LIB SHARED IMPORTED)

target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/include/Unreal/Core)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/generated_include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/File/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/String/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/DynamicOutput/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/MProgram/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Input/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Helpers/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/LuaMadeSimple/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/LuaRaw/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Constructs/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Function/include)
target_include_directories(${TARGET} PRIVATE ${IMGUI_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${PH2_SRC_DIR}/zydis/include)
target_include_directories(${TARGET} PRIVATE ${PH2_SRC_DIR}/zydis/dependencies/zycore/include)
target_include_directories(${TARGET} PRIVATE ${IGTE_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${PH2_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/UE4SS/include)
target_include_directories(${TARGET} PRIVATE ${FMT_SRC_DIR}/include)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_include_directories(${TARGET} PUBLIC ${FMT_SRC_DIR}/include)

set_target_properties(RE-UE4SS-LIB PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_IMPLIB "${RUL_SRC_DIR}/UE4SS.lib"
#        MAP_IMPORTED_CONFIG_GAME_SHIPPING_WIN64 Release
)
target_link_libraries(${TARGET} PRIVATE PolyHook_2)
target_link_libraries(${TARGET} PRIVATE RE-UE4SS-LIB)
target_compile_definitions(${TARGET} PRIVATE IS_QSW_RELEASE=1)

message(STATUS "Linked libraries for ${TARGET}: ${my_libs}")