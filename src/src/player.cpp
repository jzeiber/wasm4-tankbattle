#include "player.h"
#include "randommt.h"
#include "wasm4.h"
#include "game.h"
#include "palette.h"
#include "sprites.h"

Player::Player():m_playerindex(0),m_alive(false),m_spawn(false),m_shoot(false),m_dir(DIR_UP),m_moving(false),m_x(0),m_y(0),m_powerup(Powerup::POWERUP_NONE),m_score(0)/*,m_streak(0)*/,m_ticksuntilspawn(0),m_ticksuntilshoot(0),m_ticksuntilpowerlost(0),m_ticks(0),m_lastactivity(0)
{

}

Player::~Player()
{

}

uint8_t Player::PlayerIndex() const
{
    return m_playerindex;
}

void Player::SetPlayerIndex(const uint8_t index)
{
    m_playerindex=index;
}

uint16_t Player::Score() const
{
    return m_score;
}

bool Player::HandleInput(const Input *input)
{
    bool handled=false;

    if(m_alive==false && m_ticksuntilspawn==0 && input->GamepadButtonPress(m_playerindex+1,BUTTON_1)==true)
    {
        m_spawn=true;
        m_lastactivity=m_ticks;
        handled=true;
    }

    else if(m_alive==true)
    {
        if(input->GamepadButtonPress(m_playerindex+1,BUTTON_RIGHT)==true)
        {
            if(m_powerup==Powerup::POWERUP_REVERSE_CONTROLS)
            {
                m_dir=DIR_LEFT;
            }
            else
            {
                m_dir=DIR_RIGHT;
            }
            m_moving=true;
            m_lastactivity=m_ticks;
            handled=true;
        }
        else if(input->GamepadButtonPress(m_playerindex+1,BUTTON_DOWN)==true)
        {
            if(m_powerup==Powerup::POWERUP_REVERSE_CONTROLS)
            {
                m_dir=DIR_UP;
            }
            else
            {
                m_dir=DIR_DOWN;
            }
            m_moving=true;
            m_lastactivity=m_ticks;
            handled=true;
        }
        else if(input->GamepadButtonPress(m_playerindex+1,BUTTON_LEFT)==true)
        {
            if(m_powerup==Powerup::POWERUP_REVERSE_CONTROLS)
            {
                m_dir=DIR_RIGHT;
            }
            else
            {
                m_dir=DIR_LEFT;
            }
            m_moving=true;
            m_lastactivity=m_ticks;
            handled=true;
        }
        else if(input->GamepadButtonPress(m_playerindex+1,BUTTON_UP)==true)
        {
            if(m_powerup==Powerup::POWERUP_REVERSE_CONTROLS)
            {
                m_dir=DIR_DOWN;
            }
            else
            {
                m_dir=DIR_UP;
            }
            m_moving=true;
            m_lastactivity=m_ticks;
            handled=true;
        }

        if(m_ticksuntilshoot==0 && (input->GamepadButtonPress(m_playerindex+1,BUTTON_1)==true || (m_powerup==Powerup::POWERUP_RAPID_FIRE && input->GamepadButtonDown(m_playerindex+1,BUTTON_1))))
        {
            m_shoot=true;
            m_lastactivity=m_ticks;
            handled=true;
        }
        if(input->GamepadButtonPress(m_playerindex+1,BUTTON_2)==true)
        {
            m_moving=false;
            m_lastactivity=m_ticks;
            handled=true;
        }
    }

    return handled;
}

