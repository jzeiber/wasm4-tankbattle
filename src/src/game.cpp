#include "game.h"
#include "wasm4.h"
#include "global.h"
#include "palette.h"
#include "textprinter.h"
#include "outputstringstream.h"
#include "font5x7.h"
#include "randommt.h"

Game::Game():m_ticks(0),m_nextpowerup(0)
{
	for(int i=0; i<MAX_PLAYERS; i++)
	{
		m_player[i].SetPlayerIndex(i);
	}
}

Game::~Game()
{

}

void Game::Update(const int ticks, Game *game)
{
	m_ticks+=ticks;

	ICollidable::CollisionResult colres[MAX_PROJECTILES+MAX_PLAYERS];

	int coli=0;
	for(int i=0; i<MAX_PROJECTILES; i++,coli++)
	{
		m_projectile[i].Update(ticks,game);
	}

	for(int i=0; i<MAX_PLAYERS; i++,coli++)
	{
		m_player[i].Update(ticks,game);
		if(PlayerActive(i)==false)
		{
			m_player[i].Reset();
		}
	}

	if(PlayersActive()>0 && m_nextpowerup<=m_ticks)
	{

		SpawnPowerup();
	}

	UpdateCollisions();

}

uint64_t Game::GetTicks() const
{
	return m_ticks;
}

void Game::Draw()
{
	OutputStringStream ostr;
	TextPrinter tp;
	tp.SetCustomFont(&Font5x7::Instance());

	if(PlayersActive()==0)
	{
		*DRAW_COLORS=PALETTE_BLACK;
		rect(0,0,SCREEN_SIZE,SCREEN_SIZE);
		tp.PrintCentered("Multiplayer Tank Battle",SCREEN_SIZE/2,(SCREEN_SIZE/2)-(tp.LineHeight()*2),25,PALETTE_LIGHTGREEN);
		tp.PrintCentered("Press Button 1 To Start",SCREEN_SIZE/2,(SCREEN_SIZE/2)+tp.LineHeight(),25,PALETTE_LIGHTGREEN);
	}
	else
	{

		for(int i=0; i<MAX_OBSTACLES; i++)
		{
			m_obstacle[i].Draw();
		}

		m_powerup.Draw();

		for(int i=0; i<MAX_PROJECTILES; i++)
		{
			m_projectile[i].Draw();
		}

		for(int i=0; i<MAX_PLAYERS; i++)
		{
			m_player[i].Draw();
		}

		*DRAW_COLORS=PALETTE_BLACK;
		rect(0,GameBoardHeight(),SCREEN_SIZE,SCREEN_SIZE-GameBoardHeight());

		for(int i=0; i<MAX_PLAYERS; i++)
		{
			if(PlayerActive(i)==true)
			{
				ostr.Clear();
				if(m_player[i].Powerup()==Powerup::POWERUP_NONE)
				{
					if(m_player[i].Alive()==true)
					{
						ostr << "Alive";
					}
					else if(m_player[i].TicksUntilSpawn()>0)
					{
						ostr << "Dead";
					}
					else
					{
						ostr << "Respawn";
					}
				}
				else
				{
					if(m_player[i].TicksUntilPowerLost()>120 || (m_player[i].TicksUntilPowerLost()/5)%3!=0)
					{
						switch(m_player[i].Powerup())
						{
						case Powerup::POWERUP_REVERSE_CONTROLS:
							ostr << "Rev";
							break;
						case Powerup::POWERUP_SLOW:
							ostr << "Slow";
							break;
						case Powerup::POWERUP_CLOAK:
							ostr << "Cloak";
							break;
						case Powerup::POWERUP_SPEED:
							ostr << "Fast";
							break;
						case Powerup::POWERUP_RAPID_FIRE:
							ostr << "MG";
							break;
						case Powerup::POWERUP_QUICK_SHOT:
							ostr << "Cannon";
							break;
						case Powerup::POWERUP_SHIELD:
							ostr << "Shield";
							break;
						}
					}
				}
				tp.PrintCentered(ostr.Buffer(),(i*40)+20,GameBoardHeight(),256,PALETTE_LIGHTGREEN);

				ostr.Clear();
				ostr << m_player[i].Score();
				tp.PrintCentered(ostr.Buffer(),(i*40)+20,GameBoardHeight()+tp.LineHeight(),256,PALETTE_LIGHTGREEN);
			}
		}
	}
}

