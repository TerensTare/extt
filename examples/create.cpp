
#include "create.hpp"

struct position {
    float x, y;
};

struct velocity {
    float vx, vy;
};

int main() {
    entt::registry reg;

    // calls reg.create() and then emplaces all the passed components to the entity
    auto const id = extt::create(
        reg,
        position{10, 0},
        velocity{1, 0} //
    );
}