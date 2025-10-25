#pragma once

#include "GameObject.h"

// Forward declaration
class Player;

// Base class for entities that can move
class Entity : public GameObject
{
protected:
    sf::Vector2<float> velocity;
    float speed;
    sf::Color color;

public:
    Entity(float x, float y, float w, float h, float spd, sf::Color col);
    virtual ~Entity() override {} // Virtual destructor is good practice

    virtual void move(float dx, float dy, float dt);
    virtual void update(float dt) override;
    virtual void render(sf::RenderWindow &window) override;
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
    Player(float x, float y);
    virtual ~Player() override {}

    void handleInput(float dt);
    void update(float dt) override;
    bool tryShoot();
    void takeDamage(float damage);
    float getHealth() const { return health; }
};

// Enemy with basic AI
class Enemy : public Entity
{
private:
    float detectionRange;
    Player *targetPlayer;

public:
    Enemy(float x, float y, Player *player);
    virtual ~Enemy() override {}

    void update(float dt) override;
};