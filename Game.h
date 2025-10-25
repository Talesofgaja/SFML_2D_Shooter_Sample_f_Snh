#pragma once

#include <SFML/Graphics/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <memory>
#include "Entity.h"
#include "Projectile.h"
#include "StaticObject.h"

class Game
{
private:
    sf::RenderWindow window;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<DestructibleObject>> destructibles;

    sf::Clock clock;

    void handleCollisions();
    void cleanupInactive();
    void handleEvents();
    void update(float dt);
    void render();

public:
    Game();
    void run();
};