void Player::Update(const int ticks, Game *game)
{
    m_ticks=game->GetTicks();

    if(m_alive==false)
    {
        if(m_ticksuntilspawn>0)
        {
            if(ticks<m_ticksuntilspawn)
            {
                m_ticksuntilspawn-=ticks;
            }
            else
            {
                m_ticksuntilspawn=0;
            }
        }
    }

    if(m_spawn==true)
    {
        game->SpawnPlayer(*this);
        m_alive=true;
        m_spawn=false;
        m_moving=false;
        m_shoot=false;
        m_ticksuntilshoot=0;
        m_lastactivity=m_ticks;
    }

    if(m_alive==true)
    {
        if(m_ticksuntilshoot>0)
        {
            if(ticks>m_ticksuntilshoot)
            {
                m_ticksuntilshoot=0;
            }
            else
            {
                m_ticksuntilshoot-=ticks;
            }
        }

        if(m_ticksuntilpowerlost>0)
        {
            if(ticks>m_ticksuntilpowerlost)
            {
                m_ticksuntilpowerlost=0;
            }
            else
            {
                m_ticksuntilpowerlost-=ticks;
            }
            if(m_ticksuntilpowerlost==0)
            {
                m_powerup=Powerup::POWERUP_NONE;
            }
        }

        if(m_shoot==true)
        {
            uint8_t ptype=Projectile::PROJECTILE_BULLET;
            if(m_powerup==Powerup::POWERUP_QUICK_SHOT)
            {
                ptype=Projectile::PROJECTILE_HIGHVELOCITY;
            }
            game->FireProjectile(*this,ptype);
            m_shoot=false;
            if(m_powerup==Powerup::POWERUP_RAPID_FIRE)
            {
                m_ticksuntilshoot=20;
            }
            else
            {
                m_ticksuntilshoot=60;  // 1 seconds between shots
            }
            m_lastactivity=m_ticks;
        }

        if(m_moving==true)
        {
            float dx=0;
            float dy=0;
            float speed=Speed();
            switch(m_dir)
            {
            case DIR_UP:
                dy=-speed*static_cast<float>(ticks);
                break;
            case DIR_RIGHT:
                dx=speed*static_cast<float>(ticks);
                break;
            case DIR_DOWN:
                dy=speed*static_cast<float>(ticks);
                break;
            case DIR_LEFT:
                dx=-speed*static_cast<float>(ticks);
                break;
            }
            m_x+=dx;
            m_y+=dy;
        }
    }
}

void Player::Draw()
{
    if(Alive()==true)
    {
        uint8_t blitflags=sprite_flags;
        switch(m_dir)
        {
        case DIR_UP:
            break;
        case DIR_RIGHT:
            blitflags|=BLIT_FLIP_Y|BLIT_ROTATE;
            break;
        case DIR_DOWN:
            blitflags|=BLIT_FLIP_Y;
            break;
        case DIR_LEFT:
            blitflags|=BLIT_ROTATE;
            break;
        }

        if(m_powerup!=Powerup::POWERUP_CLOAK || (m_ticks%20ULL==0))
        {
            *DRAW_COLORS=PALETTE_BLACK << 4;
            blitSub(sprites,m_x-SpriteOffset(),m_y-SpriteOffset(),16,16,0,0,sprite_width,blitflags);
            *DRAW_COLORS=PALETTE_LIGHTGREEN << 4;
            blitSub(sprites,m_x-SpriteOffset(),m_y-SpriteOffset(),16,16,16,0,sprite_width,blitflags);
            *DRAW_COLORS=PALETTE_DARKGREEN << 4;
            blitSub(sprites,m_x-SpriteOffset(),m_y-SpriteOffset(),16,16,32,0,sprite_width,blitflags);

            if(m_moving==true)
            {
                const uint8_t tracks=((m_ticks/5)+(static_cast<uint64_t>(m_playerindex)*3ULL))%8;
                if(tracks>0)
                {
                    uint8_t xpos=32+(16*tracks);
                    *DRAW_COLORS=PALETTE_BLACK << 4;
                    blitSub(sprites,m_x-8,m_y-8,16,16,xpos,0,sprite_width,blitflags);
                }
            }
        }

        // if shield draw it
        if(m_powerup==Powerup::POWERUP_SHIELD)
        {
            *DRAW_COLORS=PALETTE_DARKGREEN << 4;
            oval(m_x-static_cast<float>(SpriteOffset())-4,m_y-static_cast<float>(SpriteOffset())-4,23,23);
        }

    }
}

bool Player::Alive() const
{
    return m_alive;
}

constexpr int16_t Player::SpriteOffset() const
{
    return 8;
}

