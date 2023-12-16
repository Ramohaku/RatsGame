#include "stdafx.h"
#include "Player.h"
#include "EdgeArray.h"

#define SQRT2 1.41421356237f
#define SQRT2_2 SQRT2 / 2.0f

/*
Player::Player(const Vec2f& center, const Vec2f& halfSize, float rotation,
	Texture* texturePtr,
	const Vec2f& texPartScale, const Vec2f& texPartIndex, GLFWwindow* windowPtr)
	: Character(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	TextureSprite(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	Sprite(center, halfSize, rotation, texturePtr, texPartScale, texPartIndex),
	m_moveIndices{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f, 5.0f, 6.0f, 7.0f, 8.0f, 7.0f, 6.0f, 5.0f },
	m_moveAnim{ m_moveIndices, 16, 25.0f }, m_windowPtr(windowPtr)
{
}

void Player::onUpdate(float deltaTime)
{
	Character::onUpdate(deltaTime);

	const float walkSpeed = 7.0f;
	const float sneakSpeed = 3.5f;
	const float runSpeed = 18.0f;

	bool sneak = glfwGetKey(m_windowPtr, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
	bool run = glfwGetKey(m_windowPtr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

	const float speed = sneak ? sneakSpeed : (run ? runSpeed : walkSpeed);
	m_moveAnim.speed = speed * 3.8f;

	bool left = glfwGetKey(m_windowPtr, GLFW_KEY_A) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right = glfwGetKey(m_windowPtr, GLFW_KEY_D) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool up = glfwGetKey(m_windowPtr, GLFW_KEY_W) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_UP) == GLFW_PRESS;
	bool down = glfwGetKey(m_windowPtr, GLFW_KEY_S) == GLFW_PRESS
		|| glfwGetKey(m_windowPtr, GLFW_KEY_DOWN) == GLFW_PRESS;

	bool upLeft = up && left;
	bool upRight = up && right;
	bool downLeft = down && left;
	bool downRight = down && right;

	bool moving = false;

	if (upLeft && !right && !down)
	{
		m_center.x -= speed * deltaTime * SQRT2_2;
		m_center.y += speed * deltaTime * SQRT2_2;
		moving = true;
		m_flipHorizontal = 1.0f;
		m_texPartIndex.y = 0.0f;
	}
	else if (upRight && !left && !down)
	{
		m_center.x += speed * deltaTime * SQRT2_2;
		m_center.y += speed * deltaTime * SQRT2_2;
		moving = true;
		m_flipHorizontal = -1.0f;
		m_texPartIndex.y = 0.0f;
	}
	else if (downLeft && !right && !up)
	{
		m_center.x -= speed * deltaTime * SQRT2_2;
		m_center.y -= speed * deltaTime * SQRT2_2;
		moving = true;
		m_flipHorizontal = 1.0f;
		m_texPartIndex.y = 1.0f;
	}
	else if (downRight && !left && !up)
	{
		m_center.x += speed * deltaTime * SQRT2_2;
		m_center.y -= speed * deltaTime * SQRT2_2;
		moving = true;
		m_flipHorizontal = -1.0f;
		m_texPartIndex.y = 1.0f;
	}
	else if (left && !right)
	{
		m_center.x -= speed * deltaTime;
		moving = true;
		m_flipHorizontal = 1.0f;
	}
	else if (right && !left)
	{
		m_center.x += speed * deltaTime;
		moving = true;
		m_flipHorizontal = -1.0f;
	}
	else if (up && !down)
	{
		m_center.y += speed * deltaTime;
		moving = true;
		m_texPartIndex.y = 0.0f;
	}
	else if (down && !up)
	{
		m_center.y -= speed * deltaTime;
		moving = true;
		m_texPartIndex.y = 1.0f;
	}

	if (moving)
	{
		if (!m_currentAnim)
			startAnimation(&m_moveAnim);
	}
	else
	{
		if (m_currentAnim)
		{
			stopAnimation();
			m_texPartIndex.x = 0.0f;
		}
	}
}
*/

Player::Player(const SpriteData& spriteData, Window* windowPtr)
	: Rat(spriteData, RatData{ 3.5f, 7.0f, 16.0f }),
	Sprite(spriteData),
	m_windowPtr(windowPtr)
{
}

