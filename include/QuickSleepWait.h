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
// #ifdef _HAS_EXCEPTIONS
// #undefine _HAS_EXCEPTIONS
// #endif
// #define _HAS_EXCEPTIONS 0
#include <Mod/CppUserModBase.hpp>
#include <Syx/Syx.h>
#ifndef IS_QSW_RELEASE
#include <UE4SSProgram.hpp>
#endif
#ifdef IS_QSW_DEBUG
#include <Debug/debug.hpp>
#endif

// #ifdef UNICODE
// #define OBR_WIN64 STR("OblivionRemastered-Win64-Shipping.exe")
// #define OBR_WINGDK STR("OblivionRemastered-WinGDK-Shipping.exe")
// #else
#define OBR_WIN64 "OblivionRemastered-Win64-Shipping.exe"
#define OBR_WINGDK "OblivionRemastered-WinGDK-Shipping.exe"
#define SIZE 12
// #endif
#endif //QSW_H
