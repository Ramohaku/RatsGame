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

	inline void setLightStrength(float lightStrength) { m_lightStrength = lightStrength; }
	inline float getLightStrength() const { return m_lightStrength; }
private:
	bool canMoveTo(const Vec2f& destPoint) const;

	struct CanMove
	{
		bool isTrue = false;
		float collisionAngle = 0.0f;
		float directionAngle = 0.0f;
	};

	CanMove canMoveDest(const Vec2f& destPoint) const;
	
	enum MoveDir
	{
		// order is important
		TopLeft = 0, Top, TopRight,
		Right,
		BottomRight, Bottom, BottomLeft,
		Left
	};
	bool tryMoveDest(float deltaTime);
	//void moveDestIfPossible(float deltaTime);

	bool tryMove(MoveDir moveDir, float deltaTime);
	void moveIfPossible(MoveDir moveDir, float deltaTime);
	bool tryMoveTo(MoveDir moveDir, float deltaTime);
	void moveToIfPossible(MoveDir moveDir, float deltaTime);
private:
	Window* m_windowPtr;
	float m_lightStrength;

	float m_lastMouseX = 0.0f;
	bool m_preDown = false;
	bool m_preLeft = false;
	bool m_preRight = false;
};