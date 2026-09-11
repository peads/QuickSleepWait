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

static constexpr uint8_t newCode[] =
{
    0x0F,   // xorps xmm0,xmm0
    0x57,
    0xC0,
    0x90,   // nop
    0x90,   // nop
    0x90,   // ...
    0x90,
    0x90,
    0x90,
    0x90,
    0x90,
    0x90
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

    // ReSharper disable once CppParameterMayBeConst
    static bool replaceCode(void *addr, HMODULE module = nullptr)
    {
        if (!addr)
            return false;

        DWORD flOldProtect;
        if (!VirtualProtect(addr,NOP_SIZE,PAGE_EXECUTE_READWRITE, &flOldProtect))
            return false;

        memcpy(addr, newCode, NOP_SIZE);
        if (!VirtualProtect(addr, NOP_SIZE, flOldProtect, &flOldProtect))
            return false;

        if(module)
            FlushInstructionCache(module, addr, NOP_SIZE);

        return true;
    }

    static HMODULE findModule()
    {
        HMODULE result = nullptr;
        if (result = GetModuleHandle(OBR_WIN64); !result)
        {
            result = GetModuleHandle(OBR_WINGDK);
        }
        return result;
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
                        const HMODULE module = findModule();
                        state = static_cast<State>(replaceCode(reinterpret_cast<void*>(Syx::FindPatternA(module,
                                        OLD_CODE_PATTERN,
                                        OLD_CODE_MASK)),
                                    module));
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
