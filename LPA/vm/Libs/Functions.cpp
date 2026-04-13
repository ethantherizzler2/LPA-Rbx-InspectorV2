#pragma once
#include "Functions.hpp"

namespace VM
{
    void Memory() {
        std::cout << "Base: 0x" << std::hex << memory::get_base() << std::dec << "\n";
        std::cout << "DataModel: 0x" << std::hex << rbx::datamodel << std::dec << "\n";
        std::cout << "Workspace: 0x" << std::hex << rbx::workspace << std::dec << "\n";
        std::cout << "LocalPlayer: 0x" << std::hex << rbx::localplayer << std::dec << "\n";
        std::cout << "Character: 0x" << std::hex << rbx::character << std::dec << "\n";
        std::cout << "Humanoid: 0x" << std::hex << rbx::humanoid << std::dec << "\n";
    }

    void Players() {
        instance::Instance playersInst(rbx::datamodel);
        playersInst = playersInst.find_first_child("Players");

        instance::Instance localPlayerInst(rbx::localplayer);
        std::cout << "\n[+] Player Details\n";

        for (auto player : playersInst.children()) {

            std::cout << "Player: " << player.name();

            if (player.name() == localPlayerInst.name())
                std::cout << " (Local Player)";

            std::cout << "\n  Class: " << player.classname();
            std::cout << "\n  Address: 0x" << std::hex << player.address << std::dec << "\n";

            instance::Instance charInst =
                instance::Instance(rbx::workspace).find_first_child(player.name());

            if (charInst.address) {
                std::cout << "  Character: 0x" << std::hex << charInst.address << std::dec;
                std::cout << " Position: " << charInst.position() << "\n";
            }

            std::cout << "---\n";
        }
    }

    void Camera()
    {
        auto cameraOpt = memory::read<uintptr_t>(rbx::workspace + offsets::Camera);
        if (!cameraOpt) return;

        uintptr_t camera = *cameraOpt;

        std::cout << "Camera: 0x" << std::hex << camera << std::dec << "\n";

        auto posOpt = memory::read<Vector3>(camera + offsets::CameraPos);
        auto rotOpt = memory::read<Vector3>(camera + offsets::CameraRotation);
        auto subjectOpt = memory::read<uintptr_t>(camera + offsets::CameraSubject);
        auto fovOpt = memory::read<float>(camera + offsets::FOV);
        auto camTypeOpt = memory::read<int>(camera + offsets::CameraType);
        auto viewportOpt = memory::read<Vector2>(camera + offsets::ViewportSize);
        auto mouseOpt = memory::read<Vector2>(rbx::workspace + offsets::MousePosition);
        auto worldOpt = memory::read<uintptr_t>(rbx::workspace + offsets::WorkspaceToWorld);

        if (posOpt)
        {
            Vector3 pos = *posOpt;
            std::cout << "\n[Transform]\n";
            std::cout << "Position: " << pos.X << ", " << pos.Y << ", " << pos.Z << "\n";
        }

        if (rotOpt)
        {
            Vector3 rot = *rotOpt;
            std::cout << "Rotation: " << rot.X << ", " << rot.Y << ", " << rot.Z << "\n";
        }

        if (fovOpt)
            std::cout << "\nFOV: " << *fovOpt << "\n";

        if (camTypeOpt)
            std::cout << "CameraType: " << *camTypeOpt << "\n";

        if (subjectOpt)
            std::cout << "Subject: 0x" << std::hex << *subjectOpt << std::dec << "\n";

        if (viewportOpt)
        {
            Vector2 vp = *viewportOpt;
            std::cout << "\n[Screen]\n";
            std::cout << "Viewport: " << vp.X << " x " << vp.Y << "\n";
        }

        if (mouseOpt)
        {
            Vector2 ms = *mouseOpt;
            std::cout << "Mouse: " << ms.X << ", " << ms.Y << "\n";
        }

        if (worldOpt)
        {
            std::cout << "\n[World]\n";
            std::cout << "WorkspaceToWorld: 0x" << std::hex << *worldOpt << std::dec << "\n";
        }
    }

