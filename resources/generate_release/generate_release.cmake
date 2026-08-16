FetchContent_Declare(ImGui
        GIT_REPOSITORY git@github.com:ocornut/imgui.git
        GIT_TAG c6e0284ac58b3f205c95365478888f7b53b077e2 #v1.89.9
        GIT_SHALLOW TRUE
)
FetchContent_Declare(ImGuiTextEdit
        GIT_REPOSITORY git@github.com:UE4SS-RE/ImGuiColorTextEdit.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_Declare(
        RE-UE4SS-LIB
        URL https://github.com/UE4SS-RE/RE-UE4SS/releases/download/experimental-latest/zDEV-UE4SS_v3.0.1-1021-g1c1a1497.zip
        URL_HASH SHA256=497f7106e19c866f38511699ffaecc17ae2a032682066d5ed8029ac61532d517
)
FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21 # 11.2.0
        GIT_SHALLOW TRUE
)
FetchContent_Declare(
        RE-UE4SS
        GIT_REPOSITORY https://github.com/UE4SS-RE/RE-UE4SS.git
#        GIT_TAG f12f0bedc34a0e4fdb05f36953686a13dd10641b # experimental-latest
        GIT_TAG bc66bb187f095307ecf4ac56c2691ed0cd046b19 # commit of dll on Nexus
        GIT_CONFIG "submodule.deps/first/Unreal.url=https://github.com/Re-UE4SS/UEPseudo.git"
        PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/resources/DynamicOutput.patch" || ${CMAKE_COMMAND} -E true
)

FetchContent_Populate(RE-UE4SS)
FetchContent_Populate(ImGui)
FetchContent_Populate(ImGuiTextEdit)
FetchContent_Populate(fmtlib)
FetchContent_Populate(RE-UE4SS-LIB)

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
        RE-UE4SS-LIB
        SOURCE_DIR RUL_SRC_DIR
        BINARY_DIR RUL_BIN_DIR
        POPULATED RUL_IS_POPULATED
)
FetchContent_GetProperties(
        fmtlib
        SOURCE_DIR FMT_SRC_DIR
        BINARY_DIR FMT_BIN_DIR
        POPULATED FMT_IS_POPULATED
)

find_program(BASH_EXECUTABLE NAMES bash git-bash HINTS "[HKLM/SOFTWARE/Microsoft/Windows/CurrentVersion;ProgramFilesDir]/Git/usr/bin" REQUIRED)
if(NOT EXISTS "${RUL_SRC_DIR}/ue4ss/UE4SS.lib")
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
endif()

add_library(${TARGET} SHARED "${CMAKE_SOURCE_DIR}/src/QuickSleepWait.cpp")
add_library(RE-UE4SS-LIB STATIC IMPORTED)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
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
target_include_directories(${TARGET} PRIVATE ${IGTE_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/UE4SS/include)
target_include_directories(${TARGET} PRIVATE ${FMT_SRC_DIR}/include)
target_include_directories(${TARGET} PRIVATE ${SYX_SRC_DIR})

set_target_properties(RE-UE4SS-LIB PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${RUL_SRC_DIR}/ue4ss/UE4SS.lib"
        MAP_IMPORTED_CONFIG_GAME_SHIPPING_WIN64 Release
        MAP_IMPORTED_CONFIG_GAME_SHIPPING_WIN64 RelWithDebInfo
)
target_compile_definitions(RE-UE4SS-LIB INTERFACE
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_BUILD_SHIPPING=1>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_GAME=1>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_EDITOR=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_SERVER=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_BUILD_SHIPPING_WITH_EDITOR=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:UE_BUILD_DOCS=0>"
#        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:USE_LOGGING_IN_SHIPPING=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:USE_CHECKS_IN_SHIPPING=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:USE_ENSURES_IN_SHIPPING=0>"
        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:FORCE_USE_STATS=0>"
#        "$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:USE_NULL_RHI=1>"
        "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>>:UBT_COMPILED_PLATFORM=Windows>"
        "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>>:PLATFORM_WINDOWS=1>"
)
target_link_libraries(${TARGET} PRIVATE RE-UE4SS-LIB)
target_compile_definitions(${TARGET} PRIVATE IS_QSW_RELEASE="${GENERATE_RELEASE}")
if(DEFINED QSW_DEBUG)
    target_compile_definitions(${TARGET} PRIVATE IS_QSW_DEBUG="${QSW_DEBUG}")
    FetchContent_Declare(
            zydis
            GIT_REPOSITORY git@github.com:zyantific/zydis.git
            GIT_TAG a2278f1d254e492f6a6b39f6cb5d1f5d515659dc #v4.1.1
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(zydis)
    FetchContent_GetProperties(
            zydis
            SOURCE_DIR ZY_SRC_DIR
            BINARY_DIR ZY_BIN_DIR
            POPULATED ZY_IS_POPULATED
    )
    target_include_directories(${TARGET} PRIVATE ${ZY_SRC_DIR}/include)
    target_include_directories(${TARGET} PRIVATE ${ZY_SRC_DIR}/dependencies/zycore/include)
    target_link_libraries(${TARGET} PRIVATE Zydis)
endif()

cmake_path(SET DEPLOY_PATH "C:/XboxGames/The Elder Scrolls IV- Oblivion Remastered/Content/OblivionRemastered/Binaries/WinGDK/ue4ss/Mods/QuickSleepWait/dlls")
cmake_path(GET DEPLOY_PATH PARENT_PATH DEPLOY_PATH_ROOT)
cmake_path(GET DEPLOY_PATH_ROOT PARENT_PATH DEPLOY_PATH_ROOT)

add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET}>
            "${DEPLOY_PATH}/main.dll"
#        COMMAND ${CMAKE_COMMAND} -E copy
#            "$<TARGET_FILE_DIR:${TARGET}>/${TARGET}.pdb"
#            "${DEPLOY_PATH}/main.pdb"
#        COMMAND ${BASH_EXECUTABLE}
#            "${CMAKE_SOURCE_DIR}/resources/generate_release/package_release.sh" "${DEPLOY_PATH_ROOT}"
)
