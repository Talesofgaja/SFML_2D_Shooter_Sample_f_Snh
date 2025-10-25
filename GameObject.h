#pragma once

// SFML 3.x uses a different header structure
#include <SFML/Graphics/Graphics.hpp>
#include <memory>
#include <vector>
 
// Abstract base class for all game objects
class GameObject
{
protected:
    sf::Vector2<float> position;
    sf::Vector2<float> size;
    bool isActive;

public:
    GameObject(float x, float y, float w, float h);
    virtual ~GameObject();

    // Pure virtual functions (must be implemented by derived classes)
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow &window) = 0;

    // SFML 3.x: sf::FloatRect is now sf::Rect<float>
    virtual sf::Rect<float> getBounds() const
    {
        return sf::Rect<float>(position, size);
    }

    bool getActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    sf::Vector2<float> getPosition() const { return position; }

    // Added to allow collision response to update position
    void setPosition(const sf::Vector2<float> &newPos) { position = newPos; }
};