    void workspace()
    {
        if (!rbx::workspace)
            return;

        instance::Instance ws(rbx::workspace);

        std::cout << "Address: 0x" << std::hex << rbx::workspace << std::dec << "\n";

        auto children = ws.children();
        std::cout << "Children: " << children.size() << "\n";

        int shown = 0;

        for (const auto& child : children)
        {
            if (!child.address)
                continue;

            std::string name = child.name();
            std::string cls = child.classname();

            std::cout << "  [" << cls << "] "
                << name
                << " (0x" << std::hex << child.address << std::dec << ")\n";

            if (cls == "Model")
            {
// maybe idk
                auto humanoid = child.find_first_child_of_class("Humanoid");
                if (humanoid.address)
                {
                    std::cout << "     -> [Character Detected]\n";
                }
            }

            if (++shown >= 10)
                break;
        }
    }

    void PrintDataModelChildren()
    {
        if (!rbx::datamodel)
            return;

        instance::Instance dm(rbx::datamodel);

        auto children = dm.children();
        std::cout << "Count: " << children.size() << "\n\n";

        for (const auto& child : children)
        {
            if (!child.address)
                continue;

            std::string name = child.name();
            std::string cls = child.classname();

            std::cout << "[" << cls << "] "
                << name
                << " (0x" << std::hex << child.address << std::dec << ")";

            if (name == "Workspace") std::cout << "  <-- [WORLD]";
            else if (name == "Players") std::cout << "  <-- [PLAYERS]";
            else if (name == "Lighting") std::cout << "  <-- [LIGHTING]";
            else if (name == "ReplicatedStorage") std::cout << "  <-- [REPLICATION]";
            else if (name == "Run Service") std::cout << "  <-- [ENGINE]";
            else if (name == "StarterGui") std::cout << "  <-- [UI]";

            std::cout << "\n";
        }
    }

    void print_character_stats()
    {
        if (!rbx::character || !rbx::humanoid)
        {
            std::cout << "[-] Character not found\n";
            return;
        }

        instance::Instance charInst(rbx::character);
        instance::Instance humanoidInst(rbx::humanoid);

        std::cout << "\n[+] Character Stats\n";

        float health = humanoidInst.health();
        float maxHealth = humanoidInst.max_health();
        float walk = humanoidInst.walk_speed();
        float jump = humanoidInst.jump_power();

        std::cout << "Health: " << health << "/" << maxHealth << "\n";
        std::cout << "Walk Speed: " << walk << "\n";
        std::cout << "Jump Power: " << jump << "\n";

        instance::Instance root;

        //  R6 + R15
        const char* rootCandidates[] = {
            "HumanoidRootPart", // common
            "UpperTorso",       // R15
            "LowerTorso",       // R15
            "Torso",            // R6
            "Head"
        };

        for (auto name : rootCandidates)
        {
            root = charInst.find_first_child(name);
            if (root.address)
                break;
        }

        if (!root.address)
        {
            std::cout << "[-] no body part found\n";
            return;
        }

        Vector3 pos = root.position();
        Vector3 vel = root.velocity();

        std::cout << "Root Part: " << root.name() << "\n";
        std::cout << "Position: " << pos.X << ", " << pos.Y << ", " << pos.Z << "\n";
        std::cout << "Velocity: " << vel.X << ", " << vel.Y << ", " << vel.Z << "\n";
    }

    void dump_character_parts()
    {
        instance::Instance charInst(rbx::character);

        std::cout << "\n[+] Character Children\n";
        for (auto child : charInst.children())
        {
            std::cout << "[" << child.classname() << "] "
                << child.name()
                << " (0x" << std::hex << child.address << std::dec << ")\n";
        }
    }
}