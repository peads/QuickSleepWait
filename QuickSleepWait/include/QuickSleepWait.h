#ifndef QSW_H
#define QSW_H
#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
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
#define OBR_WIN64 STR("OblivionRemastered-Win64-Shipping.exe")
#define OBR_WINGDK STR("OblivionRemastered-WinGDK-Shipping.exe")
// #define LOBR_WIN64 L##"OblivionRemastered-Win64-Shipping.exe"
// #define LOBR_WINGDK L##"OblivionRemastered-WinGDK-Shipping.exe"

// using FMBNI_ExtraPredicate = std::function<bool(Mod*)>;

#endif //QSW_H
