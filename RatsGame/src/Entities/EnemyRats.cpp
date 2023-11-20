#include "stdafx.h"
#include "EnemyRats.h"
#include "EdgeArray.h"

EnemyRat::EnemyRat(const SpriteData& spriteData, const RatData& ratData,
	float sightDist, Player* playerPtr)
	: Rat(spriteData, ratData),
	m_sightDist(sightDist), m_playerPtr(playerPtr),
	Sprite(spriteData)
{
}

void EnemyRat::onUpdate(float deltaTime)
{
	const float minDist = 0.1f;

	float dx = m_destPoint.x - m_spriteData.center.x;
	float dy = m_destPoint.y - m_spriteData.center.y;
	float dist = hypot(dx, dy);
	float angle = atan2(dy, dx);

	m_destRotation = -angle;

	if (m_moving && dist > minDist)
	{
		m_currentSpeed =
			m_moveMode == MoveMode::Walk ? m_ratData.walkSpeed
			: (m_moveMode == MoveMode::Run ? m_ratData.runSpeed : 0.0f);

		m_spriteData.center.x += m_currentSpeed * cos(angle) * deltaTime;
		m_spriteData.center.y += m_currentSpeed * sin(angle) * deltaTime;
	}

	const Vec2f playerCenter = m_playerPtr->getCenter();

	dx = playerCenter.x - m_spriteData.center.x;
	dy = playerCenter.y - m_spriteData.center.y;
	dist = hypot(dx, dy);
	angle = atan2(dy, dx);

	managePlayer(deltaTime, dist, angle);

	Rat::onUpdate(deltaTime);
}

EnemyRatGuard::EnemyRatGuard(const SpriteData& spriteData, Player* playerPtr)
	: EnemyRat(spriteData, RatData{ 3.5f, 7.0f, 12.0f }, 10.0f, playerPtr),
	Sprite(spriteData)
{
}

void EnemyRatGuard::managePlayer(float deltaTime, float dist, float angle)
{
	if (dist < m_sightDist)
	{
		m_destPoint = m_playerPtr->getCenter();
		m_destRotation = -angle;
		m_moveMode = MoveMode::Run;
		if (m_alarmTime < 1.0f)
			m_alarmTime += deltaTime;
		else
			m_moving = true;
	}
	else
	{
		m_moveMode = MoveMode::Stand;
		m_spriteData.texPartIndex.x = 0.0f;
		if (m_alarmTime > 0.0f)
			m_alarmTime = std::max(m_alarmTime - deltaTime * 0.2f, 0.0f);
		else
			m_moving = false;
	}
}

EnemyRatSniffer::EnemyRatSniffer(const SpriteData& spriteData, Player* playerPtr)
	: EnemyRat(spriteData, RatData{ 3.5f, 7.0f, 13.0f }, 10.0f, playerPtr),
	Sprite(spriteData)
{
}

void EnemyRatSniffer::onUpdate(float deltaTime)
{
	EnemyRat::onUpdate(deltaTime);
}

void EnemyRatSniffer::managePlayer(float deltaTime, float dist, float angle)
{
	if (dist < 16.0f || m_alarmTime > 0.0f)
		updateRaysForPlayer(deltaTime, dist, angle); // follow the player
	else
	{
		if (!m_targetPoints.empty())
		{
			const float dx = m_targetPoints[m_targetIndex].x - m_spriteData.center.x;
			const float dy = m_targetPoints[m_targetIndex].y - m_spriteData.center.y;
			const float targetDist = hypot(dx, dy);
			const float targetAngle = atan2(dy, dx);
			updateRaysForTarget(deltaTime, targetDist, targetAngle);
			m_nextIndexTime += deltaTime;
			if (m_nextIndexTime > m_nextIndexTimeMax)
			{
				m_targetIndex = (m_targetIndex + 1) % m_targetPoints.size();
				m_nextIndexTime = 0.0f;
			}
		}
	}

	if (m_moveWeights[COL_RAYS] > 2.0f) // rat can run directly the target
	{
		m_colTimeMax = 0.5f;
		m_rayLength = 10.0f;
		m_destPoint = m_collisionRays[COL_RAYS];
	}
	else
	{
		if (m_colTimeMax < 0.5f)
			m_colTimeMax = 0.5f;
		else
			m_colTimeMax += 0.05f * deltaTime;
		m_rayLength += 0.2f * deltaTime;

		const auto index = std::distance(m_moveWeights.begin(), std::max_element(m_moveWeights.begin(), m_moveWeights.end()));
		if (m_moveWeights[index] > 0.1f)
			m_destPoint = m_collisionRays[index];
	}
}