bool Game::HandleInput(const Input *input)
{
	bool handled=false;

	for(int i=0; i<MAX_PLAYERS; i++)
	{
		m_player[i].HandleInput(input);
	}

	return handled;
}

uint16_t Game::GameBoardWidth() const
{
	return SCREEN_SIZE;
}

uint16_t Game::GameBoardHeight() const
{
	return SCREEN_SIZE-16;
}

uint8_t Game::PlayersAlive() const
{
	uint8_t count=0;
	for(int i=0; i<MAX_PLAYERS; i++)
	{
		if(m_player[i].Alive()==true)
		{
			count++;
		}
	}
	return count;
}

bool Game::PlayerActive(const uint8_t playerindex) const
{
	if(playerindex>=0 && playerindex<MAX_PLAYERS)
	{
		return (m_player[playerindex].LastActivity()>0 && (m_player[playerindex].LastActivity()+static_cast<uint64_t>(60*60)) > m_ticks);
	}
	return false;
}

uint8_t Game::PlayersActive() const
{
	uint8_t count=0;
	for(int i=0; i<MAX_PLAYERS; i++)
	{
		if(PlayerActive(i)==true)
		{
			count++;
		}
	}
	return count;
}

void Game::UpdateCollisions()
{

	ICollidable::CollisionResult colres[MAX_PROJECTILES+MAX_PLAYERS+1];

	int coli=0;
	for(int i=0; i<MAX_PROJECTILES; i++,coli++)
	{
		if(m_projectile[i].Alive()==true)
		{
			if(CheckCollision(m_projectile[i],m_projectile[i].Direction(),colres[coli]))
			{
				
			}
		}
	}

	for(int i=0; i<MAX_PLAYERS; i++,coli++)
	{
		if(m_player[i].Alive()==true)
		{
			if(CheckCollision(m_player[i],m_player[i].Direction(),colres[coli]))
			{
				
			}
		}
	}

	if(m_powerup.Alive()==true)
	{
		if(CheckCollision(m_powerup,DIR_UP,colres[coli]))
		{
			
		}
	}
	coli++;

	// must handle collisions after all collisions are checked
	coli=0;
	for(int i=0; i<MAX_PROJECTILES; i++,coli++)
	{
		if(colres[coli].m_type!=ICollidable::COLLISION_NONE)
		{
			m_projectile[i].Collide(colres[coli]);
			// +score of owner if destroyed tank
			if(colres[coli].m_type==ICollidable::COLLISION_TANK && m_projectile[i].Owner()!=colres[coli].m_player)
			{
				m_player[m_projectile[i].Owner()].AddScore(1);
			}
		}
	}

	for(int i=0; i<MAX_PLAYERS; i++,coli++)
	{
		if(colres[coli].m_type!=ICollidable::COLLISION_NONE)
		{
			m_player[i].Collide(colres[coli]);
		}
	}

	if(colres[coli].m_type!=ICollidable::COLLISION_NONE)
	{
		m_powerup.Collide(colres[coli]);
	}
	coli++;

}

