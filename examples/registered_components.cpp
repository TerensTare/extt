
#include <entt/entity/registry.hpp>
#include "registered_components.hpp"

// From this point on, trying to insert/update/get/remove a non-registered component from a registry will fail compilation.

// OK: this is a component
struct derived_component: extt::component {};

// OK: this is also a component
struct component_with_alias {
    using ecs_component = void;
};

// OK: char is a component now
template<>
struct extt::is_component<char>: std::true_type {};

int main() {
    entt::registry reg;

    {
        auto const id = reg.create();

        reg.emplace<derived_component>(id);    // OK
        reg.emplace<component_with_alias>(id); // also OK
        reg.emplace<char>(id, 'a');            // OK as well

        // reg.emplace<int>(id); // oops
    }
}