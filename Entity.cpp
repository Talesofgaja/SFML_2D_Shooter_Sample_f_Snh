#include "Entity.h"
#include <cmath>
#include <algorithm>

// ============= GameObject Implementation =============

GameObject::GameObject(float x, float y, float w, float h)
    : position(x, y), size(w, h), isActive(true) {}

GameObject::~GameObject() {}

// ============= Entity Implementation =============

Entity::Entity(float x, float y, float w, float h, float spd, sf::Color col)
    : GameObject(x, y, w, h), velocity(0, 0), speed(spd), color(col) {}

void Entity::move(float dx, float dy, float dt)
{
    velocity.x = dx * speed;
    velocity.y = dy * speed;
}

void Entity::update(float dt)
{
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}

void Entity::render(sf::RenderWindow &window)
{
    sf::RectangleShape shape(size);
    shape.setPosition(position);
    shape.setFillColor(color);
    window.draw(shape);
}

// ============= Player Implementation =============

Player::Player(float x, float y)
    : Entity(x, y, 30, 30, 200.0f, sf::Color::Green),
      health(100), canShoot(true), shootCooldown(0.2f), cooldownTimer(0) {}

void Player::handleInput(float dt)
{
    float dx = 0, dy = 0;

    // SFML 3.x: Keyboard keys are now in an enum class sf::Keyboard::Key
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        dy = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        dy = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        dx = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        dx = 1;

    // Normalize diagonal movement
    if (dx != 0 && dy != 0)
    {
        dx *= 0.707f; // 1/sqrt(2)
        dy *= 0.707f;
    }

    move(dx, dy, dt);
}

void Player::update(float dt)
{
    Entity::update(dt);

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

bool Player::tryShoot()
{
    if (canShoot)
    {
        canShoot = false;
        return true;
    }
    return false;
}

void Player::takeDamage(float damage) { health -= damage; }

// ============= Enemy Implementation =============

Enemy::Enemy(float x, float y, Player *player)
    : Entity(x, y, 25, 25, 100.0f, sf::Color::Red),
      detectionRange(300.0f), targetPlayer(player) {}

void Enemy::update(float dt)
{
    if (targetPlayer && targetPlayer->getActive())
    {
        sf::Vector2<float> playerPos = targetPlayer->getPosition();
        sf::Vector2<float> dir = playerPos - position;
        float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (distance < detectionRange && distance > 0)
        {
            dir.x /= distance;
            dir.y /= distance;
            move(dir.x, dir.y, dt);
        }
        else
        {
            move(0, 0, dt);
        }
    }

    Entity::update(dt);
}