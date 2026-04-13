#include <iostream>
#include <string>
#include "memory/memory.hpp"
#include "vm/rbx.hpp"
#include "other/notifications.hpp"
#include <thread>

// LPA-RBX Process Inspector | v2 | I have put amount of focus into this version expect beter logic  compared to v1.

bool attached = false;
bool lastStateInGame = false;

bool IsGameLoaded() { // pro
    auto dm = rbx::DataModel();
    if (!dm) return false;

    if (!rbx::IsInGame(dm))
        return false;

    rbx::Services();

    if (!rbx::localplayer || !rbx::character || !rbx::humanoid)
        return false;

    auto health = memory::read<float>(rbx::humanoid + offsets::Health);
    return health.has_value() && health.value() > 0.0f;
}

bool auto_attach() {
    if (memory::proc.valid())
        return true;

    if (!memory::proc.find("RobloxPlayerBeta.exe"))
        return false;

    if (!memory::wait_for_window())
        return false;

    if (!memory::proc.attach())
        return false;

    return memory::proc.valid();
}

int main() {
    std::cout << "[+] waiting for Roblox\n";

    while (true) {

        if (!attached) {
            attached = auto_attach();

            if (attached) {
                notifications::show(L"LPA", L"Attached");
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        if (!memory::proc.valid()) {
            attached = false;
            notifications::show(L"Roblox", L"Lost connection — reattaching");
            continue;
        }

        rbx::Services();

        SetConsoleTitle("LPA - RBX Inspector");

        std::cout << "[1] Roblox info\n";
        std::cout << "[2] DataModel info\n";
        std::cout << "[3] Player info\n";
        std::cout << "[4] Camera info\n";
        std::cout << "[5] Workspace stats\n";
        std::cout << "[6] Player stats\n";
		std::cout << "[7] Dump Character Parts\n";
        std::cout << "[8] Manual refresh services\n";
        std::cout << "<: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "1") VM::Memory();
        else if (input == "2") VM::PrintDataModelChildren();
        else if (input == "3") VM::Players();
        else if (input == "4") VM::Camera();
        else if (input == "5") VM::workspace();
        else if (input == "6") VM::print_character_stats();
        else if (input == "7") VM::dump_character_parts();

        else if (input == "8") {
            rbx::Services();
            notifications::show(L"Roblox", L"Services refreshed");
        }

        else if (input == "P") {
            memory::proc.detach();
            std::cout << "Detached from Roblox\n";
            break;
        }

        else {
            std::cout << "Invalid option.\n";
        }
    }
}
