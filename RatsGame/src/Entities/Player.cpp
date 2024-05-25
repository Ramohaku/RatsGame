#include "stdafx.h"
#include "Player.h"
#include "EdgeArray.h"

#define SQRT2 1.41421356237f
#define SQRT2_2 SQRT2 / 2.0f

Player::Player(const SpriteData& spriteData, Window* windowPtr)
	: Rat(spriteData, RatData{ 3.5f, 7.0f, 15.0f }),
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
	/*
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
*/


	double mouseX, mouseY;
	glfwGetCursorPos(m_windowPtr->getGlfwWindow(), &mouseX, &mouseY);

	//std::cout << mouseX << ' ' << mouseY << '\n';

	const float dx =
		static_cast<float>(mouseX)
		- static_cast<float>(m_windowPtr->getWidth()) / 2.0f;
	const float dy =
		static_cast<float>(mouseY)
		- static_cast<float>(m_windowPtr->getHeight()) / 2.0f;

	constexpr float sensitivity = 0.01f;
	m_destRotation += dx * deltaTime * sensitivity;

	glfwSetCursorPos(m_windowPtr->getGlfwWindow(), mouseX - dx * deltaTime * 8.0f, mouseY);

	correctAngleOneIter(m_destRotation);

	/*
	double mouseX, mouseY;
	glfwGetCursorPos(m_windowPtr->getGlfwWindow(), &mouseX, &mouseY);

	const Vec2f windowCenter {
		static_cast<float>(m_windowPtr->getWidth()) / 2.0f,
		static_cast<float>(m_windowPtr->getHeight()) / 2.0f
	};
	glfwSetCursorPos(m_windowPtr->getGlfwWindow(), windowCenter.x, windowCenter.y * 0.5f);

	const float mousePosX = static_cast<float>(mouseX);
	const float deltaX = mousePosX - m_lastMouseX;

	if ((mousePosX > windowCenter.x && deltaX > 0.0f) ||
		(mousePosX < windowCenter.x && deltaX < 0.0f))
	{
		m_destRotation += deltaX * deltaTime * 0.9f;

		if (m_destRotation < -PI_F)
			m_destRotation += 2.0f * PI_F;
		else if (m_destRotation > PI_F)
			m_destRotation -= 2.0f * PI_F;
	}

	m_lastMouseX = mousePosX;
	*/

	if (up)
	{
		tryMoveDest(deltaTime);
	}
	else if (down && !m_preDown)
	{
		m_destRotation = m_destRotation - PI_F;
		correctAngleTooSmall(m_destRotation);
	}
	
	if (left && !m_preLeft)
	{
		m_destRotation = m_destRotation - PI_F / 2.0f;
		correctAngleTooSmall(m_destRotation);
	}
	else if (right && !m_preRight)
	{
		m_destRotation = m_destRotation + PI_F / 2.0f;
		correctAngleTooBig(m_destRotation);
	}

	m_preDown = down;
	m_preLeft = left;
	m_preRight = right;



	/*const float turnFactor = 4.0f;

	if (left)
	{
		m_destRotation -= deltaTime * turnFactor;

		if (m_destRotation < -PI_F)
			m_destRotation += 2.0f * PI_F;
	}

	if (right)
	{
		m_destRotation += deltaTime * turnFactor;

		if (m_destRotation > PI_F)
			m_destRotation -= 2.0f * PI_F;
	}*/

#if 0
	if (upLeft && !right && !down)
	{
		moveIfPossible(TopLeft, deltaTime);
	}
	else if (upRight && !left && !down)
	{
		moveIfPossible(TopRight, deltaTime);
	}
	else if (downLeft && !right && !up)
	{
		moveIfPossible(BottomLeft, deltaTime);
	}
	else if (downRight && !left && !up)
	{
		moveIfPossible(BottomRight, deltaTime);
	}
	else if (left && !right)
	{
		moveIfPossible(Left, deltaTime);
	}
	else if (right && !left)
	{
		moveIfPossible(Right, deltaTime);
	}
	else if (up && !down)
	{
		moveIfPossible(Top, deltaTime);
	}
	else if (down && !up)
	{
		moveIfPossible(Bottom, deltaTime);
	}
#endif

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

