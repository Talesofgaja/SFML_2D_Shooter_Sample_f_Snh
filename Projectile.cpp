#include "Projectile.h"
#include <cmath>

Projectile::Projectile(float x, float y, float dx, float dy)
    : GameObject(x, y, 8, 8), speed(400.0f),
      color(sf::Color::Yellow), lifetime(3.0f), age(0)
{
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0)
    {
        velocity.x = (dx / length) * speed;
        velocity.y = (dy / length) * speed;
    }
    else
    {
        velocity = sf::Vector2<float>(0, 0);
    }
}

void Projectile::update(float dt)
{
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    age += dt;
    if (age >= lifetime)
    {
        isActive = false;
    }
}

void Projectile::render(sf::RenderWindow &window)
{
    sf::CircleShape shape(size.x / 2);
    shape.setPosition(position);
    shape.setFillColor(color);
    window.draw(shape);
}