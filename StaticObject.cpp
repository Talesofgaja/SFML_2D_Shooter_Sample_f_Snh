#include "StaticObject.h"

// ============= StaticObject Implementation =============

StaticObject::StaticObject(float x, float y, float w, float h, sf::Color col)
    : GameObject(x, y, w, h), color(col) {}

void StaticObject::update(float dt)
{
    // Static objects don't update their state over time
}

void StaticObject::render(sf::RenderWindow &window)
{
    sf::RectangleShape shape(size);
    shape.setPosition(position);
    shape.setFillColor(color);
    window.draw(shape);
}

// ============= Wall Implementation =============

Wall::Wall(float x, float y, float w, float h)
    : StaticObject(x, y, w, h, sf::Color(100, 100, 100)) {}

// ============= DestructibleObject Implementation =============

DestructibleObject::DestructibleObject(float x, float y, float w, float h, float hp)
    : StaticObject(x, y, w, h, sf::Color(139, 69, 19)),
      health(hp), maxHealth(hp) {}

void DestructibleObject::takeDamage(float damage)
{
    health -= damage;
    if (health <= 0)
    {
        isActive = false;
        health = 0; // Prevent negative health
    }

    // Change color based on health
    float ratio = health / maxHealth;
    color = sf::Color(static_cast<sf::Uint8>(139 * ratio),
                      static_cast<sf::Uint8>(69 * ratio),
                      static_cast<sf::Uint8>(19 * ratio));
}