bool Game::CheckCollision(ICollidable &obj, const uint8_t dir, ICollidable::CollisionResult &res)
{
	Rect r=obj.GetBoundingBox();
	res.m_type=ICollidable::COLLISION_NONE;
	res.m_player=0;
	res.m_continuemove=true;
	if(r.X1()<0 || r.X2()<0 || r.Y1()<0 || r.Y2()<0 || r.X1()>static_cast<float>(GameBoardWidth()) || r.X2()>static_cast<float>(GameBoardWidth()) || r.Y1()>static_cast<float>(GameBoardHeight()) || r.Y2()>static_cast<float>(GameBoardHeight()))
	{
		res.m_type=ICollidable::COLLISION_BOUNDS;
		res.m_newposition=r;
		res.m_continuemove=false;
		if(r.X1()<0)
		{
			res.m_newposition.Displace(-r.X1(),0);
		}
		if(r.Y1()<0)
		{
			res.m_newposition.Displace(0,-r.Y1());
		}
		if(r.X2()>=static_cast<float>(GameBoardWidth()))
		{
			res.m_newposition.Displace(static_cast<float>(GameBoardWidth())-r.X2(),0);
		}
		if(r.Y2()>=static_cast<float>(GameBoardHeight()))
		{
			res.m_newposition.Displace(0,static_cast<float>(GameBoardHeight())-r.Y2());
		}
	}

	{
		Rect r2=m_powerup.GetBoundingBox();
		if(m_powerup.Alive()==true && &m_powerup!=&obj && r2.Intersects(r))
		{
			res.m_type=ICollidable::COLLISION_POWERUP;
			res.m_param=m_powerup.Type();
			res.m_continuemove=true;
		}
	}

	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		Rect r2=m_projectile[i].GetBoundingBox();
		if(m_projectile[i].Alive()==true && &m_projectile[i]!=&obj && r2.Intersects(r))
		{
			res.m_type=ICollidable::COLLISION_PROJECTILE;
			res.m_player=m_projectile[i].Owner();
		}
	}

	for(int i=0; i<MAX_PLAYERS; i++)
	{
		Rect r2=m_player[i].GetBoundingBox();
		if(m_player[i].Alive()==true && &m_player[i]!=&obj && r2.Intersects(r))
		{
			res.m_type=ICollidable::COLLISION_TANK;
			res.m_player=i;
			res.m_newposition=r;
			res.m_continuemove=false;
			if(dir==DIR_LEFT && r.X1()<r2.X2())
			{
				if(r.X1()>=r2.X1())
				{
					res.m_newposition.Displace(r2.X2()-r.X1(),0);
				}
				if(r.X1()<r2.X1())
				{
					res.m_newposition.Displace(r2.X1()-r.X2(),0);
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_UP && r.Y1()<r2.Y2())
			{
				if(r.Y1()>=r2.Y1())
				{
					res.m_newposition.Displace(0,r2.Y2()-r.Y1());
				}
				if(r.Y1()<r2.Y1())
				{
					res.m_newposition.Displace(0,r2.Y1()-r.Y2());
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_RIGHT && r.X2()>r2.X1())
			{
				if(r.X2()<=r2.X2())
				{
					res.m_newposition.Displace(r2.X1()-r.X2(),0);
				}
				if(r.X2()>r2.X2())
				{
					res.m_newposition.Displace(r2.X2()-r.X1(),0);
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_DOWN && r.Y2()>r2.Y1())
			{
				if(r.Y2()<=r2.Y2())
				{
					res.m_newposition.Displace(0,r2.Y1()-r.Y2());
				}
				if(r.Y2()>r2.Y2())
				{
					res.m_newposition.Displace(0,r2.Y2()-r.Y1());
					res.m_continuemove=true;
				}
			}
		}
	}

	for(int i=0; i<MAX_OBSTACLES; i++)
	{
		Rect r2=m_obstacle[i].GetBoundingBox();
		if(m_obstacle[i].Alive()==true && &m_obstacle[i]!=&obj && r2.Intersects(r))
		{
			res.m_type=ICollidable::COLLISION_OBSTACLE;
			res.m_newposition=r;
			res.m_continuemove=false;
			if(dir==DIR_LEFT && r.X1()<r2.X2())
			{
				if(r.X1()>=r2.X1())
				{
					res.m_newposition.Displace(r2.X2()-r.X1(),0);
				}
				if(r.X1()<r2.X1())
				{
					res.m_newposition.Displace(r2.X1()-r.X2(),0);
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_UP && r.Y1()<r2.Y2())
			{
				if(r.Y1()>=r2.Y1())
				{
					res.m_newposition.Displace(0,r2.Y2()-r.Y1());
				}
				if(r.Y1()<r2.Y1())
				{
					res.m_newposition.Displace(0,r2.Y1()-r.Y2());
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_RIGHT && r.X2()>r2.X1())
			{
				if(r.X2()<=r2.X2())
				{
					res.m_newposition.Displace(r2.X1()-r.X2(),0);
				}
				if(r.X2()>r2.X2())
				{
					res.m_newposition.Displace(r2.X2()-r.X1(),0);
					res.m_continuemove=true;
				}
			}
			if(dir==DIR_DOWN && r.Y2()>r2.Y1())
			{
				if(r.Y2()<=r2.Y2())
				{
					res.m_newposition.Displace(0,r2.Y1()-r.Y2());
				}
				if(r.Y2()>r2.Y2())
				{
					res.m_newposition.Displace(0,r2.Y2()-r.Y1());
					res.m_continuemove=true;
				}
			}
		}
	}

	return res.m_type!=ICollidable::COLLISION_NONE;
}

void Game::SpawnPlayer(Player &p)
{
	RandomMT rand(m_ticks);

	if(PlayersAlive()==0)
	{
		// place obstacles before first player is spawned
		for(int i=0; i<MAX_OBSTACLES; i++)
		{
			m_obstacle[i].SetAlive(false);
		}
		int ob=(rand.Next()%5)+4;
		for(int i=0; i<ob; i++)
		{
			m_obstacle[i].SetAlive(true);
			bool placed=false;
			do
			{
				ICollidable::CollisionResult colres;
				m_obstacle[i].SetPosition(rand.NextDouble()*static_cast<float>(GameBoardWidth()),rand.NextDouble()*static_cast<float>(GameBoardHeight()));
				m_obstacle[i].SetSize(static_cast<uint64_t>((rand.NextDouble()*8.0)+8.0));
				if(CheckCollision(m_obstacle[i],DIR_UP,colres)==false)
				{
					placed=true;
				}
			} while (placed==false);
		}

		m_nextpowerup=m_ticks+(rand.Next()%120ULL)+120ULL;

	}

	bool placed=false;
	do
	{
		ICollidable::CollisionResult colres;
		float w=static_cast<float>(GameBoardWidth())*4.0/5.0;
		float h=static_cast<float>(GameBoardHeight())*4.0/5.0;
		float w2=w/2.0;
		float h2=h/2.0;
		p.SetDirection((rand.Next()%4)+1);
		p.SetPosition((rand.NextDouble()*w)+w2,(rand.NextDouble()*h)+h2);
		if(CheckCollision(p,p.Direction(),colres)==false)
		{
			placed=true;
			p.SetPowerup(Powerup::POWERUP_SHIELD,30);
		}
	} while (placed==false);

}

bool Game::FireProjectile(const Player &p, uint8_t projectiletype)
{
	for(int i=0; i<MAX_PROJECTILES; i++)
	{
		if(m_projectile[i].Alive()==false)
		{
			float x=p.X();
			float y=p.Y();
			switch(p.Direction())
			{
			case DIR_UP:
				y=p.GetBoundingBox().Y1()-1.0;
				break;
			case DIR_RIGHT:
				x=p.GetBoundingBox().X2()+1.0;
				break;
			case DIR_DOWN:
				y=p.GetBoundingBox().Y2()+1.0;
				break;
			case DIR_LEFT:
				x=p.GetBoundingBox().X1()-1.0;
				break;
			}
			m_projectile[i].SetPosition(x,y);
			m_projectile[i].SetOwner(p.PlayerIndex());
			m_projectile[i].SetDirection(p.Direction());
			m_projectile[i].SetAlive(true);
			m_projectile[i].SetType(projectiletype);

			return true;
		}
	}
	return false;
}

void Game::SpawnPowerup()
{
	RandomMT rand(m_ticks);
	bool placed=false;
	do
	{
		ICollidable::CollisionResult colres;
		m_powerup.SetAlive(true);
		m_powerup.SetPosition(rand.Next()%GameBoardWidth(),rand.Next()%GameBoardHeight());
		if(CheckCollision(m_powerup,DIR_UP,colres)==false)
		{
			m_powerup.SetType((rand.Next()%(Powerup::POWERUP_MAX-1)+1));
			placed=true;
		}
	} while (placed==false);
	m_nextpowerup=m_ticks+(rand.Next()%480ULL)+240ULL;
	
}
