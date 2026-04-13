#pragma once
#include <cstdint>
#include "Roblox/instance.hpp"
#include "Libs/Functions.hpp"

namespace rbx {
    extern uintptr_t datamodel;
    extern uintptr_t workspace;
    extern uintptr_t localplayer;
    extern uintptr_t character;
    extern uintptr_t humanoid;

    uintptr_t DataModel();
    void Services();
	void ResetPlayerState();
	void UpdatePlayerCharacter();

    inline bool IsInGame(uintptr_t DataModel)
    {
        auto gameLoaded = memory::read<uint32_t>(DataModel + offsets::GameLoaded);
        if (!gameLoaded)
            return false;

        return gameLoaded.value() == 31;
    }
}