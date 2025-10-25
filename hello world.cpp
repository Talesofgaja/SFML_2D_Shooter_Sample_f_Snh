// main.cpp - Entry point
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>

// ============= BASE CLASSES =============

// Abstract base class for all game objects
class GameObject
{
protected:
    sf::Vector2f position;
    sf::Vector2f size;
    bool isActive;

public:
    GameObject(float x, float y, float w, float h)
        : position(x, y), size(w, h), isActive(true) {}

    virtual ~GameObject() {}

    // Pure virtual functions (must be implemented by derived classes)
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow &window) = 0;
    virtual sf::FloatRect getBounds() const
    {
        return sf::FloatRect(position.x, position.y, size.x, size.y);
    }

    bool getActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    sf::Vector2f getPosition() const { return position; }
};

// ============= ENTITY CLASSES (Moving Objects) =============

// Base class for entities that can move
class Entity : public GameObject
{
protected:
    sf::Vector2f velocity;
    float speed;
    sf::Color color;

public:
    Entity(float x, float y, float w, float h, float spd, sf::Color col)
        : GameObject(x, y, w, h), velocity(0, 0), speed(spd), color(col) {}

    virtual void move(float dx, float dy, float dt)
    {
        velocity.x = dx * speed;
        velocity.y = dy * speed;
    }

    virtual void update(float dt) override
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    virtual void render(sf::RenderWindow &window) override
    {
        sf::RectangleShape shape(size);
        shape.setPosition(position);
        shape.setFillColor(color);
        window.draw(shape);
    }
};

// Player class with input handling
class Player : public Entity
{
private:
    float health;
    bool canShoot;
    float shootCooldown;
    float cooldownTimer;

public:
    Player(float x, float y)
        : Entity(x, y, 30, 30, 200.0f, sf::Color::Green),
          health(100), canShoot(true), shootCooldown(0.2f), cooldownTimer(0) {}

    void handleInput(float dt)
    {
        float dx = 0, dy = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            dy = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            dy = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            dx = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            dx = 1;

        // Normalize diagonal movement
        if (dx != 0 && dy != 0)
        {
            dx *= 0.707f; // 1/sqrt(2)
            dy *= 0.707f;
        }

        move(dx, dy, dt);
    }

    void update(float dt) override
    {
        Entity::update(dt);

        // Update shoot cooldown
        if (!canShoot)
        {
            cooldownTimer += dt;
            if (cooldownTimer >= shootCooldown)
            {
                canShoot = true;
                cooldownTimer = 0;
            }
        }
    }

    bool tryShoot()
    {
        if (canShoot)
        {
            canShoot = false;
            return true;
        }
        return false;
    }

    void takeDamage(float damage) { health -= damage; }
    float getHealth() const { return health; }
};

// Enemy with basic AI
class Enemy : public Entity
{
private:
    float detectionRange;
    Player *targetPlayer;

public:
    Enemy(float x, float y, Player *player)
        : Entity(x, y, 25, 25, 100.0f, sf::Color::Red),
          detectionRange(300.0f), targetPlayer(player) {}

    void update(float dt) override
    {
        if (targetPlayer && targetPlayer->getActive())
        {
            // Simple chase AI
            sf::Vector2f playerPos = targetPlayer->getPosition();
            sf::Vector2f dir = playerPos - position;
            float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (distance < detectionRange && distance > 0)
            {
                dir.x /= distance;
                dir.y /= distance;
                move(dir.x, dir.y, dt);
            }
        }

        Entity::update(dt);
    }
};

// ============= PROJECTILE CLASS =============

class Projectile : public GameObject
{
private:
    sf::Vector2f velocity;
    float speed;
    sf::Color color;
    float lifetime;
    float age;

public:
    Projectile(float x, float y, float dx, float dy)
        : GameObject(x, y, 8, 8), speed(400.0f),
          color(sf::Color::Yellow), lifetime(3.0f), age(0)
    {

        // Normalize direction
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0)
        {
            velocity.x = (dx / length) * speed;
            velocity.y = (dy / length) * speed;
        }
    }

    void update(float dt) override
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        age += dt;
        if (age >= lifetime)
        {
            isActive = false;
        }
    }

    void render(sf::RenderWindow &window) override
    {
        sf::CircleShape shape(size.x / 2);
        shape.setPosition(position);
        shape.setFillColor(color);
        window.draw(shape);
    }
};

// ============= STATIC OBJECTS =============

class StaticObject : public GameObject
{
protected:
    sf::Color color;

public:
    StaticObject(float x, float y, float w, float h, sf::Color col)
        : GameObject(x, y, w, h), color(col) {}

    void update(float dt) override
    {
        // Static objects don't update
    }

    void render(sf::RenderWindow &window) override
    {
        sf::RectangleShape shape(size);
        shape.setPosition(position);
        shape.setFillColor(color);
        window.draw(shape);
    }
};

// Wall obstacle
class Wall : public StaticObject
{
public:
    Wall(float x, float y, float w, float h)
        : StaticObject(x, y, w, h, sf::Color(100, 100, 100)) {}
};

