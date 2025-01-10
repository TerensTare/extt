#ifndef EXTT_CREATE_HPP
#define EXTT_CREATE_HPP

#include <entt/entity/registry.hpp>

namespace extt {
/*! @cond TURN_OFF_DOXYGEN */
namespace internal {
template<typename T>
struct remove_cvref final
    : std::remove_cv<std::remove_reference_t<T>> {
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;
} // namespace internal
/*! @endcond */

/**
 * @brief Creates an entity and emplaces the given components.
 *
 * @tparam Ts The types of the components to emplace.
 * @param reg The registry to use.
 * @param ts The components to emplace.
 * @return The id of the created entity.
 */
template<typename Entity = entt::entity, typename Allocator = std::allocator<Entity>, typename... Ts>
Entity create(entt::basic_registry<Entity, Allocator> &reg, Ts &&...ts) {
    auto const id = reg.create();
    ((reg.template emplace<internal::remove_cvref_t<Ts>>(id, std::forward<Ts>(ts))), ...);
    return id;
}
} // namespace extt

#endif