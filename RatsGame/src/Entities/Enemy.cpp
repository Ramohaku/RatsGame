#include "stdafx.h"
#include "Enemy.h"
/*
Enemy::Enemy(const Vec2f& center, const Vec2f& halfSize, float rotation,
	Texture* texturePtr,
	const Vec2f& texPartScale, const Vec2f& texPartIndex,
	Player* playerPtr,
	float sightDist, float walkSpeed, float runSpeed)
	: Character(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	Sprite(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	m_playerPtr(playerPtr),
	m_sightDist(sightDist), m_walkSpeed(walkSpeed), m_runSpeed(runSpeed),
	m_destPoint(center), m_moveMode(MoveMode::Walk)
{
}

void Enemy::onUpdate(float deltaTime)
{
	Character::onUpdate(deltaTime);

	const float minDist = 0.1f;

	float dx = m_destPoint.x - m_center.x;
	float dy = m_destPoint.y - m_center.y;
	float dist = hypot(dx, dy);
	float angle = atan2(dy, dx);

	m_flipHorizontal = (angle > -3.1415f / 2.0f && angle < 3.1415f / 2.0f) ? -1.0f : 1.0f;
	
	if (dist > minDist)
	{
		const float speed =
			m_moveMode == MoveMode::Walk ? m_walkSpeed
			: (m_moveMode == MoveMode::Run ? m_runSpeed : 0.0f);

		m_center.x += speed * cos(angle) * deltaTime;
		m_center.y += speed * sin(angle) * deltaTime;
	}

	const Vec2f playerCenter = m_playerPtr->getCenter();

	dx = m_center.x - playerCenter.x;
	dy = m_center.y - playerCenter.y;
	dist = hypot(dx, dy);
	angle = atan2(dy, dx);

	managePlayer(dist, angle);
}

GuardEnemy::GuardEnemy(const Vec2f& center, const Vec2f& halfSize, float rotation,
	Texture* texturePtr,
	const Vec2f& texPartScale, const Vec2f& texPartIndex,
	Player* playerPtr)
	: Enemy(center, halfSize, rotation,
		texturePtr, texPartScale, texPartIndex, playerPtr,
		10.0f, 5.0f, 10.0f),
	Sprite(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	TextureSprite(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	m_walkIndices{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f, 5.0f, 6.0f, 7.0f, 8.0f, 7.0f, 6.0f, 5.0f },
	m_runIndices{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f, 5.0f, 6.0f, 7.0f, 8.0f, 7.0f, 6.0f, 5.0f },
	m_walkAnim{ m_walkIndices, 16, 22.0f }, m_runAnim{ m_runIndices, 16, 69.0f }
{
}

void GuardEnemy::managePlayer(float dist, float angle)
{
	if (dist < m_sightDist)
	{
		m_destPoint = m_playerPtr->getCenter();
		m_moveMode = MoveMode::Run;
		startAnimation(&m_runAnim, m_animTime);
	}
	else
	{
		stopAnimation();
		m_moveMode = MoveMode::Stand;
		m_texPartIndex.x = 0.0f;
	}
}
*/