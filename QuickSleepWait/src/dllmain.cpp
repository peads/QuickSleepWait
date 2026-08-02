#include <Mod/CppUserModBase.hpp>
#include <utils/Scanner.h>
#define NOP_SIZE 12

using namespace RC;

enum class State
{
    CONSTRUCTED,
    UNREAL_READY,
    CODE_HOOKED,
    DESTROYED
};

static const uint8_t arr[] = {0x0F,0x57,0xC0};

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

    auto on_update() -> void override
    {
        if (State::UNREAL_READY != state)
        {
            return;
        }
        Scanner::Add("F3 0F 10 35 ? ? ? ? F3 0F 58 C6 F3 0F 11 05", [](uint8_t* addr) {
            // Resolve pointer to next sleep tick
            const auto relOff = *reinterpret_cast<int32_t*>(addr + 16);
            // const auto rip = (addr + 20);
            // const auto ptr = rip + relOff;
            //timeUntilNextSleepTick = reinterpret_cast<float*>(ptr);
            Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] Address: {}\n"), (void*)addr);
            
            // Nop original code
            DWORD flOldProtect;
            VirtualProtect(addr, NOP_SIZE, PAGE_EXECUTE_READWRITE, &flOldProtect);
            memset(addr, 0x90, NOP_SIZE);
            memcpy(addr, arr, 3);
            VirtualProtect(addr, NOP_SIZE, flOldProtect, &flOldProtect);
            });
        Scanner::Scan();
        state = State::CODE_HOOKED;
    }

    auto on_unreal_init() -> void override
    {
        Output::send<LogLevel::Verbose>(STR("[QuickSleepWait] QuickSleepWait ready"));
        state = State::UNREAL_READY;
    }
};

#define QUICK_SLEEP_WAIT_API __declspec(dllexport)

extern "C" {
    QUICK_SLEEP_WAIT_API CppUserModBase* start_mod()
    {
        return new QuickSleepWait();
    }

    QUICK_SLEEP_WAIT_API void uninstall_mod(const CppUserModBase* mod)
    {
        delete mod;
    }
}