void EnemyRatSniffer::addTargetPoint(const Vec2f& point)
{
	m_targetPoints.push_back(point);
}

void EnemyRatSniffer::updateRaysForPlayer(float deltaTime, float dist, float angle)
{
	updateTargetCollisionRay(deltaTime, angle);

	if (!updateCollisionRays(deltaTime))
		return;

	updateCollisions(0, COL_RAYS - 1);
	

	//if (/*dist < 14.0f || m_alarmTime > 0.0f*/)
	{
		updateMoveWeights(deltaTime, angle);

		if (m_alarmTime > 0.0f)
		{
			m_alarmTime -= m_colTimeMax;
			m_moveMode = MoveMode::Run;
		}
		else
		{
			m_moveMode = dist < 8.0f ? MoveMode::Run : MoveMode::Walk;
		}

		if (dist < 8.5f)
		{
			m_alarmTime = m_alarmTimeMax;
		}
	}
	//else
	//{
	//	m_moving = false;
	//	m_moveMode = MoveMode::Stand;
	//	m_colTimeMax = 0.5f;
	//	m_rayLength = 10.0f;
	//}

#if DEBUG_LINES
	for (int i = 0; i < COL_RAYS + 1; i++)
	{
		Line::s_debugLines[i].setColor(glm::vec3{ m_moveWeights[i] / 2.0f, 0.3f, 0.3f });
	}
#endif
}

void EnemyRatSniffer::updateRaysForTarget(float deltaTime, float dist, float angle)
{
	updateTargetCollisionRay(deltaTime, angle);

	if (!updateCollisionRays(deltaTime))
		return;

	updateCollisions(0, COL_RAYS - 1);


	if (dist > 2.0f)
	{
		updateMoveWeights(deltaTime, angle);

		m_moveMode = MoveMode::Walk;
	}
	else
	{
		m_moving = false;
		m_moveMode = MoveMode::Stand;
		//m_colTimeMax = 0.5f;
		//m_rayLength = 10.0f;
	}

#if DEBUG_LINES
	for (int i = 0; i < COL_RAYS + 1; i++)
	{
		Line::s_debugLines[i].setColor(glm::vec3{ m_moveWeights[i] / 2.0f, 0.3f, 0.3f });
	}
#endif
}

