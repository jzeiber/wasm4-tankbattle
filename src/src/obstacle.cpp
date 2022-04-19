#include "obstacle.h"
#include "palette.h"
#include "wasm4.h"

Obstacle::Obstacle():m_alive(false),m_x(0),m_y(0),m_size(0)
{

}

Obstacle::~Obstacle()
{

}

void Obstacle::Draw()
{
    if(m_alive==true)
    {
        *DRAW_COLORS=PALETTE_LIGHTGREEN << 4 | PALETTE_BLACK;
        rect(m_x,m_y,m_size,m_size);
        *DRAW_COLORS=PALETTE_LIGHTGREEN;
        line(m_x,m_y,m_x+(m_size-1),m_y+(m_size-1));
        line(m_x+(m_size-1),m_y,m_x,m_y+(m_size-1));
    }
}

void Obstacle::Collide(const CollisionResult &res)
{

}

Rect Obstacle::GetBoundingBox() const
{
    return Rect(m_x,m_y,m_size,m_size);
}

void Obstacle::SetPosition(const float x, const float y)
{
    m_x=x;
    m_y=y;
}

void Obstacle::SetSize(const float size)
{
    m_size=size;
}

void Obstacle::SetAlive(const bool alive)
{
    m_alive=alive;
}

bool Obstacle::Alive() const
{
    return m_alive;
}
