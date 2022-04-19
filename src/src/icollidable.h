#pragma once

#include <stdint.h>

#include "rect.h"

class ICollidable
{
public:
    ICollidable()           { };
    virtual ~ICollidable()  { };

    enum CollisionResultType
    {
        COLLISION_NONE=0,
        COLLISION_BOUNDS=1,
        COLLISION_TANK=2,
        COLLISION_PROJECTILE=3,
        COLLISION_OBSTACLE=4,
        COLLISION_POWERUP=5
    };

    struct CollisionResult
    {
        uint8_t m_type{COLLISION_NONE};
        uint8_t m_player;
        Rect m_newposition;
        bool m_continuemove;
        uint8_t m_param;
    };

    virtual void Collide(const CollisionResult &res);

    virtual Rect GetBoundingBox() const=0;

};
