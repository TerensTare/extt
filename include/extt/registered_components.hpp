#ifndef EXTT_REGISTERED_COMPONENTS_HPP
#define EXTT_REGISTERED_COMPONENTS_HPP

#include <entt/entity/fwd.hpp>

namespace extt {
/**
 * @brief A trait used to specify types that can be used as a component on registry operations.
 *
 * After including this file, types that don't fulfill this trait will fail to compile when used on a registry.
 *
 * To make sure your type is recognized as a component, you can do one of the following:
 *
 * 1. Derive from `extt::component` (when you "own" the type and don't mind deriving):
 * @code{cpp}
 * struct my_type : extt::component {};
 *
 * static_assert(extt::is_component_v<my_type>); // OK
 * @endcode
 *
 * 2. Add an alias named `ecs_component` to your type (when you own the type, but don't want a base class; eg. for POD types):
 * @code{cpp}
 * struct my_type
 * {
 *      using ecs_component = void; // can be anything, even a new type
 * };
 *
 * static_assert(extt::is_component_v<my_type>); // OK
 * @endcode
 *
 * 3. Specialize `extt::is_component` for your type (anytime really, especially useful when using 3-rd party types):
 * @code{cpp}
 * // on global namespace
 * template <>
 * struct extt::is_component<my_type> : std::true_type {};
 *
 * static_assert(extt::is_component_v<my_type>); // OK
 * @endcode
 */
template<typename T, typename = void>
struct is_component: std::false_type {
};

template<typename T>
struct is_component<T, std::void_t<typename T::ecs_component>>: std::true_type {
};

struct component {
    using ecs_component = void;
};

template<typename T>
inline static constexpr bool is_component_v = is_component<T>::value;
} // namespace extt

template<typename Type, typename Entity, typename Allocator>
struct entt::storage_type<Type, Entity, Allocator> {
    static_assert(
        std::is_same_v<Type, Entity> || extt::is_component_v<Type>,
        "Trying to emplace a type that is not registered as a component!\n"
        "Consider deriving from `extt::component` if you own the type.\n"
        "Alternatively, you can specify an `ecs_component` member alias to avoid deriving or specialize `extt::is_component` for your type, all as following:\n"
        "```cpp\n"
        "// Option 1. Deriving from `extt::component`"
        "struct my_type : extt::component {};\n"
        "\n"
        "// Option 2. Use an alias.\n"
        "struct my_type\n"
        "{\n"
        "    using ecs_component = void; // can be anything\n"
        "};\n"
        "\n"
        "// Option 3. Specialize `extt::is_component`.\n"
        "template <>\n"
        "struct is_component<my_type> : std::true_type {};\n"
        "```\n"
        "\n");

    using type = sigh_mixin<basic_storage<Type, Entity, Allocator>>;
};

#endif