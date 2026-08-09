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

    void replaceCode(LPVOID addr)
    {
        if (!addr)
        {
            state = State::FAILURE;
        }
#ifdef QSW_DEBUG
        Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Address: {}\n"), addr);
#endif
        DWORD flOldProtect;
        if (!VirtualProtect(addr,NOP_SIZE,PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            state = State::FAILURE;
        }
        memcpy(addr, newCode, NOP_SIZE);
        if (!VirtualProtect(addr, NOP_SIZE, flOldProtect, &flOldProtect))
        {
            state = State::FAILURE;
        }
        state = State::SUCCESS;
    }

    static uintptr_t findModule()
    {
        char pattern[] = OLD_CODE_PATTERN;
        char mask[] = OLD_CODE_MASK;
        uintptr_t addr = NULL;
        if (addr = Syx::FindPatternA(OBR_WIN64, pattern, mask); !addr)
        {
            addr = Syx::FindPatternA(OBR_WINGDK, pattern, mask);
        }
        return addr;
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
                    replaceCode(reinterpret_cast<LPVOID>(findModule()));
                    break;
                case State::SUCCESS:
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
#ifdef QSW_DEBUG
                    Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] State: {}"), (uint32_t)state);
#endif
                    return;
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
