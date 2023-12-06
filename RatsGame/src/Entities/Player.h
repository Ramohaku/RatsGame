#pragma once
#include "Rat.h"
#include "Window.h"

/*
class Player : public Character, public TextureSprite
{
public:
	Player(const Vec2f& center, const Vec2f& halfSize, float rotation,
		Texture* texturePtr,
		const Vec2f& texPartScale, const Vec2f& texPartIndex, GLFWwindow* windowPtr);
	~Player() {}

	void onUpdate(float deltaTime) override;
private:
	float m_moveIndices[16];
	Animation m_moveAnim;
	GLFWwindow* m_windowPtr;
};
*/

class Player : public Rat
{
public:
	Player(const SpriteData& spriteData, Window* windowPtr);
	Player(const Player&) = delete;
	Player(Player&&) = delete;
	~Player() {}

	void onUpdate(float deltaTime) override;
private:
	bool canMoveTo(const Vec2f& destPoint) const;
	
	enum MoveDir
	{
		// order is important
		TopLeft = 0, Top, TopRight,
		Right,
		BottomRight, Bottom, BottomLeft,
		Left
	};
	bool tryMoveTo(MoveDir moveDir, float deltaTime);
	void moveToIfPossible(MoveDir moveDir, float deltaTime);
private:
	Window* m_windowPtr;
};