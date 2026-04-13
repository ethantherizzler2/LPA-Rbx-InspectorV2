#include "Roblox/instance.hpp"
#include "vm/rbx.hpp"

namespace instance {

    static std::string ReadData(uintptr_t ptr) {
        if (!ptr)
            return "";

        return memory::read_string(ptr, 128);
    }

    std::string Instance::name() const {
        if (!address) return "";

        auto ptr = memory::read<uintptr_t>(address + offsets::Name);
        if (!ptr) return "";

        return memory::read_string(*ptr, 128);
    }

    std::string Instance::classname() const {
        if (!address) return "";

        auto desc = memory::read<uintptr_t>(address + offsets::ClassDescriptor);
        if (!desc) return "";

        auto name_ptr = memory::read<uintptr_t>(*desc + offsets::ClassDescriptorToClassName);
        if (!name_ptr) return "";

        return memory::read_string(*name_ptr, 128);
    }

    Instance Instance::parent() const {
        if (!address)
            return Instance(0);

        auto ptr = memory::read<uintptr_t>(address + offsets::Parent);
        return Instance(ptr ? *ptr : 0);
    }

    std::vector<Instance> Instance::children() const {
        std::vector<Instance> result;
        if (!address) return result;

        auto start = memory::read<uintptr_t>(address + offsets::Children);
        if (!start) return result;

        auto first = memory::read<uintptr_t>(*start);
        auto end = memory::read<uintptr_t>(*start + offsets::ChildrenEnd);

        if (!first || !end) return result;

        for (uintptr_t ptr = *first; ptr != *end; ptr += 0x10) {

            auto child_ptr = memory::read<uintptr_t>(ptr);
            if (!child_ptr) continue;

            result.emplace_back(*child_ptr);
        }

        return result;
    }

    Instance Instance::find_first_child(std::string_view name) const {
        if (!address) return Instance(0);

        auto start = memory::read<uintptr_t>(address + offsets::Children);
        if (!start) return Instance(0);

        auto first = memory::read<uintptr_t>(*start);
        auto end = memory::read<uintptr_t>(*start + offsets::ChildrenEnd);

        if (!first || !end) return Instance(0);

        for (uintptr_t ptr = *first; ptr != *end; ptr += 0x10) {

            auto child_ptr = memory::read<uintptr_t>(ptr);
            if (!child_ptr) continue;

            Instance child(*child_ptr);

            if (child.name() == name)
                return child;
        }

        return Instance(0);
    }

    Instance Instance::find_first_child_of_class(std::string_view class_name) const {
        if (!address)
            return Instance(0);

        for (const auto& child : children()) {
            if (child.classname() == class_name)
                return child;
        }

        return Instance(0);
    }

    std::vector<Instance> Instance::get_children_of_class(std::string_view class_name) const {
        std::vector<Instance> result;

        if (!address)
            return result;

        for (const auto& child : children()) {
            if (child.classname() == class_name)
                result.push_back(child);
        }

        return result;
    }

    Instance Instance::find_descendant(std::string_view name) const {
        if (!address) return Instance(0);

        for (const auto& child : children()) {

            if (child.name() == name)
                return child;

            Instance found = child.find_descendant(name);
            if (found)
                return found;
        }

        return Instance(0);
    }

    Vector3 Instance::position() const {
        auto v = memory::read<Vector3>(address + offsets::Position);
        return v ? *v : Vector3();
    }

    Vector3 Instance::velocity() const {
        auto v = memory::read<Vector3>(address + offsets::Velocity);
        return v ? *v : Vector3();
    }

    float Instance::walk_speed() const {
        auto v = memory::read<float>(address + offsets::WalkSpeed);
        return v ? *v : 0.0f;
    }

    float Instance::jump_power() const {
        auto v = memory::read<float>(address + offsets::JumpPower);
        return v ? *v : 0.0f;
    }

    bool Instance::anchored() const {
        auto v = memory::read<bool>(address + offsets::Anchored);
        return v ? *v : false;
    }

    bool Instance::can_collide() const {
        auto v = memory::read<bool>(address + offsets::CanCollide);
        return v ? *v : false;
    }

    float Instance::health() const {
        auto v = memory::read<float>(address + offsets::Health);
        return v ? *v : 0.0f;
    }

    float Instance::max_health() const {
        auto v = memory::read<float>(address + offsets::MaxHealth);
        return v ? *v : 0.0f;
    }

    float Instance::transparency() const {
        auto v = memory::read<float>(address + offsets::Transparency);
        return v ? *v : 0.0f;
    }

    std::string Instance::display_name() const {
        if (!address)
            return "";

        auto ptr = memory::read<uintptr_t>(address + offsets::DisplayName);
        if (!ptr) return "";

        return memory::read_string(*ptr, 128);
    }

    std::string Instance::value() const {
        if (!address)
            return "";

        auto ptr = memory::read<uintptr_t>(address + offsets::Value);
        if (!ptr) return "";

        return memory::read_string(*ptr, 128);
    }

    Instance Instance::team() const {
        if (!address)
            return Instance(0);

        auto ptr = memory::read<uintptr_t>(address + offsets::Team);
        return Instance(ptr ? *ptr : 0);
    }

    bool Instance::is_a(std::string_view class_name) const {
        return this->classname() == class_name;
    }

    std::string Instance::full_path() const {
        if (!address)
            return "";

        std::vector<std::string> parts;

        Instance current = *this;

        while (current && current.address != rbx::datamodel) {

            parts.push_back(
                current.name() + "[" + current.classname() + "]"
            );

            current = current.parent();
        }

        std::string result;

        for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
            result += *it;

            if (it + 1 != parts.rend())
                result += " → ";
        }

        return result;
    }

}