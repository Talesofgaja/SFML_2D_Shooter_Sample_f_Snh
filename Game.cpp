#include "Game.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <optional>

// SFML 3.x: Window constructor uses an initializer list for settings
Game::Game() : window({{800, 600}, "2D Shooter - OOP Project (SFML 3.x)"})
{
    window.setVerticalSyncEnabled(true);

    player = std::make_unique<Player>(400, 300);

    walls.push_back(std::make_unique<Wall>(0, 0, 800, 20));
    walls.push_back(std::make_unique<Wall>(0, 580, 800, 20));
    walls.push_back(std::make_unique<Wall>(0, 0, 20, 600));
    walls.push_back(std::make_unique<Wall>(780, 0, 20, 600));
    walls.push_back(std::make_unique<Wall>(200, 200, 100, 20));
    walls.push_back(std::make_unique<Wall>(500, 400, 20, 150));

    destructibles.push_back(std::make_unique<DestructibleObject>(300, 300, 40, 40, 100));
    destructibles.push_back(std::make_unique<DestructibleObject>(600, 200, 40, 40, 100));

    enemies.push_back(std::make_unique<Enemy>(200, 100, player.get()));
    enemies.push_back(std::make_unique<Enemy>(600, 500, player.get()));
}

void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents()
{
    // SFML 3.x: pollEvent returns an optional event
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        // SFML 3.x: Check event type with is<T>()
        if (event->is<sf::Event::Closed>())
            window.close();

        // SFML 3.x: Get event data with getIf<T>()
        if (const auto *mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
        {
            // SFML 3.x: Mouse buttons are in an enum class sf::Mouse::Button
            if (mouseButton->button == sf::Mouse::Button::Left)
            {
                if (player->tryShoot())
                {
                    sf::Vector2<float> pPos = player->getPosition();
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    float dx = static_cast<float>(mousePos.x) - (pPos.x + 15);
                    float dy = static_cast<float>(mousePos.y) - (pPos.y + 15);

                    projectiles.push_back(
                        std::make_unique<Projectile>(pPos.x + 15, pPos.y + 15, dx, dy));
                }
            }
        }
    }
}

void Game::update(float dt)
{
    player->handleInput(dt);
    player->update(dt);

    for (auto &enemy : enemies)
        enemy->update(dt);
    for (auto &proj : projectiles)
        proj->update(dt);
    for (auto &dest : destructibles)
        dest->update(dt);

    handleCollisions();
    cleanupInactive();
}

void Game::render()
{
    window.clear(sf::Color(50, 50, 50));

    for (auto &wall : walls)
        wall->render(window);
    for (auto &dest : destructibles)
        dest->render(window);
    player->render(window);
    for (auto &enemy : enemies)
        enemy->render(window);
    for (auto &proj : projectiles)
        proj->render(window);

    window.display();
}

void Game::handleCollisions()
{
    // Player vs walls
    for (auto &wall : walls)
    {
        if (wall->getActive())
        {
            sf::Rect<float> pBounds = player->getBounds();
            sf::Rect<float> wBounds = wall->getBounds();

            // SFML 3.x: findIntersection returns an optional
            if (const auto intersection = pBounds.findIntersection(wBounds))
            {
                sf::Vector2<float> pPos = player->getPosition();
                float overlapX = intersection->size.x;
                float overlapY = intersection->size.y;

                if (overlapX < overlapY)
                {
                    if (pBounds.position.x < wBounds.position.x)
                        pPos.x -= overlapX;
                    else
                        pPos.x += overlapX;
                }
                else
                {
                    if (pBounds.position.y < wBounds.position.y)
                        pPos.y -= overlapY;
                    else
                        pPos.y += overlapY;
                }
                player->setPosition(pPos); // Update player position
            }
        }
    }

    // Projectiles vs entities
    for (auto &proj : projectiles)
    {
        if (!proj->getActive())
            continue;

        // vs enemies
        for (auto &enemy : enemies)
        {
            if (enemy->getActive() && proj->getBounds().intersects(enemy->getBounds()))
            {
                proj->setActive(false);
                enemy->setActive(false);
                break;
            }
        }

        if (!proj->getActive())
            continue; // Check again in case it hit an enemy

        // vs destructibles
        for (auto &dest : destructibles)
        {
            if (dest->getActive() && proj->getBounds().intersects(dest->getBounds()))
            {
                proj->setActive(false);
                dest->takeDamage(25.0f);
                break;
            }
        }
    }
}

void Game::cleanupInactive()
{
    std::erase_if(projectiles, [](const auto &p)
                  { return !p->getActive(); });
    std::erase_if(enemies, [](const auto &e)
                  { return !e->getActive(); });
    std::erase_if(destructibles, [](const auto &d)
                  { return !d->getActive(); });
}