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
#include <Mod/CppUserModBase.hpp>
#include <Syx/Syx.h>

//#define QSW_DEBUG 1
#define NOP_SIZE 12
// Pattern in WinGDK 1.512:
// "F3 0F 10 35 F4 9B CA 01 F3 0F 58 C6 F3 0F 11 05"
// Generic pattern w/wildcards for the specific DWORD addr in mem
// "F3 0F 10 35 ?  ?  ?  ?  F3 0F 58 C6 F3 0F 11 05"
// With this loop condition found, we will replace it with code
// that will force the loop state to its break condition (i.e. 0 in this case).
// Hence, it becomes 0F 75 C0 90 ... 90 with the NOPs to fill the remaining space.
#define OLD_CODE_PATTERN "\xF3\x0F\x10\x35\xF4\x9B\xCA\x01\xF3\x0F\x58\xC6\xF3\x0F\x11\x05"
#define OLD_CODE_MASK "xxxx????xxxxxxxx"
#define OBR_WIN64 "OblivionRemastered-Win64-Shipping.exe"
#define OBR_WINGDK "OblivionRemastered-WinGDK-Shipping.exe"
#define LOBR_WIN64 L##"OblivionRemastered-Win64-Shipping.exe"
#define LOBR_WINGDK L##"OblivionRemastered-WinGDK-Shipping.exe"

using namespace RC;

enum class State
{
    CONSTRUCTED,
    UNREAL_READY,
    SUCCESS,
    FAILED,
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

    void replaceCode(void *addr)
    {
        if (!addr) 
        {
            state = State::FAILED;
        }
#ifdef QSW_DEBUG
        Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Address: {}\n"), addr);
#endif
        DWORD flOldProtect;
        if (!VirtualProtect(addr,NOP_SIZE,PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            state = State::FAILED;
        }
        memcpy(addr, newCode, NOP_SIZE);
        if (!VirtualProtect(addr, NOP_SIZE, flOldProtect, &flOldProtect))
        {
            state = State::FAILED;
        }
        state = State::SUCCESS;
    }

    static uintptr_t findModule()
    {
        char pattern[] = OLD_CODE_PATTERN;
        char mask[] = OLD_CODE_MASK;

        // TODO figure out why FindPattern barfs when module not found
        if (GetModuleHandleA(OBR_WIN64))
        {
            return Syx::FindPatternA(LOBR_WIN64, pattern, mask);
        }

        if (GetModuleHandleA(OBR_WINGDK))
        {
            return Syx::FindPatternA(LOBR_WINGDK, pattern, mask);
        }
        Output::send<LogLevel::Error>(STR("[QuickSleepWait] Code address not found\n"));
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
            if (State::UNREAL_READY != state)
            {
                return;
            }
            replaceCode((void*)findModule());
#ifdef QSW_DEBUG
            switch (state) {
                case State::SUCCESS:
                    Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait succeeded\n"));
                    break;
                default:
                    Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait failed\n"));
                    break;
            }
#endif
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
