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
// ReSharper disable CppMissingIncludeGuard
#if !defined(DEBUG_HPP) && defined(IS_QSW_DEBUG)
#define DEBUG_HPP
#include <String/StringType.hpp>

#include "Zydis/Disassembler.h"

namespace QSW
{
    class Debug
    {
        public:
        static void debug(const uintptr_t addr)
        {
            constexpr size_t data = 16;
            // wchar_t buf[4096];
            ZyanUSize offset = 0;
            ZydisDisassembledInstruction instruction;
            uintptr_t runtime_address = addr;
            // wchar_t *buffer = buf;

            std::ofstream outFile(std::filesystem::temp_directory_path().append("debug.log"), std::ios::app);

            while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                 /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
                 /* runtime_address: */ runtime_address,
                 /* buffer:          */ reinterpret_cast<void*>(runtime_address + offset),
                 /* length:          */ data - offset,
                 /* instruction:     */ &instruction )))
            {
                // wchar_t wbuf[96];
                // const size_t len = mbstowcs(wbuf, instruction.text, 96);
                // // len + 2
                // swprintf(buffer, len + 3, STR("\n%s\n"), wbuf);
                // // printf("%016" PRIX64 "  %s\n", runtime_address, instruction.text);
                outFile << std::format("{:016X}: {}\n", runtime_address, instruction.text);
                offset += instruction.info.length;
                runtime_address += instruction.info.length;
                // buffer += len + 2;
            }
            // *(buffer + 1) = '\0';
            // RC::Output::send<RC::LogLevel::Default>(buf);
            outFile << std::endl;
            outFile.close();
        }
    };
}
#endif //DEBUG_HPP
