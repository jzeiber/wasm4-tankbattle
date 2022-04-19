#pragma once

#include <stdint.h>

#include "global.h"
#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "player.h"
#include "projectile.h"
#include "obstacle.h"
#include "powerup.h"

class Game:public IUpdatable,public IDrawable,public IInputHandler
{
public:
	Game();
	~Game();
	
	bool HandleInput(const Input *input);
	void Update(const int ticks, Game *game=nullptr);
	void Draw();

	uint64_t GetTicks() const;

	uint16_t GameBoardWidth() const;
	uint16_t GameBoardHeight() const;

	void SpawnPlayer(Player &p);
	bool FireProjectile(const Player &p, uint8_t projectiletype);
	void SpawnPowerup();

private:

	uint64_t m_ticks;
	uint64_t m_nextpowerup;
	Player m_player[MAX_PLAYERS];
	Projectile m_projectile[MAX_PROJECTILES];
	Obstacle m_obstacle[MAX_OBSTACLES];
	Powerup m_powerup;

	bool PlayerActive(const uint8_t playerindex) const;
	uint8_t PlayersAlive() const;
	uint8_t PlayersActive() const;

	void UpdateCollisions();
	bool CheckCollision(ICollidable &obj, const uint8_t dir, ICollidable::CollisionResult &res);

};
