#pragma once

#include "GameObject.h"

class Projectile : public GameObject
{
private:
    sf::Vector2<float> velocity;
    float speed;
    sf::Color color;
    float lifetime;
    float age;

public:
    Projectile(float x, float y, float dx, float dy);
    virtual ~Projectile() override {}

    void update(float dt) override;
    void render(sf::RenderWindow &window) override;
};