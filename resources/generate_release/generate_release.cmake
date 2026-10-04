set(UE4SS_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/re-ue4ss-src")
set(UE4SS_BIN_DIR "${CMAKE_BINARY_DIR}/_deps/re-ue4ss-build")
set(UE4SS_SUB_DIR "${CMAKE_BINARY_DIR}/_deps/re-ue4ss-subbuild")

set(IMGUI_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-src")
set(IMGUI_BIN_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-build")
set(IMGUI_SUB_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-subbuild")

set(IGTE_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/imguitextedit-src")
set(IGTE_BIN_DIR "${CMAKE_BINARY_DIR}/_deps/imguitextedit-build")
set(IGTE_SUB_DIR "${CMAKE_BINARY_DIR}/_deps/imguitextedit-subbuild")

set(PH2_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/polyhook_2-src")
set(PH2_BIN_DIR "${CMAKE_BINARY_DIR}/_deps/polyhook_2-build")
set(PH2_SUB_DIR "${CMAKE_BINARY_DIR}/_deps/polyhook_2-subbuild")

set(FMT_SRC_DIR "${CMAKE_BINARY_DIR}/_deps/fmtlib-src")
set(FMT_BIN_DIR "${CMAKE_BINARY_DIR}/_deps/fmtlib-build")
set(FMT_SUB_DIR "${CMAKE_BINARY_DIR}/_deps/fmtlib-subbuild")

FetchContent_Populate(
        RE-UE4SS
        QUIET
        GIT_REPOSITORY https://github.com/UE4SS-RE/RE-UE4SS.git
        GIT_TAG bc66bb187f095307ecf4ac56c2691ed0cd046b19 # commit of dll on Nexus
        GIT_CONFIG "submodule.deps/first/Unreal.url=https://github.com/Re-UE4SS/UEPseudo.git"
        PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/resources/re4uess.patch" || ${CMAKE_COMMAND} -E true
        PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/resources/DynamicOutput.patch" || ${CMAKE_COMMAND} -E true
        SOURCE_DIR "${UE4SS_SRC_DIR}"
        BINARY_DIR "${UE4SS_BIN_DIR}"
        SUBBUILD_DIR "${UE4SS_SUB_DIR}"
)
FetchContent_Populate(
        ImGui
        QUIET
        GIT_REPOSITORY git@github.com:ocornut/imgui.git
        GIT_TAG c6e0284ac58b3f205c95365478888f7b53b077e2 #v1.89.9
        GIT_SHALLOW TRUE
        SOURCE_DIR "${IMGUI_SRC_DIR}"
        BINARY_DIR "${IMGUI_BIN_DIR}"
        SUBBUILD_DIR "${IMGUI_SUB_DIR}"
)
FetchContent_Populate(
        ImGuiTextEdit
        QUIET
        GIT_REPOSITORY git@github.com:UE4SS-RE/ImGuiColorTextEdit.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        SOURCE_DIR "${IGTE_SRC_DIR}"
        BINARY_DIR "${IGTE_BIN_DIR}"
        SUBBUILD_DIR "${IGTE_SUB_DIR}"
)
FetchContent_Populate(
        POLYHOOK2
        QUIET
        GIT_REPOSITORY git@github.com:stevemk14ebr/PolyHook_2_0.git
        GIT_TAG fd2a88f09c8ae89440858fc52573656141013c7f
        SOURCE_DIR "${PH2_SRC_DIR}"
        BINARY_DIR "${PH2_BIN_DIR}"
        SUBBUILD_DIR "${PH2_SUB_DIR}"
)
FetchContent_Populate(
        fmtlib
        QUIET
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21 # 11.2.0
        GIT_SHALLOW TRUE
        SOURCE_DIR "${FMT_SRC_DIR}"
        BINARY_DIR "${FMT_BIN_DIR}"
        SUBBUILD_DIR "${FMT_SUB_DIR}"
)
cmake_path(SET RUL_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/re-ue4ss-lib-src")
if(NOT EXISTS "${RUL_SRC_DIR}/UE4SS.lib")
    find_program(BASH_EXECUTABLE NAMES bash git-bash HINTS "[HKLM/SOFTWARE/Microsoft/Windows/CurrentVersion;ProgramFilesDir]/Git/usr/bin" REQUIRED)
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

add_library(RE-UE4SS-LIB STATIC IMPORTED)

target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/include/Unreal/Core)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Unreal/generated_include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/File/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/String/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/DynamicOutput/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Input/include)
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/deps/first/Helpers/include)
target_include_directories(${TARGET} PRIVATE ${IMGUI_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${IGTE_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${PH2_SRC_DIR})
target_include_directories(${TARGET} PRIVATE ${UE4SS_SRC_DIR}/UE4SS/include)
target_include_directories(${TARGET} PRIVATE ${FMT_SRC_DIR}/include)

set_target_properties(RE-UE4SS-LIB PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${RUL_SRC_DIR}/UE4SS.lib"
        MAP_IMPORTED_CONFIG_GAME_SHIPPING_WIN64 $<CONFIG>
)
set(COMPILE_DEFNS
        UE_BUILD_SHIPPING=1
        UE_GAME=1
        UE_EDITOR=0
        UE_SERVER=0
        UE_BUILD_SHIPPING_WITH_EDITOR=0
        UE_BUILD_DOCS=0
        USE_LOGGING_IN_SHIPPING=0
        USE_CHECKS_IN_SHIPPING=0
        USE_ENSURES_IN_SHIPPING=0
        FORCE_USE_STATS=0
        USE_NULL_RHI=1
        UBT_COMPILED_PLATFORM=$<PLATFORM_ID>
        PLATFORM_WINDOWS=1
        OPTIMIZATIONS_ON
)
target_compile_definitions(RE-UE4SS-LIB INTERFACE ${COMPILE_DEFNS})
target_compile_definitions(${TARGET} INTERFACE ${COMPILE_DEFNS})

target_link_libraries(${TARGET} PRIVATE RE-UE4SS-LIB)
target_compile_definitions(${TARGET} PRIVATE IS_QSW_RELEASE="${GENERATE_RELEASE}")
cmake_path(SET DEPLOY_PATH "C:/XboxGames/The Elder Scrolls IV- Oblivion Remastered/Content/OblivionRemastered/Binaries/WinGDK/ue4ss/Mods/QuickSleepWait/dlls")
cmake_path(GET DEPLOY_PATH PARENT_PATH DEPLOY_PATH_ROOT)
cmake_path(GET DEPLOY_PATH_ROOT PARENT_PATH DEPLOY_PATH_ROOT)

add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET}>
            "${DEPLOY_PATH}/main.dll"
)
if (PACKAGE_RELEASE)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${BASH_EXECUTABLE}
            "${CMAKE_SOURCE_DIR}/resources/generate_release/package_release.sh" "${DEPLOY_PATH_ROOT}"
    )
endif ()