Player::CanMove Player::canMoveDest(const Vec2f& destPoint) const
{
	for (Sprite* sprite : s_collidingSprites)
	{
		if (dynamic_cast<Rat*>(sprite))
			continue;

		EdgeArray edgeArray(m_spriteData.center, sprite);
		const auto minIndex = edgeArray.getMinIndex();

		const auto intersectionPos1 = edgeArray[minIndex].pos;

		const auto intersection1Pos2 = edgeArray[(minIndex + 1) % edgeArray.size()].pos;
		auto intersection1 = getIntersection(m_spriteData.center, destPoint, intersectionPos1, intersection1Pos2);
		if (intersection1)
		{
			const Vec2f segmentVec = intersection1Pos2 - intersectionPos1;
			const Vec2f playerVec = m_spriteData.center - intersectionPos1;
			const float cp = crossProduct(segmentVec, playerVec);

			if (cp >= 0.0f)
				continue;

			return CanMove{ false, getAngleBetweenLineAndSegment(-m_destRotation, intersectionPos1, intersection1Pos2), atan2(intersection1Pos2.y - intersectionPos1.y, intersection1Pos2.x - intersectionPos1.x) };
		}

		const auto intersection2Pos2 = edgeArray[(minIndex - 1) % edgeArray.size()].pos;
		auto intersection2 = getIntersection(m_spriteData.center, destPoint, intersectionPos1, intersection2Pos2);
		if (intersection2)
		{
			const Vec2f segmentVec = intersectionPos1 - intersection2Pos2;
			const Vec2f playerVec = m_spriteData.center - intersection2Pos2;
			const float cp = crossProduct(segmentVec, playerVec);

			if (cp >= 0.0f)
				continue;

			return CanMove{ false, getAngleBetweenLineAndSegment(-m_destRotation, intersection2Pos2, intersectionPos1), atan2(intersectionPos1.y - intersection2Pos2.y, intersectionPos1.x - intersection2Pos2.x) };
		}
	}

	return CanMove{ true };
}

bool Player::tryMoveDest(float deltaTime)
{
	const float angle = -m_destRotation;

	float moveCos = cos(angle);
	float moveSin = sin(angle);

	Vec2f destPoint = {
		m_spriteData.center.x + m_currentSpeed * deltaTime * moveCos,
		m_spriteData.center.y + m_currentSpeed * deltaTime * moveSin
	};

	auto canMove = canMoveDest(destPoint);
	if (canMove.isTrue)
	{
		m_spriteData.center = destPoint;
		m_moving = true;
		return true;
	}

	moveCos = cos(canMove.directionAngle);
	moveSin = sin(canMove.directionAngle);

	//const float speed =
	//	std::clamp(2.0f * (-2.0f * m_currentSpeed * canMove.collisionAngle / PI_F + m_currentSpeed),
	//	-m_currentSpeed, m_currentSpeed);

	const float base = canMove.collisionAngle - PI_F / 2.0f;
	const float speed =
		std::clamp(-32.0f * m_currentSpeed / (PI_F * PI_F * PI_F) * base * base * base,
			-m_currentSpeed, m_currentSpeed);

	destPoint = {
		m_spriteData.center.x + speed * deltaTime * moveCos,
		m_spriteData.center.y + speed * deltaTime * moveSin
	};

	canMove = canMoveDest(destPoint);
	if (canMove.isTrue)
	{
		m_spriteData.center = destPoint;
		m_moving = true;
		return true;
	}

	return false;
}

bool Player::tryMove(MoveDir moveDir, float deltaTime)
{
	const float angle = -m_destRotation
		- PI_F / 4.0f * static_cast<float>(moveDir) + PI_F / 4.0f;
	//std::cout << angle << '\n';

	const float moveCos = cos(angle);
	const float moveSin = sin(angle);

	const Vec2f destPoint = {
		m_spriteData.center.x + m_currentSpeed * deltaTime * moveCos,
		m_spriteData.center.y + m_currentSpeed * deltaTime * moveSin
	};

	if (canMoveTo(destPoint))
	{
		m_spriteData.center = destPoint;
		m_moving = true;
		return true;
	}
	return false;
}

void Player::moveIfPossible(MoveDir moveDir, float deltaTime)
{
	if (tryMove(moveDir, deltaTime))
		return;
	// 8 must be an unsigned int so that the expression is always positive
	if (tryMove(static_cast<MoveDir>((static_cast<int>(moveDir) - 1) % 8u), deltaTime))
		return;
	tryMove(static_cast<MoveDir>((static_cast<int>(moveDir) + 1) % 8u), deltaTime);
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
