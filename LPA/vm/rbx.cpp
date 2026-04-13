#include "rbx.hpp"
#include <iostream>
#include <iomanip>
#include "memory/memory.hpp"
#include "rbxglobals.h"

namespace rbx {

    uintptr_t DataModel() {
        uintptr_t base = memory::get_base();
        if (!base) return 0;

        auto fake_dm = memory::read<uintptr_t>(base + offsets::FakeDataModelPointer);
        if (!fake_dm) return 0;

        auto dm = memory::read<uintptr_t>(*fake_dm + offsets::FakeDataModelToDataModel);
        if (!dm) return 0;

        return *dm;
    }

    void Services() {
        datamodel = DataModel();
        if (!datamodel) return;

        instance::Instance dm(datamodel);

        auto get = [&](const char* name) -> uintptr_t {
            return dm.find_first_child(name).address;
            };

        workspace = get("Workspace");
        players = get("Players");
        lighting = get("Lighting");
        RunService = get("Run Service");
        LocalScriptService = get("LocalScriptService");
        ReplicatedStorage = get("ReplicatedStorage");
        ReplicatedFirst = get("ReplicatedFirst");
        StarterGui = get("StarterGui");
        StarterPack = get("StarterPack");
        StarterPlayer = get("StarterPlayer");
        Teams = get("Teams");
        Chat = get("Chat");
        MarketplaceService = get("MarketplaceService");

        if (!workspace || !players) {
            ResetPlayerState();
            return;
        }

        UpdatePlayerCharacter();
    }

    void ResetPlayerState()
    {
        character = 0;
        humanoid = 0;
        humanoidrootpart = 0; 
        localplayer = 0;
    }

    void UpdatePlayerCharacter()
    {
        auto lp = memory::read<uintptr_t>(players + offsets::LocalPlayer);
        if (!lp) {
            ResetPlayerState();
            return;
        }

        localplayer = *lp;

        instance::Instance plr(localplayer);
        const std::string plr_name = plr.name();

        if (plr_name.empty()) {
            ResetPlayerState();
            return;
        }

        instance::Instance ws(workspace);

        auto getChar = [&]() -> instance::Instance {
            auto c = ws.find_first_child(plr_name);
            if (c.address) return c;
            return ws.find_descendant(plr_name);
            };

        instance::Instance char_obj = getChar();
        if (!char_obj.address) {
            ResetPlayerState();
            return;
        }

        character = char_obj.address;
        instance::Instance char_inst(character);

        auto getHumanoid = [&]() -> instance::Instance {
            auto h = char_inst.find_first_child("Humanoid");
            if (h.address) return h;
            return char_inst.find_first_child_of_class("Humanoid");
            };

        instance::Instance hum_obj = getHumanoid();
        humanoid = hum_obj.address;

        auto getHumanoidRootPart = [&]() -> instance::Instance {
            auto hrp = char_inst.find_first_child("HumanoidRootPart");
            if (hrp.address) return hrp;

            return char_inst.find_first_child_of_class("Part");
            };

        instance::Instance hrp_obj = getHumanoidRootPart();
        humanoidrootpart = hrp_obj.address;
    }

    bool valid() {
        return datamodel &&
            workspace &&
            players &&
            localplayer &&
            character &&
            humanoid &&
            humanoidrootpart;
    }
}