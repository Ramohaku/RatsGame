#pragma once
#include "Player.h"
/*
class Enemy : public Character
{
public:
	Enemy(const Vec2f& center, const Vec2f& halfSize, float rotation,
		Texture* texturePtr,
		const Vec2f& texPartScale, const Vec2f& texPartIndex,
		Player* playerPtr,
		float sightDist, float walkSpeed, float runSpeed);
	virtual ~Enemy() {}

	virtual void onUpdate(float deltaTime) override;
	virtual void managePlayer(float dist, float angle) = 0;
protected:
	enum class MoveMode
	{
		Stand = 0,
		Walk,
		Run
	};

	Player* m_playerPtr;
	float m_walkSpeed;
	float m_runSpeed;
	float m_sightDist;
	Vec2f m_destPoint;
	MoveMode m_moveMode;
};

class GuardEnemy : public Enemy, public TextureSprite
{
public:
	GuardEnemy(const Vec2f& center, const Vec2f& halfSize, float rotation,
		Texture* texturePtr,
		const Vec2f& texPartScale, const Vec2f& texPartIndex,
		Player* playerPtr);
	virtual ~GuardEnemy() {}

	virtual void managePlayer(float dist, float angle) override;
protected:
	float m_walkIndices[16];
	float m_runIndices[16];
	Animation m_walkAnim;
	Animation m_runAnim;
	std::vector<Vec2f> m_movePoints;
};
*/