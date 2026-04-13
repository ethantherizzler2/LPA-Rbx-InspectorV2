#pragma once
#include <vector>
#include <string>
#include <cstdint>

#include "other/vector.hpp"
#include "other/offsets.hpp"
#include "memory/memory.hpp"

namespace instance {

    class Instance {
    public:
        uintptr_t address;

        Instance(uintptr_t addr = 0)
            : address(addr) {}

        std::string name() const;
        std::string classname() const;
        Instance parent() const;

        std::vector<Instance> children() const;
        Instance find_first_child(std::string_view name) const;
        Instance find_first_child_of_class(std::string_view class_name) const;
        std::vector<Instance> get_children_of_class(std::string_view class_name) const;
        Instance find_descendant(std::string_view name) const;

        Vector3 position() const;
        Vector3 velocity() const;

        float walk_speed() const;
        float jump_power() const;

        bool anchored() const;
        bool can_collide() const;

        float health() const;
        float max_health() const;
        float transparency() const;

        std::string display_name() const;
        std::string value() const;

        Instance team() const;

        bool is_a(std::string_view class_name) const;
        std::string full_path() const;

        operator bool() const { return address != 0; }
    };

}