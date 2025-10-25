#pragma once

#include "GameObject.h"

// Base class for static objects
class StaticObject : public GameObject
{
protected:
    sf::Color color;

public:
    StaticObject(float x, float y, float w, float h, sf::Color col);
    virtual ~StaticObject() override {}

    void update(float dt) override;
    void render(sf::RenderWindow &window) override;
};

// Wall obstacle
class Wall : public StaticObject
{
public:
    Wall(float x, float y, float w, float h);
    virtual ~Wall() override {}
};

// Destructible object
class DestructibleObject : public StaticObject
{
protected:
    float health;
    float maxHealth;

public:
    DestructibleObject(float x, float y, float w, float h, float hp);
    virtual ~DestructibleObject() override {}

    void takeDamage(float damage);
    float getHealth() const { return health; }
};