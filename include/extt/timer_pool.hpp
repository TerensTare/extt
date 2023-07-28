#ifndef EXTT_TIMER_POOL_HPP
#define EXTT_TIMER_POOL_HPP

#include <vector>
#include <entt/signal/dispatcher.hpp>

namespace extt
{
    /**
     * @brief A timer pool that can be used to trigger events after a certain amount of time has passed.
     *
     * @tparam Delta The type of the delta time.
     *
     * @code{.cpp}
     * #include <iostream>
     * #include <extt/timer_pool.hpp>
     *
     * struct my_timer {};
     *
     * void say_something()
     * {
     *   std::cout << "Hello, world!" << std::endl;
     * }
     *
     * int main()
     * {
     *    extt::timer_pool<float> timers;
     *
     *    timers
     *      .looping<my_timer>(1.f)
     *      .connect<&say_something>();
     *
     *    while (true)
     *    {
     *       timers.update(0.5f);
     *    }
     * }
     */
    template <typename Delta>
    class timer_pool final
    {
        template <typename T>
        struct tag final
        {
        };

    public:
        /**
         * @brief The type of the delta time.
         */
        using duration = Delta;

        /**
         * @brief Constructs a timer pool.
         */
        timer_pool() noexcept = default;

        /**
         * @brief Triggers the event associated with the given tag after the specified amount of time has passed.
         *
         * @tparam Tag The type of the event to trigger.
         * @param dt The amount of time to wait before triggering the event.
         * @return A sink object to connect to the event.
         */
        template <typename Tag>
        auto add(duration dt) noexcept
        {
            timers.push_back({{}, dt, +[](entt::dispatcher &dsp)
                                      { dsp.trigger<tag<Tag>>(); }});
            return dsp.sink<tag<Tag>>();
        }

        /**
         * @brief Triggers the event associated with the given tag every time the specified amount of time has passed.
         *
         * @tparam Tag The type of the event to trigger.
         * @param dt The amount of time to wait before triggering the event.
         * @return A sink object to connect to the event.
         */
        template <typename Tag>
        auto looping(duration dt) noexcept
        {
            loop_timers.push_back({{}, dt, +[](entt::dispatcher &dsp)
                                           { dsp.enqueue<tag<Tag>>(); }});
            return dsp.sink<tag<Tag>>();
        }

        /**
         * @brief Updates the timer pool.
         *
         * @param dt The amount of time elapsed since the last update.
         */
        constexpr void update(duration dt) noexcept
        {
            for (auto &t : timers)
                t.elapsed += dt;

            auto done = std::remove_if(timers.begin(), timers.end(), [](auto const &t)
                                       { return t.elapsed >= t.total; });

            for (auto it = done; it != timers.end(); ++it)
                it->trigger(dsp);

            timers.erase(done, timers.end());

            for (auto &t : loop_timers)
                t.elapsed += dt;

            for (auto &t : loop_timers)
                if (t.elapsed >= t.total)
                {
                    t.elapsed = {};
                    t.trigger(dsp);
                }

            dsp.update();
        }

    private:
        struct entry final
        {
            duration elapsed;
            duration total;
            void (*trigger)(entt::dispatcher &);
        };

        std::vector<entry> timers;
        std::vector<entry> loop_timers;
        entt::dispatcher dsp{};
    };
}

#endif