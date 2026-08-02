#include <Mod/CppUserModBase.hpp>
#include <utils/Scanner.h>
#define NOP_SIZE 12
#define OLD_CODE_SIG "F3 0F 10 35 ? ? ? ? F3 0F 58 C6 F3 0F 11 05"
#define OBR_WIN64 "OblivionRemastered-Win64-Shipping.exe";
#define OBR_WINGDK "OblivionRemastered-WinGDK-Shipping.exe";

using namespace RC;

enum class State
{
    CONSTRUCTED,
    UNREAL_READY,
    CODE_HOOKED,
    DESTROYED
};

static constexpr uint8_t newCode[] =
{
    0x0F,0x57,0xC0,0x90,    // xorps xmm0,xmm0
    0x90,0x90,0x90,0x90,    // nop; nop; ...
    0x90,0x90,0x90,0x90
};

class QuickSleepWait final : public CppUserModBase
{
    State state;

    public:
        QuickSleepWait()
        {
            state = State::CONSTRUCTED;

            ModName = STR("QuickSleepWait");
            ModVersion = STR("1.0");
            ModDescription = STR("Makes waiting faster and not suck.");
            ModAuthors = STR("Patrick Eads");
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait constructed"));
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
            Scanner::Add(OLD_CODE_SIG,
                         [](const uint8_t *addr)->void
                         {
                             Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Address: {}\n"),
                                                             (void*) addr);

                             // Nop original code
                             DWORD flOldProtect;
                             VirtualProtect(LPVOID(addr),
                                            NOP_SIZE,
                                            PAGE_EXECUTE_READWRITE,
                                            &flOldProtect);
                             memcpy((void*) addr, newCode, NOP_SIZE);
                             VirtualProtect(LPVOID(addr), NOP_SIZE, flOldProtect, &flOldProtect);
                         });
            Scanner::Scan();
            state = State::CODE_HOOKED;
        }

        auto on_unreal_init()->void override
        {
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait ready"));
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