void Player::onUpdate(float deltaTime)
{
	GLFWwindow* glfwWindow = m_windowPtr->getGlfwWindow();

	bool sneak = glfwGetKey(glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
	bool run = glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

	m_currentSpeed = sneak ? m_ratData.sneakSpeed : (run ? m_ratData.runSpeed : m_ratData.walkSpeed);
	//m_moveAnim.speed = speed * 3.8f;

	bool left = glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right = glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool up = glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS;
	bool down = glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS
		|| glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS;

	bool upLeft = up && left;
	bool upRight = up && right;
	bool downLeft = down && left;
	bool downRight = down && right;

	m_moving = false;

	if (upLeft && !right && !down)
	{
		moveToIfPossible(TopLeft, deltaTime);
	}
	else if (upRight && !left && !down)
	{
		moveToIfPossible(TopRight, deltaTime);
	}
	else if (downLeft && !right && !up)
	{
		moveToIfPossible(BottomLeft, deltaTime);
	}
	else if (downRight && !left && !up)
	{
		moveToIfPossible(BottomRight, deltaTime);
	}
	else if (left && !right)
	{
		moveToIfPossible(Left, deltaTime);
	}
	else if (right && !left)
	{
		moveToIfPossible(Right, deltaTime);
	}
	else if (up && !down)
	{
		moveToIfPossible(Top, deltaTime);
	}
	else if (down && !up)
	{
		moveToIfPossible(Bottom, deltaTime);
	}

	Rat::onUpdate(deltaTime);

	/*
	double mouseX, mouseY;
	glfwGetCursorPos(m_windowPtr->getGlfwWindow(), &mouseX, &mouseY);

	//std::cout << mouseX << ' ' << mouseY << '\n';

	const float dx =
		static_cast<float>(mouseX)
		- static_cast<float>(m_windowPtr->getWidth()) / 2.0f;
	const float dy =
		static_cast<float>(mouseY)
		- static_cast<float>(m_windowPtr->getHeight()) / 2.0f;

	m_nextRotation = atan2(dy, dx) + 3.1415f / 2.0f;
	if (m_nextRotation > 3.1415f)
		m_nextRotation -= 3.1415f * 2.0f;
	*/
}

bool Player::canMoveTo(const Vec2f& destPoint) const
{
	for (Sprite* sprite : s_collidingSprites)
	{
		if (dynamic_cast<Rat*>(sprite))
			continue;

		EdgeArray edgeArray(m_spriteData.center, sprite);
		const auto minIndex = edgeArray.getMinIndex();

		auto intersection1 = getIntersection(m_spriteData.center, destPoint, edgeArray[minIndex].pos, edgeArray[(minIndex + 1) % edgeArray.size()].pos);
		if (intersection1)
			return false;
		
		auto intersection2 = getIntersection(m_spriteData.center, destPoint, edgeArray[minIndex].pos, edgeArray[(minIndex - 1) % edgeArray.size()].pos);
		if (intersection2)
			return false;
	}

	return true;
}

bool Player::tryMoveTo(MoveDir moveDir, float deltaTime)
{
	switch (moveDir)
	{
		case Player::TopLeft:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x - m_currentSpeed * deltaTime * SQRT2_2,
				m_spriteData.center.y + m_currentSpeed * deltaTime * SQRT2_2
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center = destPoint;
				m_destRotation = PI_F * -0.75f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::Top:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x,
				m_spriteData.center.y + m_currentSpeed * deltaTime
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center.y = destPoint.y;
				m_destRotation = PI_F * -0.5f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::TopRight:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x + m_currentSpeed * deltaTime * SQRT2_2,
				m_spriteData.center.y + m_currentSpeed * deltaTime * SQRT2_2
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center = destPoint;
				m_destRotation = PI_F * -0.25f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::Right:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x + m_currentSpeed * deltaTime,
				m_spriteData.center.y
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center.x = destPoint.x;
				m_destRotation = PI_F * 0.0f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::BottomRight:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x + m_currentSpeed * deltaTime * SQRT2_2,
				m_spriteData.center.y - m_currentSpeed * deltaTime * SQRT2_2
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center = destPoint;
				m_destRotation = PI_F * 0.25f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::Bottom:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x,
				m_spriteData.center.y - m_currentSpeed * deltaTime
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center.y = destPoint.y;
				m_destRotation = PI_F * 0.5f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::BottomLeft:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x - m_currentSpeed * deltaTime * SQRT2_2,
				m_spriteData.center.y - m_currentSpeed * deltaTime * SQRT2_2
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center = destPoint;
				m_destRotation = PI_F * 0.75f;
				m_moving = true;
				return true;
			}
			return false;
		}
		case Player::Left:
		{
			const Vec2f destPoint = {
				m_spriteData.center.x - m_currentSpeed * deltaTime,
				m_spriteData.center.y
			};
			if (canMoveTo(destPoint))
			{
				m_spriteData.center.x = destPoint.x;
				m_destRotation = PI_F * 1.0f;
				m_moving = true;
				return true;
			}
			return false;
		}
	}

	return false;
}

void Player::moveToIfPossible(MoveDir moveDir, float deltaTime)
{
	if (tryMoveTo(moveDir, deltaTime))
		return;
	// 8 must be an unsigned int so that the expression is always positive
	if (tryMoveTo(static_cast<MoveDir>((static_cast<int>(moveDir) - 1) % 8u), deltaTime))
		return;
	tryMoveTo(static_cast<MoveDir>((static_cast<int>(moveDir) + 1) % 8u), deltaTime);
}
