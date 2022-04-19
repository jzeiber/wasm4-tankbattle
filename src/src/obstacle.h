#pragma once

#include "idrawable.h"
#include "icollidable.h"

class Obstacle:public IDrawable,public ICollidable
{
public:
    Obstacle();
    ~Obstacle();

    void Draw();

    void Collide(const CollisionResult &res);

    Rect GetBoundingBox() const;

    void SetAlive(const bool alive);
    bool Alive() const;

    void SetPosition(const float x, const float y);
    void SetSize(const float size);

private:
    bool m_alive;
    float m_x;
    float m_y;
    float m_size;

};