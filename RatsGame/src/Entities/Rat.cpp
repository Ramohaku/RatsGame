#include "stdafx.h"
#include "Rat.h"

std::vector<Sprite*> Rat::s_collidingSprites;

Rat::Rat(const SpriteData& spriteData, const RatData& ratData)
	: Character(spriteData),
	TextureSprite(spriteData),
	Sprite(spriteData),
	m_ratData(ratData),
	m_startHalfSize(spriteData.halfSize),
	m_moveForwIndices{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f },
	m_moveBackIndices{ 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f },
	m_moveAnimRight{ m_moveForwIndices, 10, 25.0f },
	m_moveAnimLeft{ m_moveForwIndices, 10, 25.0f }
{
}

void Rat::onUpdate(float deltaTime)
{
	Character::onUpdate(deltaTime);

	float diff = m_spriteData.rotation - m_destRotation;
	if (diff < -PI_F)
		diff += 2.0f * PI_F;
	else if (diff > PI_F)
		diff -= 2.0f * PI_F;

	m_spriteData.rotation -= diff * deltaTime * 8.0f;

	if (m_spriteData.rotation < -PI_F)
		m_spriteData.rotation += 2.0f * PI_F;
	if (m_spriteData.rotation > PI_F)
		m_spriteData.rotation -= 2.0f * PI_F;

	if (diff > 0.0f)
	{

	}

	if (m_moving)
	{
		m_stretchTime += deltaTime * m_currentSpeed * 1.6f;
		m_spriteData.halfSize.x = m_startHalfSize.x + sin(m_stretchTime) * 0.11f;
	}
	else
	{
		m_stretchTime = 0.0f;
		m_spriteData.halfSize.x = m_startHalfSize.x;
	}
}
