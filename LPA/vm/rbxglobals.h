#pragma once
#include <cstdint>
#include <string>

namespace rbx {

    inline uintptr_t datamodel = 0;

    inline uintptr_t workspace = 0;
    inline uintptr_t players = 0;
    inline uintptr_t lighting = 0;

    inline uintptr_t RunService = 0;
    inline uintptr_t LocalScriptService = 0;
    inline uintptr_t ReplicatedStorage = 0;
    inline uintptr_t ReplicatedFirst = 0;
    inline uintptr_t StarterGui = 0;
    inline uintptr_t StarterPack = 0;
    inline uintptr_t StarterPlayer = 0;
    inline uintptr_t Teams = 0;
    inline uintptr_t Chat = 0;
    inline uintptr_t MarketplaceService = 0;

    inline uintptr_t localplayer = 0;
    inline uintptr_t character = 0;
    inline uintptr_t humanoid = 0;

    inline uintptr_t camera = 0;
    inline uintptr_t current_camera = 0;
    inline uintptr_t local_team = 0;

    inline uintptr_t humanoidrootpart = 0;

    inline long long local_userid = 0;
    inline std::string local_displayname = "";
    inline long long place_id = 0;

    inline uintptr_t players_service = 0;
    inline uintptr_t stats_service = 0;
    inline uintptr_t terrain = 0;
    inline uintptr_t visualengine = 0;

}