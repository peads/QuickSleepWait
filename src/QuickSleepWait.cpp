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
#include <Mod/CppUserModBase.hpp>
#ifndef IS_QSW_RELEASE
#include <UE4SSProgram.hpp>
#endif
#include <windows/MemoryOps.hpp>

using namespace RC;

class QuickSleepWait final : public CppUserModBase
{
    bool state = false;

    void cleanUp()
    {
#ifndef IS_QSW_RELEASE
        CppMod *thisMod = UE4SSProgram::find_mod_by_name<CppMod>(ModName,
                 UE4SSProgram::IsInstalled::Yes,
                 UE4SSProgram::IsStarted::Yes);
        if (thisMod)
        {
            thisMod->uninstall();
        }
#endif
    }

    public:
        explicit QuickSleepWait(const bool state) : state(state)
        {
            ModName = STR("QuickSleepWait");
            ModVersion = STR("1.0");
            ModDescription = STR("Makes waiting faster and not suck.");
            ModAuthors = STR("Patrick Eads - https://github.com/peads/QuickSleepWait");
        }

        auto on_unreal_init()->void override
        {
            const auto msg = std::wstring(state ? STR(" Success\n") : STR(" Failure\n"));
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] ") + msg);
            cleanUp();
        }
};

extern "C" {
    QUICK_SLEEP_WAIT_API CppUserModBase *start_mod()
    {
        static const char *names[] = {OBR_WIN64, OBR_WINGDK};
        static PMO::Pattern pattern(SLEEP_WAIT_PATTERN,
                                    SLEEP_WAIT_MASK,
                                    SLEEP_WAIT_CODE);
        static const HMODULE module = PMO::findModule(names);
        auto [lpBaseOfDll, SizeOfImage, EntryPoint] = PMO::getImportInfo(module);
        const PMO::PointerUnion pu{lpBaseOfDll};

        return new QuickSleepWait(findPatterns(pu.address, SizeOfImage, pattern)
                                  && replaceCode(pattern.back(),
                                                 pattern.code.str,
                                                 pattern.codeLen));
    }

    QUICK_SLEEP_WAIT_API void uninstall_mod(const CppUserModBase *mod)
    {
        delete mod;
    }
}
