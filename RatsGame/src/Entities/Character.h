#pragma once
#include "Entity.h"

struct Animation
{
	float* indices = nullptr;
	uint32_t size = 0;
	float speed = 0.0f;
};

class Character : virtual public Sprite
{
public:
	Character(const SpriteData& spriteData);
	Character(const Character&) = delete;
	Character(Character&&) = delete;
	virtual ~Character() {}

	virtual void onUpdate(float deltaTime) override;

	void startAnimation(Animation* animation, float startAnimTime = 0.0f);
	void stopAnimation();

	friend class CharacterShadow;
protected:
	// animates the indices horizontally
	virtual void animate(const Animation& animation, float deltaTime);
protected:
	float m_animTime = 0.0f;
	Animation* m_currentAnim = nullptr;
};


class CharacterShadow : public Character, public ShadowSprite
{
public:
	CharacterShadow(const SpriteData& spriteData, int lightIndex, Character* character);
	~CharacterShadow() {}

	void onUpdate(float deltaTime) override;
private:
	Character* m_character;
};



