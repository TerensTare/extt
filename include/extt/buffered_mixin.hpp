#ifndef EXTT_BUFFERED_MIXIN_HPP
#define EXTT_BUFFERED_MIXIN_HPP

#include <entt/entity/storage.hpp>

// TODO:
// - explain how this can also be used on entities specifically
// - for now this only supports destruction, note that and add support for other operations in the future
// - find something more efficient than a triple array
// - recheck move ctor; is it moving everything as it should?

namespace extt {
/**
 * @brief A mixin to storages that delays deletions until flushed. Can be used both on entities and components alike.
 * 
 * To use this mixin, override `entt::storage_type` for the types you want to have the mixin, as following:
 *
 * @code{cpp}
 * struct my_type_i_need_buffered {};
 *
 * template <>
 * struct entt::storage_type<my_type_i_need_buffered>
 * {
 *      using type = extt::buffered_mixin<entt::storage<my_type_i_need_buffered>>;
 * };
 * @endcode
 * 
 * Then use the storage as usual, and call `flush()` when you need your operations to take effect.
 * 
 * Note that this storage flushes all its operations (if any) on destruction, unless moved.
 */
template<typename Type>
class basic_buffered_mixin final: public Type {
    using underlying_type = Type;

    using alloc_traits = std::allocator_traits<typename underlying_type::allocator_type>;
    using container_type = std::vector<
        typename underlying_type::entity_type,
        typename alloc_traits::template rebind_alloc<typename underlying_type::entity_type>>;

protected:
    using basic_iterator = typename underlying_type::basic_iterator;

    void pop(basic_iterator first, basic_iterator last) override {
        to_delete.insert(to_delete.end(), first, last);
    }

public:
    using entity_type = typename underlying_type::entity_type;
    using allocator_type = typename underlying_type::allocator_type;

    basic_buffered_mixin()
        : basic_buffered_mixin{allocator_type{}} {}

    explicit basic_buffered_mixin(const allocator_type &allocator)
        : underlying_type{allocator},
          to_delete{allocator} {}

    basic_buffered_mixin(const basic_buffered_mixin &) = delete;

    basic_buffered_mixin(basic_buffered_mixin &&other) noexcept
        : to_delete{std::move(other.to_delete)},
          underlying_type{std::move(other)} {}

    basic_buffered_mixin(basic_buffered_mixin &&other, const allocator_type &allocator)
        : to_delete{std::move(other.to_delete), allocator},
          underlying_type{std::move(other), allocator} {}

    ~basic_buffered_mixin() override {
        flush();
    }

    basic_buffered_mixin &operator=(const basic_buffered_mixin &) = delete;

    basic_buffered_mixin &operator=(basic_buffered_mixin &&other) noexcept {
        underlying_type::swap(other);
        return *this;
    }

    /**
     * @brief Apply all buffered operations since the last call to `flush` (if any).
     */
    void flush() {
        for(auto const id: to_delete) {
            // TODO: these entities are guaranteed to be part of the storage. Find a way to avoid the check
            auto iter = find(id);
            underlying_type::pop(iter, iter + 1u);
        }

        to_delete.clear();
    }

private:
    container_type to_delete;
};

template<typename Type>
using buffered_mixin = basic_buffered_mixin<Type>;
} // namespace extt

#endif