// Destructible object (multi-level inheritance)
class DestructibleObject : public StaticObject
{
protected:
    float health;
    float maxHealth;

public:
    DestructibleObject(float x, float y, float w, float h, float hp)
        : StaticObject(x, y, w, h, sf::Color(139, 69, 19)),
          health(hp), maxHealth(hp) {}

    void takeDamage(float damage)
    {
        health -= damage;
        if (health <= 0)
        {
            isActive = false;
        }
        // Change color based on health
        float ratio = health / maxHealth;
        color = sf::Color(139 * ratio, 69 * ratio, 19 * ratio);
    }

    float getHealth() const { return health; }
};

// ============= GAME MANAGER =============

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
    sf::Font font;

    void handleCollisions()
    {
        // Player vs walls
        for (auto &wall : walls)
        {
            if (wall->getActive() &&
                player->getBounds().intersects(wall->getBounds()))
            {
                // Simple push-back collision
                sf::Vector2f pPos = player->getPosition();
                sf::FloatRect pBounds = player->getBounds();
                sf::FloatRect wBounds = wall->getBounds();

                // Calculate overlap
                float overlapX = std::min(pBounds.left + pBounds.width - wBounds.left,
                                          wBounds.left + wBounds.width - pBounds.left);
                float overlapY = std::min(pBounds.top + pBounds.height - wBounds.top,
                                          wBounds.top + wBounds.height - pBounds.top);

                // Push back in smallest overlap direction
                if (overlapX < overlapY)
                {
                    if (pBounds.left < wBounds.left)
                        pPos.x -= overlapX;
                    else
                        pPos.x += overlapX;
                }
                else
                {
                    if (pBounds.top < wBounds.top)
                        pPos.y -= overlapY;
                    else
                        pPos.y += overlapY;
                }
            }
        }

        // Projectiles vs enemies
        for (auto &proj : projectiles)
        {
            if (!proj->getActive())
                continue;

            for (auto &enemy : enemies)
            {
                if (enemy->getActive() &&
                    proj->getBounds().intersects(enemy->getBounds()))
                {
                    proj->setActive(false);
                    enemy->setActive(false);
                    break;
                }
            }

            // Projectiles vs destructibles
            for (auto &dest : destructibles)
            {
                if (dest->getActive() &&
                    proj->getBounds().intersects(dest->getBounds()))
                {
                    proj->setActive(false);
                    dest->takeDamage(25.0f);
                    break;
                }
            }
        }
    }

    void cleanupInactive()
    {
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                           [](const auto &p)
                           { return !p->getActive(); }),
            projectiles.end());

        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                           [](const auto &e)
                           { return !e->getActive(); }),
            enemies.end());

        destructibles.erase(
            std::remove_if(destructibles.begin(), destructibles.end(),
                           [](const auto &d)
                           { return !d->getActive(); }),
            destructibles.end());
    }

public:
    Game() : window(sf::VideoMode(800, 600), "2D Shooter - OOP Project")
    {
        window.setVerticalSyncEnabled(true);

        // Create player
        player = std::make_unique<Player>(400, 300);

        // Create walls (map boundaries)
        walls.push_back(std::make_unique<Wall>(0, 0, 800, 20));   // Top
        walls.push_back(std::make_unique<Wall>(0, 580, 800, 20)); // Bottom
        walls.push_back(std::make_unique<Wall>(0, 0, 20, 600));   // Left
        walls.push_back(std::make_unique<Wall>(780, 0, 20, 600)); // Right

        // Add some interior walls
        walls.push_back(std::make_unique<Wall>(200, 200, 100, 20));
        walls.push_back(std::make_unique<Wall>(500, 400, 20, 150));

        // Add destructible objects
        destructibles.push_back(std::make_unique<DestructibleObject>(300, 300, 40, 40, 100));
        destructibles.push_back(std::make_unique<DestructibleObject>(600, 200, 40, 40, 100));

        // Create enemies
        enemies.push_back(std::make_unique<Enemy>(200, 100, player.get()));
        enemies.push_back(std::make_unique<Enemy>(600, 500, player.get()));
    }

    void run()
    {
        while (window.isOpen())
        {
            float dt = clock.restart().asSeconds();
            dt = std::min(dt, 0.05f); // Cap delta time to prevent huge jumps

            handleEvents();
            update(dt);
            render();
        }
    }

    void handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Mouse click to shoot
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                if (player->tryShoot())
                {
                    sf::Vector2f pPos = player->getPosition();
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    float dx = mousePos.x - (pPos.x + 15);
                    float dy = mousePos.y - (pPos.y + 15);

                    projectiles.push_back(
                        std::make_unique<Projectile>(pPos.x + 15, pPos.y + 15, dx, dy));
                }
            }
        }
    }

    void update(float dt)
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

    void render()
    {
        window.clear(sf::Color(50, 50, 50));

        // Render all objects
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
};

// ============= MAIN =============

int main()
{
    Game game;
    game.run();
    return 0;
}