void Player::Collide(const CollisionResult &res)
{
    // do nothing if we collide with our own projectile
    if(res.m_type==COLLISION_PROJECTILE && res.m_player==m_playerindex)
    {
        return;
    }

    if(res.m_type==COLLISION_PROJECTILE)
    {
        if(m_powerup==Powerup::POWERUP_SHIELD)
        {
            m_ticksuntilpowerlost=0;
        }
        else
        {
            m_alive=false;
            m_powerup=Powerup::POWERUP_NONE;
            m_ticksuntilspawn=120;  // 2 seconds
        }
        m_powerup=Powerup::POWERUP_NONE;
        return;
    }

    if(res.m_type==COLLISION_POWERUP)
    {
        RandomMT rand(m_ticks);
        m_ticksuntilpowerlost=360+(rand.Next()%480ULL);
        m_powerup=res.m_param;
        return;
    }

    if(m_moving==true)
    {
        // lose shield or cloak on collision with any other object
        if(m_powerup==Powerup::POWERUP_SHIELD || m_powerup==Powerup::POWERUP_CLOAK)
        {
            m_powerup=Powerup::POWERUP_NONE;
            m_ticksuntilpowerlost=0;
        }
        m_moving=res.m_continuemove;
        MoveTo(res.m_newposition);
    }
}

float Player::X() const
{
    return m_x;
}

float Player::Y() const
{
    return m_y;
}

Rect Player::GetBoundingBox() const
{
    float w=(m_dir==DIR_UP || m_dir==DIR_DOWN ? 14 : 16);
    float h=(m_dir==DIR_UP || m_dir==DIR_DOWN ? 16 : 14);
    float x=(m_dir==DIR_UP || m_dir==DIR_DOWN ? m_x-(static_cast<float>(SpriteOffset())-1.0) : m_x-static_cast<float>(SpriteOffset()));
    float y=(m_dir==DIR_UP || m_dir==DIR_DOWN ? m_y-static_cast<float>(SpriteOffset()) : m_y-(static_cast<float>(SpriteOffset())-1.0));
    return Rect(x,y,w,h);
}

void Player::MoveTo(const Rect &boundingbox)
{
    if(m_dir==DIR_UP || m_dir==DIR_DOWN)
    {
        m_x=boundingbox.X1()+(static_cast<float>(SpriteOffset())-1.0);
        m_y=boundingbox.Y1()+static_cast<float>(SpriteOffset());
    }
    else
    {
        m_x=boundingbox.X1()+static_cast<float>(SpriteOffset());
        m_y=boundingbox.Y1()+(static_cast<float>(SpriteOffset())-1.0);
    }
}

float Player::Speed() const
{
    if(m_alive && m_moving)
    {
        if(m_powerup==Powerup::POWERUP_SPEED)
        {
            return 1.2;
        }
        else if(m_powerup==Powerup::POWERUP_SLOW)
        {
            return 0.5;
        }
        else
        {
            return 0.8;
        }
    }
    return 0.0;
}

uint8_t Player::Direction() const
{
    return m_dir;
}

void Player::SetDirection(const uint8_t dir)
{
    m_dir=dir;
}

void Player::SetPosition(const float x, const float y)
{
    m_x=x;
    m_y=y;
}

uint64_t Player::LastActivity() const
{
    return m_lastactivity;
}

uint64_t Player::TicksUntilSpawn() const
{
    return m_ticksuntilspawn;
}

void Player::AddScore(uint64_t add)
{
    m_score+=add;
}

void Player::ClearScore()
{
    m_score=0;
}

void Player::Reset()
{
    m_score=0;
    m_alive=false;
    m_powerup=Powerup::POWERUP_NONE;
}

uint8_t Player::Powerup() const
{
    return m_powerup;
}

void Player::SetPowerup(const uint8_t powerup, const uint64_t ticksuntilpowerlost)
{
    m_powerup=powerup;
    m_ticksuntilpowerlost=ticksuntilpowerlost;
}

uint64_t Player::TicksUntilPowerLost() const
{
    return m_ticksuntilpowerlost;
}
