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
    FAILURE,
    SUCCESS,
    CONSTRUCTED,
    UNREAL_READY,
    WORKING,
    DESTROYING,
    DESTROYED
};

class QuickSleepWait final : public CppUserModBase
{
    State state;

    void cleanUp()
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

    // static bool replaceCode(void *addr, HMODULE module = nullptr)
    // {
    //     if (!addr)
    //         return false;
    //
    //     DWORD flOldProtect;
    //     if (!VirtualProtect(addr,NOP_SIZE,PAGE_EXECUTE_READWRITE, &flOldProtect))
    //         return false;
    //
    //     memcpy(addr, newCode, NOP_SIZE);
    //     if (!VirtualProtect(addr, NOP_SIZE, flOldProtect, &flOldProtect))
    //         return false;
    //
    //     if(module)
    //         FlushInstructionCache(module, addr, NOP_SIZE);
    //
    //     return true;
    // }

    // static HMODULE findModule()
    // {
    //     HMODULE result = nullptr;
    //     if (result = GetModuleHandle(OBR_WIN64); !result)
    //     {
    //         result = GetModuleHandle(OBR_WINGDK);
    //     }
    //     return result;
    // }

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
#ifdef QSW_DEBUG
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] destroyed\n"));
#endif
            state = State::DESTROYED;
        }

        auto on_update()->void override
        {
            switch (state)
            {
                case State::UNREAL_READY:
                    state = State::WORKING;
                    {
                        static const char *names[] = {OBR_WIN64, OBR_WINGDK};
                        static PMO::Pattern pattern(SLEEP_WAIT_PATTERN,
                                                    SLEEP_WAIT_MASK,
                                                    SLEEP_WAIT_CODE);
                        static const HMODULE module = PMO::findModule(names); // TODO find where the ue4ss console iostream is hidden
                        MODULEINFO info{};

                        bool result = module && PMO::getImportInfo(module, info);
                        result = result && findPatterns(reinterpret_cast<uintptr_t>(info.lpBaseOfDll), info.SizeOfImage,pattern);
                        result = result && replaceCode(pattern.back(), pattern.code.str, pattern.codeLen);

                        state = static_cast<State>(result);
                    }
                    break;
                case State::SUCCESS:
                    Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Success\n"));
                    cleanUp();
                    break;
                case State::FAILURE:
                    Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Failure\n"));
                    cleanUp();
                    break;
                default:
                    break;
            }
        }

        auto on_unreal_init()->void override
        {
#ifdef QSW_DEBUG
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait Unreal namespace ready\n"));
#endif
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
