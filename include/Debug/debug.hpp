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
            static void debug(const uintptr_t addr, const size_t data, std::wstringstream &ws)
            {
                ZyanUSize offset = 0;
                ZydisDisassembledInstruction instruction;
                uintptr_t runtime_address = addr;
                std::wstringstream outFile{};

                outFile.seekp(0, std::ios::end);
                ws.clear();
                for (size_t i = 0; i < data; ++i)
                {
                    ws << std::format(STR("{:02X} "), *(uint8_t *)(addr + i));
                }
                ws << std::endl;
                outFile << "\n[QuickSleepWait]\n-------------" << std::endl;

                while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                                        ZYDIS_MACHINE_MODE_LONG_64,
                                        runtime_address,
                                        reinterpret_cast<void*>(runtime_address + offset),
                                         (data << 2) - offset,
                                        &instruction )))
                {
                    size_t retval;
                    wchar_t buffer[96];
                    retval = mbstowcs_s(&retval, buffer, instruction.text, std::strlen(instruction.text));
                    outFile << std::format(STR("{:016X}:\t{}"), runtime_address, buffer);

                    offset += instruction.info.length;
                    runtime_address += instruction.info.length;
                    outFile << std::endl;
                }
                outFile << "-------------" << std::endl;
                outFile.seekp(0, std::ios::beg);
                Output::send<LogLevel::Verbose>(outFile.str());
                // outFile.close();
            }
    };
}
#endif //DEBUG_HPP
