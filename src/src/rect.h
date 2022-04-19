#pragma once

class Rect
{
public:
    Rect();
    Rect(const float x, const float y, const float width, const float height);
    ~Rect();

    void Set(const float x, const float y, const float width, const float height);

    bool Intersects(const Rect &r2) const;

    float X1() const;
    float X2() const;
    float Y1() const;
    float Y2() const;
    float Width() const;
    float Height() const;

    void Displace(const float dx, const float dy);

private:
    float m_x;
    float m_y;
    float m_w;
    float m_h;
};
