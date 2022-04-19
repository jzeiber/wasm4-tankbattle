#include "projectile.h"
#include "global.h"
#include "palette.h"
#include "wasm4.h"

Projectile::Projectile():m_alive(false),m_owner(0),m_type(0),m_dir(0),m_x(0),m_y(0)
{

}

Projectile::~Projectile()
{

}

void Projectile::Update(const int ticks, Game *game)
{
    if(m_alive==true)
    {
        float dx=0;
        float dy=0;
        float speed=Speed();

        switch(m_dir)
        {
        case DIR_UP:
            dy-=speed;
            break;
        case DIR_RIGHT:
            dx+=speed;
            break;
        case DIR_DOWN:
            dy+=speed;
            break;
        case DIR_LEFT:
            dx-=speed;
            break;
        }

        m_x+=dx;
        m_y+=dy;
    }
}

void Projectile::Draw()
{
    if(m_alive)
    {
        *DRAW_COLORS=PALETTE_BLACK;
        rect(m_x-1.0,m_y-1.0,2,2);
    }
}

void Projectile::SetAlive(const bool alive)
{
    m_alive=alive;
}

bool Projectile::Alive() const
{
    return m_alive;
}

void Projectile::Collide(const CollisionResult &res)
{
    // collison with owner player - do nothing
    if(res.m_type==COLLISION_TANK && res.m_player==m_owner)
    {
        return;
    }

    // all other collisions kill projectile
    m_alive=false;
}

Rect Projectile::GetBoundingBox() const
{
    Rect r;
    r.Set(m_x-1,m_y-1,2,2);
    return r;
}

uint8_t Projectile::Direction() const
{
    return m_dir;
}

void Projectile::SetDirection(const uint8_t dir)
{
    m_dir=dir;
}

void Projectile::SetPosition(const float x, const float y)
{
    m_x=x;
    m_y=y;
}

void Projectile::SetOwner(const uint8_t owner)
{
    m_owner=owner;
}

uint8_t Projectile::Owner() const
{
    return m_owner;
}

void Projectile::SetType(const uint8_t projectiletype)
{
    m_type=projectiletype;
}

float Projectile::Speed() const
{
    if(m_type==PROJECTILE_BULLET)
    {
        return 1.8;
    }
    else if(m_type==PROJECTILE_HIGHVELOCITY)
    {
        return 2.4;
    }
    return 0.0;
}