void EnemyRatSniffer::updateCollisions(int startIndex, int endIndex)
{
	for (Sprite* sprite : s_collidingSprites)
	{
		if (sprite == this)
			continue;

		EdgeArray edgeArray(m_spriteData.center, sprite);
		const auto minIndex = edgeArray.getMinIndex();

		const float collisionMult = dynamic_cast<EnemyRat*>(sprite) ? 0.05f : 0.0f;

		for (int i = startIndex; i <= endIndex; i++)
		{
			const auto intersection1 = getIntersection(m_spriteData.center, m_collisionRays[i], edgeArray[minIndex].pos, edgeArray[(minIndex + 1) % edgeArray.size()].pos);
			const auto intersection2 = getIntersection(m_spriteData.center, m_collisionRays[i], edgeArray[minIndex].pos, edgeArray[(minIndex - 1) % edgeArray.size()].pos);

#if DEBUG_LINES
			Line::s_debugLines.emplace_back(glm::vec3{ edgeArray[minIndex].pos.x, edgeArray[minIndex].pos.y, 0.0f }, glm::vec3{ edgeArray[(minIndex + 1) % edgeArray.size()].pos.x, edgeArray[(minIndex + 1) % edgeArray.size()].pos.y, 0.0f });
			Line::s_debugLines.emplace_back(glm::vec3{ edgeArray[minIndex].pos.x, edgeArray[minIndex].pos.y, 0.0f }, glm::vec3{ edgeArray[(minIndex - 1) % edgeArray.size()].pos.x, edgeArray[(minIndex - 1) % edgeArray.size()].pos.y, 0.0f });
#endif

			if (intersection1)
			{
				m_moveWeights[i] *= collisionMult;// 0.2f * intersection1->offset01;// *intersection1->offset01;
			}
			if (intersection2)
			{
				m_moveWeights[i] *= collisionMult;// 0.2f * intersection2->offset01;// *intersection2->offset01;
			}
		}
	}
}

void EnemyRatSniffer::updateTargetCollisionRay(float deltaTime, float angle)
{
	m_colTimeP += deltaTime;
	if (m_colTimeP < m_colTimeMaxP)
		return;

	m_colTimeP = 0.0f;

	m_collisionRays[COL_RAYS].x = m_spriteData.center.x + m_rayLength * cos(angle);
	m_collisionRays[COL_RAYS].y = m_spriteData.center.y + m_rayLength * sin(angle);
	m_moveWeights[COL_RAYS] = 2.1f;

#if DEBUG_LINES
	Line::s_debugLines.emplace_back(glm::vec3{ m_spriteData.center.x, m_spriteData.center.y, 0.0f }, glm::vec3{ m_collisionRays[COL_RAYS].x, m_collisionRays[COL_RAYS].y, 0.0f });
#endif

	updateCollisions(COL_RAYS, COL_RAYS);
}

bool EnemyRatSniffer::updateCollisionRays(float deltaTime)
{
	m_colTime += deltaTime;
	if (m_colTime < m_colTimeMax)
		return false;

	m_colTime = 0.0f;

	const float step = PI_F * 2.0f / static_cast<float>(COL_RAYS);
	float rayAngle = -PI_F;
	for (int i = 0; i < COL_RAYS; i++)
	{
		m_collisionRays[i].x = m_spriteData.center.x + m_rayLength * cos(rayAngle);
		m_collisionRays[i].y = m_spriteData.center.y + m_rayLength * sin(rayAngle);
		m_moveWeights[i] = 1.0f;
		rayAngle += step;

#if DEBUG_LINES
		Line::s_debugLines.emplace_back(glm::vec3{ m_spriteData.center.x, m_spriteData.center.y, 0.0f }, glm::vec3{ m_collisionRays[i].x, m_collisionRays[i].y, 0.0f });
#endif
	}

	return true;
}

void EnemyRatSniffer::updateMoveWeights(float deltaTime, float angle)
{
	std::array<float, COL_RAYS> rayLines;
	const float step = PI_F * 2.0f / static_cast<float>(COL_RAYS);
	float rayAngle = -PI_F;
	for (int i = 0; i < COL_RAYS; i++)
	{
		rayLines[i] = rayAngle;
		rayAngle += step;
	}

	size_t index = findClosest(rayLines, angle);
	for (int i = 0; i < COL_RAYS / 4.0f + 1; i++)
	{
		m_moveWeights[(index - i) % COL_RAYS] *= 2.0f - static_cast<float>(i) * 0.4f;
}
	m_moveWeights[index] *= 2.0f;
	for (int i = 0; i < COL_RAYS / 4.0f + 1; i++)
	{
		m_moveWeights[(index + i) % COL_RAYS] *= 2.0f - static_cast<float>(i) * 0.4f;
	}

	m_moving = true;
}
