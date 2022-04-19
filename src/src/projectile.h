#pragma once

#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "icollidable.h"
#include "rect.h"

class Projectile:public IUpdatable,public IDrawable,public ICollidable
{
public:
    Projectile();
    ~Projectile();

    enum ProjectileType
    {
        PROJECTILE_NONE=0,
        PROJECTILE_BULLET=1,
        PROJECTILE_HIGHVELOCITY=2
    };

	void Update(const int ticks, Game *game=nullptr);
	void Draw();

    void SetAlive(const bool alive);
    bool Alive() const;

    void Collide(const CollisionResult &res);
    Rect GetBoundingBox() const;

    uint8_t Direction() const;
    void SetDirection(const uint8_t dir);
    void SetPosition(const float x, const float y);

    void SetOwner(const uint8_t owner);
    uint8_t Owner() const;

    void SetType(const uint8_t projectiletype);

    float Speed() const;

private:
    bool m_alive;
    uint8_t m_owner;
    uint8_t m_type;
    uint8_t m_dir;
    float m_x;
    float m_y;

};