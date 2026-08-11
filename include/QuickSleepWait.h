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
#ifndef QSW_H
#define QSW_H
#include <Mod/CppUserModBase.hpp>
#ifndef IS_QSW_RELEASE
#include <UE4SSProgram.hpp>
#endif
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
#ifdef UNICODE
#define OBR_WIN64 STR("OblivionRemastered-Win64-Shipping.exe")
#define OBR_WINGDK STR("OblivionRemastered-WinGDK-Shipping.exe")
#else
#define OBR_WIN64 "OblivionRemastered-Win64-Shipping.exe"
#define OBR_WINGDK "OblivionRemastered-WinGDK-Shipping.exe"
#endif
#endif //QSW_H
