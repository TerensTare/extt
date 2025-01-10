
#include <entt/entity/registry.hpp>
#include "buffered_mixin.hpp"

template<typename T>
struct entt::storage_type<T> {
    using type = extt::buffered_mixin<entt::storage<T>>;
};

// cl -EHsc -std:c++17 -O2 main.cpp -Iinclude\extt -I%vcpkg_root%\installed\x64-windows\include

int main() {
    entt::registry reg;

    {
        auto const id = reg.create();
        reg.emplace<int>(id, 42);

        reg.erase<int>(id);

        if(reg.try_get<int>(id)) {
            std::printf("Type still has int\n");
        } else {
            std::printf("No int\n");
        }

        reg.storage<int>().flush();

        if(reg.try_get<int>(id)) {
            std::printf("Type still has int\n");
        } else {
            std::printf("No int\n");
        }
    }

    for(int i = 0; i < 26; ++i) {
        auto const id = reg.create();
        reg.emplace<char>(id, 'a' + i);
    }

    auto chars = reg.view<char const>();
    reg.destroy(chars.begin(), chars.end());

    std::printf("%zd entities left\n", reg.alive());

    reg.storage<entt::entity>().flush();

    std::printf("Now there are %zd entities left\n", reg.alive());
}