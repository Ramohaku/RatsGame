#include "stdafx.h"
#include "Character.h"

Character::Character(const SpriteData& spriteData)
	: Sprite(spriteData)
{
}

void Character::onUpdate(float deltaTime)
{
	if (m_currentAnim)
	{
		animate(*m_currentAnim, deltaTime);
	}
}

void Character::startAnimation(Animation* animation, float startAnimTime)
{
	m_currentAnim = animation;
	m_animTime = startAnimTime;
}

void Character::stopAnimation()
{
	m_currentAnim = nullptr;
}

void Character::animate(const Animation& animation, float deltaTime)
{
	m_animTime += animation.speed * deltaTime;
	const int index = static_cast<int>(m_animTime) % animation.size;

	m_spriteData.texPartIndex.x = animation.indices[index];
}


CharacterShadow::CharacterShadow(const SpriteData& spriteData, int lightIndex, Character* character)
	: Character(spriteData),
	ShadowSprite(spriteData, character, lightIndex),
	Sprite(spriteData),
	m_character(character)
{
}

void CharacterShadow::onUpdate(float deltaTime)
{
	if (m_character->m_currentAnim)
	{
		animate(*m_character->m_currentAnim, deltaTime);
	}
	else
	{
		stopAnimation();
		m_spriteData.texPartIndex.x = m_character->m_spriteData.texPartIndex.x;
	}
}

