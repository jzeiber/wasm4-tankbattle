#pragma once

#include "idrawable.h"
#include "icollidable.h"

class Powerup:public IDrawable,public ICollidable
{
public:
    Powerup();
    ~Powerup();

    enum PowerupType
    {
    POWERUP_NONE=0,
    POWERUP_REVERSE_CONTROLS=1,
    POWERUP_SLOW=2,
    POWERUP_CLOAK=3,
    POWERUP_SHIELD=4,
    POWERUP_SPEED=5,
    POWERUP_QUICK_SHOT=6,
    POWERUP_RAPID_FIRE=7,
    POWERUP_MAX
    };

    void Draw();

    void Collide(const CollisionResult &res);

    Rect GetBoundingBox() const;

    void SetAlive(const bool alive);
    bool Alive() const;

    void SetPosition(const float x, const float y);
    void SetSize(const float size);

    void SetType(const uint8_t poweruptype);
    uint8_t Type() const;

private:
    bool m_alive;
    uint8_t m_type;
    float m_x;
    float m_y;
    float m_size;

};