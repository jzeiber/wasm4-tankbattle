#pragma once

#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "icollidable.h"
#include "rect.h"

class Game;

class Player:public IUpdatable,public IDrawable,public IInputHandler,public ICollidable
{
public:
    Player();
    ~Player();

	bool HandleInput(const Input *input);
	void Update(const int ticks, Game *game=nullptr);
	void Draw();

    uint8_t PlayerIndex() const;
    void SetPlayerIndex(const uint8_t index);

    bool Alive() const;
    uint16_t Score() const;
    void AddScore(uint64_t add);
    void ClearScore();
    void Reset();

    constexpr int16_t SpriteOffset() const;

    void Collide(const CollisionResult &res);
    Rect GetBoundingBox() const;
    float X() const;
    float Y() const;

    float Speed() const;
    uint8_t Direction() const;
    void SetDirection(const uint8_t dir);
    void SetPosition(const float x, const float y);

    uint64_t LastActivity() const;
    uint64_t TicksUntilSpawn() const;
    uint64_t TicksUntilPowerLost() const;

    uint8_t Powerup() const;
    void SetPowerup(const uint8_t powerup, const uint64_t ticksuntilpowerlost);

private:

    uint8_t m_playerindex;
    bool m_alive;
    bool m_spawn;
    bool m_shoot;
    uint8_t m_dir;
    bool m_moving;
    float m_x;
    float m_y;
    uint8_t m_powerup;
    uint64_t m_score;
    //uint64_t m_streak;
    uint64_t m_ticksuntilspawn;
    uint64_t m_ticksuntilshoot;
    uint64_t m_ticksuntilpowerlost;
    uint64_t m_ticks;
    uint64_t m_lastactivity;

    void MoveTo(const Rect &boundingbox);

};
