#include "powerup.h"
#include "palette.h"
#include "wasm4.h"

Powerup::Powerup():m_alive(false),m_x(0),m_y(0),m_size(9)
{

}

Powerup::~Powerup()
{

}

void Powerup::Draw()
{
    if(m_alive==true)
    {
        *DRAW_COLORS=PALETTE_LIGHTGREEN << 4 | PALETTE_BLACK;
        rect(m_x,m_y,m_size,m_size);
        text("?",m_x+1,m_y+1);
    }
}

void Powerup::Collide(const CollisionResult &res)
{
    m_alive=false;
}

Rect Powerup::GetBoundingBox() const
{
    return Rect(m_x,m_y,m_size,m_size);
}

void Powerup::SetPosition(const float x, const float y)
{
    m_x=x;
    m_y=y;
}

void Powerup::SetSize(const float size)
{
    m_size=size;
}

void Powerup::SetAlive(const bool alive)
{
    m_alive=alive;
}

bool Powerup::Alive() const
{
    return m_alive;
}

void Powerup::SetType(const uint8_t poweruptype)
{
    m_type=poweruptype;
}

uint8_t Powerup::Type() const
{
    return m_type;
}
