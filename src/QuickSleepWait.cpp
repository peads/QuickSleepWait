/*
 * This file is part of the QuickSleepWait distribution
 * (https://github.com/peads/QuickSleepWait).
 * Copyright (c) 2026 Patrick Eads.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "QuickSleepWait.h"
using namespace RC;

enum class State
{
    CONSTRUCTED,
    UNREAL_READY,
    WORKING,
    SUCCESS,
    FAILURE,
    DESTROYING,
    DESTROYED
};

/* Brief overview of what's happening */
// We observe this set of instructions in a disassembler (e.g. x64dbg, IDA, etc.):
// "F3 0F 10 35 F4 9B CA 01 F3 0F 58 C6 F3 0F 11 05"
// We manually generate a generic pattern replacing dereferenced addresses
// (e.g. computed/displacement/rip-relative/scaled-index addresses) with wildcards as needed:
// "F3 0F 10 35 ?? ?? ?? ?? F3 0F 58 C6 F3 0F 11 05"
// This is a branching loop condition that forces the executing thread to pause for some
// human-perceivable amount of time while counting down to zero. The pattern is used find the
// address of the code to be replaced at run-time.
static inline constexpr const char *const oldPattern = "\xF3\x0F\x10\x35\xF4\x9B\xCA\x01\xF3\x0F\x58\xC6\xF3\x0F\x11\x05";
static inline constexpr const char *const oldMask = "xxxx????xxxxxxxx";
/* Brief overview of what's happening (continued) */
// Once the location is determined, the code in memory is replaced with some that forces it to
// break the loop (i.e. we zero out the xmm0 register and fill the remaining space with NOPs).
// Hence, it becomes 0F 57 C0 90 ... 90, or `xorps xmm0,xmm0; nop; ... nop;`
static inline constexpr const char *const newCode = "\x0F\x57\xC0\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";

class QuickSleepWait final : public CppUserModBase
{
    State state;

    /**
     * @brief Replaces the extant code at given memory address with code bytes at the given pointer.
     * @details Guards against null pointers for addr, code and module. As a side effect,
     *          it sets this instance's state based on success (i.e. success only if all steps
     *          complete. Otherwise, failure). memcpy is used for speed. Thus, addr and code
     *          pointers must not contain overlapping memory regions, or the behavior is undefined.
     * @param addr Memory address where the code to-be-replaced starts. May not be NULL.
     * @param code Pointer to the contiguous bytes (e.g. an array) to be written to
     *             addr. May not be NULL.
     * @param size Number of bytes to be written.
     * @param module Module containing addr.
     */
    void replaceCode(LPVOID addr, const void *code, size_t size, const HMODULE &module)
    {
#ifdef IS_QSW_DEBUG
        QSW::Debug::debug(reinterpret_cast<uintptr_t>(addr));
#endif
        state = State::FAILURE;
        if (!addr || !module || !code)
        {
            return;
        }

        DWORD flOldProtect;
        if (!VirtualProtect(addr, size,PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            Output::send<
                LogLevel::Warning>(STR("[QuickSleepWait] Failed to set memory attributes.\n"));
            return;
        }
        memcpy(addr, code, size);
        if (!VirtualProtect(addr, size, flOldProtect, &flOldProtect))
        {
            Output::send<
                LogLevel::Warning>(STR("[QuickSleepWait] Failed to reset memory attributes.\n"));
            return;
        }
        if (!FlushInstructionCache(module, addr, size))
        {
            Output::send<LogLevel::Warning>(STR("[QuickSleepWait] Failed to flush cache.\n"));
            return;
        }
#ifdef IS_QSW_DEBUG
        QSW::Debug::debug(reinterpret_cast<uintptr_t>(addr));
#endif
        state = State::SUCCESS;
    }

    /**
     * @brief Searches for OBR instance in memory and the address of the given pattern in that
     *        module.
     * @details Searches for either of the two known names for OBR in succession. Returns
     *          starting address of pattern, and stores module in out parameter if successful.
     *          Otherwise, returns NULL. See documentation of SyxLib for further details on its
     *          usage here. N.B. The implementation used here is modified using the patch provided
     *          in the `resources` directory.
     * @param pattern Pointer to sequence of bytes to be found in memory. May not be NULL.
     * @param mask Pointer to pattern mask indicating required key values and wildcards.
     *             May not be NULL.
     * @param outModule Reference to out-value storing the module in which the return address was
     *                  found, if applicable.
     * @return Pointer to the starting address where the pattern was found, or NULL if not found.
     */
    static uintptr_t findModule(const char *pattern, const char *mask, HMODULE &outModule)
    {
        if (outModule = GetModuleHandle(OBR_WIN64); outModule)
        {
            return Syx::FindPatternAB(outModule, pattern, mask);
        }

        if (outModule = GetModuleHandle(OBR_WINGDK); outModule)
        {
            return Syx::FindPatternAB(outModule, pattern, mask);
        }
        Output::send<LogLevel::Warning>(STR("[QuickSleepWait] Failed to find OBR in memory.\n"));
        return NULL;
    }

    public:
        QuickSleepWait()
        {
            state = State::CONSTRUCTED;

            ModName = STR("QuickSleepWait");
            ModVersion = STR("1.0");
            ModDescription = STR("Makes waiting faster and not suck.");
            ModAuthors = STR("Patrick Eads - https://github.com/peads/QuickSleepWait");
        }

        ~QuickSleepWait() override
        {
            state = State::DESTROYED;
        }

        auto on_update()->void override
        {
            switch (state)
            {
                case State::UNREAL_READY:
                    state = State::WORKING;
                    {
                        HMODULE module = nullptr;
                        replaceCode(reinterpret_cast<LPVOID>(
                                        findModule(oldPattern, oldMask, module)),
                                    newCode,
                                    SIZE,
                                    module);
                    }
                case State::SUCCESS:
                    Output::send<LogLevel::Default>(STR("[QuickSleepWait] Success\n"));

                case State::FAILURE:
                {
#ifdef IS_QSW_RELEASE
                    state = State::DESTROYED;
#else
                    state = State::DESTROYING;
                    CppMod *thisMod = UE4SSProgram::find_mod_by_name<CppMod>(ModName,
                             UE4SSProgram::IsInstalled::Yes,
                             UE4SSProgram::IsStarted::Yes);
                    if (thisMod)
                    {
                        thisMod->uninstall();
                    }
#endif
                }
                break;
                default:
                    return;
            }
        }

        auto on_unreal_init()->void override
        {
            state = State::UNREAL_READY;
        }
};

#define QUICK_SLEEP_WAIT_API __declspec(dllexport)

extern "C" {
    QUICK_SLEEP_WAIT_API CppUserModBase *start_mod()
    {
        return new QuickSleepWait();
    }

    QUICK_SLEEP_WAIT_API void uninstall_mod(const CppUserModBase *mod)
    {
        delete mod;
    }
}
