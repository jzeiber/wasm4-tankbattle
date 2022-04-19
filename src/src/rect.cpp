#include "rect.h"

Rect::Rect():m_x(0),m_y(0),m_w(0),m_h(0)
{

}

Rect::Rect(const float x, const float y, const float width, const float height):m_x(x),m_y(y),m_w(width),m_h(height)
{

}

Rect::~Rect()
{

}

void Rect::Set(const float x, const float y, const float width, const float height)
{
    m_x=x;
    m_y=y;
    m_w=width;
    m_h=height;
}

bool Rect::Intersects(const Rect &r2) const
{
    if(X1()<r2.X2() && X2()>r2.X1() && Y1()<r2.Y2() && Y2()>r2.Y1())
    {
        return true;
    }
    return false;
}

float Rect::X1() const
{
    return m_x;
}

float Rect::X2() const
{
    return m_x+m_w;
}

float Rect::Y1() const
{
    return m_y;
}

float Rect::Y2() const
{
    return m_y+m_h;
}

float Rect::Width() const
{
    return m_w;
}

float Rect::Height() const
{
    return m_h;
}

void Rect::Displace(const float dx, const float dy)
{
    m_x+=dx;
    m_y+=dy;
}
