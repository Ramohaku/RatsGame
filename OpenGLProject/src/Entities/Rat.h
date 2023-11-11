#pragma once
#include "Character.h"

struct RatData
{
	float sneakSpeed;
	float walkSpeed;
	float runSpeed;
};

class Rat : public Character, public TextureSprite
{
public:
	Rat(const SpriteData& spriteData, const RatData& ratData);
	virtual ~Rat() {}

	virtual void onUpdate(float deltaTime) override;
public:
	static std::vector<Sprite*> s_collidingSprites;
protected:
	RatData m_ratData;
	float m_currentSpeed = 0.0f;
	bool m_moving = false;
	Vec2f m_startHalfSize;
	float m_stretchTime = 0.0f;
	float m_moveForwIndices[10];
	float m_moveBackIndices[10];
	Animation m_moveAnimRight;
	Animation m_moveAnimLeft;
	float m_destRotation = 0.0f;
};