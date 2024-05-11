#include "stdafx.h"
#include "EnemyRats.h"
#include "EdgeArray.h"

EnemyRat::EnemyRat(const SpriteData& spriteData, const RatData& ratData,
	Player* playerPtr)
	: Rat(spriteData, ratData),
	m_playerPtr(playerPtr),
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

	// the distance between the center and the head is equal to m_spriteData.halfSize.x
	m_headCenter.x = m_spriteData.center.x + m_spriteData.halfSize.x * cos(m_spriteData.rotation);
	m_headCenter.y = m_spriteData.center.y - m_spriteData.halfSize.x * sin(m_spriteData.rotation);
	dx = playerCenter.x - m_headCenter.x;
	dy = playerCenter.y - m_headCenter.y;
	dist = hypot(dx, dy);
	angle = atan2(dy, dx);

	managePlayer(deltaTime, dist, angle);

	Rat::onUpdate(deltaTime);
}

void EnemyRat::addTargetPoint(const Vec2f& point)
{
	m_targetPoints.push_back(point);
}

void EnemyRat::updateRaysForPlayer(float deltaTime, float dist, float angle)
{
	m_playerFollowing = true;
	m_playerLostGo = false;

	updateTargetCollisionRay(deltaTime, angle);

	if (!updateCollisionRays(deltaTime))
		return;

	updateCollisions(0, COL_RAYS - 1);
	
	updateMoveWeights(deltaTime, angle);

	// call the virtual function that decides what to do when player is near
	updateNearPlayer(deltaTime, dist, angle);

#if DEBUG_LINES
	for (int i = 0; i < COL_RAYS + 1; i++)
	{
		Line::s_debugLines[i].setColor(glm::vec3{ m_moveWeights[i] / 2.0f, 0.3f, 0.3f });
	}
#endif
}

void EnemyRat::updateRaysForTarget(float deltaTime, float dist, float angle)
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

void EnemyRat::updateCollisions(int startIndex, int endIndex)
{
	for (Sprite* sprite : s_collidingSprites)
	{
		if (sprite == this)
			continue;

		if (hypot(sprite->getCenter().x - m_spriteData.center.x, sprite->getCenter().y - m_spriteData.center.y) > m_rayLength + sprite->getRange())
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

void EnemyRat::updateTargetCollisionRay(float deltaTime, float angle)
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

bool EnemyRat::updateCollisionRays(float deltaTime)
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

void EnemyRat::updateMoveWeights(float deltaTime, float angle)
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
		m_moveWeights[(index + i) % COL_RAYS] *= 2.0f - static_cast<float>(i) * 0.4f;
	}
	m_moveWeights[index] *= 2.0f;

	m_moving = true;
}

void EnemyRat::updateTargets(float deltaTime)
{
	if (m_playerFollowing)
	{
		m_playerFollowing = false;
		m_playerLostGo = true;
		m_playerLostPos = m_playerPtr->getCenter();
		//std::cerr << "following\n";
		m_lostGoTime = 0.0f;
	}
	if (m_playerLostGo)
	{
		/*const float eyesX = m_spriteData.center.x + m_spriteData.halfSize.x * cos(m_spriteData.rotation);
		const float eyesY = m_spriteData.center.y - m_spriteData.halfSize.y * sin(m_spriteData.rotation);
		std::cerr << eyesX - m_spriteData.center.x << " " << eyesY - m_spriteData.center.y << '\n';
		const float dx = m_playerLostPos.x - eyesX;
		const float dy = m_playerLostPos.y - eyesY;
		const float playerLostDist = hypot(dx, dy);
		const float playerLostAngle = atan2(dy, dx);*/
		const float dx = m_playerLostPos.x - m_spriteData.center.x;
		const float dy = m_playerLostPos.y - m_spriteData.center.y;
		const float playerLostDist = hypot(dx, dy);
		const float playerLostAngle = atan2(dy, dx);
		//std::cerr << playerLostDist << '\n';
		if (playerLostDist < 2.0f)
		{
			m_playerLostGo = false;
			m_moving = false;
			m_moveMode = MoveMode::Stand;
		}
		else
		{
			updateRaysForTarget(deltaTime, playerLostDist, playerLostAngle);
		}
		//std::cerr << "lost go\n";

		m_lostGoTime += deltaTime;
		//std::cerr << m_lostGoTime << '\n';
		if (m_lostGoTime > 8.2f)
		{
			m_playerLostGo = false;
			m_moving = false;
			m_moveMode = MoveMode::Stand;
		}
	}
	else if (!m_targetPoints.empty())
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
	else
	{
		m_moving = false;
		m_moveMode = MoveMode::Stand;
		//std::cerr << "standing\n";
	}
}

void EnemyRat::updateMoveToPlayer(float deltaTime)
{
	if (m_moveWeights[COL_RAYS] > 2.0f)
	{
		m_colTimeMax = 0.5f;
		m_rayLength = m_rayLengthMax;
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

EnemyRatWatcher::EnemyRatWatcher(const SpriteData& spriteData, Player* playerPtr, std::vector<sf::SoundBuffer*>&& soundBuffers)
	: EnemyRat(spriteData, RatData{ 4.0f, 8.0f, 13.0f }, playerPtr),
	Sprite(spriteData), m_soundBuffers(soundBuffers)
{
	m_sound.setBuffer(*soundBuffers[randInt(0, soundBuffers.size() - 1)]);
	m_sound2.setBuffer(*soundBuffers[randInt(0, soundBuffers.size() - 1)]);

	m_sound.setPosition(-1.0f, 0.0f, 0.0f);
	m_sound2.setPosition(1.0f, 0.0f, 0.0f);
}

void EnemyRatWatcher::managePlayer(float deltaTime, float dist, float angle)
{
	float diffAngle = m_spriteData.rotation + angle;
	while (diffAngle > PI_F)
		diffAngle -= 2.0f * PI_F;
	while (diffAngle < -PI_F)
		diffAngle += 2.0f * PI_F;
	//const float func = 1.3f / (abs(0.07f * funcAngle) + .03f);
	float range = 8.0f;
	const float sightRange = PI_F / 2.0f + 0.6f;
	if (diffAngle > -sightRange && diffAngle < sightRange)
		range = 18.0f;

	if (dist < range && m_playerPtr->getLightStrength() > 0.05f)
	{
		bool playerLook = true;

		for (Sprite* sprite : s_collidingSprites)
		{
			if (sprite == this)
				continue;

			//if (hypot(sprite->getCenter().x - m_spriteData.center.x, sprite->getCenter().y - m_spriteData.center.y) > m_rayLength + sprite->getRange())
			//	continue;

			EdgeArray edgeArray(m_headCenter, sprite);
			const auto minIndex = edgeArray.getMinIndex();

			const auto& playerCenter = m_playerPtr->getCenter();
			const auto intersection1 = getIntersection(m_headCenter, playerCenter, edgeArray[minIndex].pos, edgeArray[(minIndex + 1) % edgeArray.size()].pos);
			const auto intersection2 = getIntersection(m_headCenter, playerCenter, edgeArray[minIndex].pos, edgeArray[(minIndex - 1) % edgeArray.size()].pos);

#if DEBUG_LINES
			Line::s_debugLines.emplace_back(glm::vec3{ m_headCenter.x, m_headCenter.y, 0.0f }, glm::vec3{ playerCenter.x, playerCenter.y, 0.0f });
#endif

			const float rayLength = hypot(m_headCenter.x - playerCenter.x, m_headCenter.y - playerCenter.y);
			//std::cerr << rayLength << '\n';
			const float minCollisionLength = 0.0f;
			if ((intersection1 && intersection1->offset01 * rayLength >= minCollisionLength) || (intersection2 && intersection2->offset01 * rayLength > minCollisionLength))
			{
				playerLook = false;
				break;
			}
		}

		if (playerLook)
		{
			if (m_sound.getStatus() != sf::Sound::Playing)
			{
				int randIndex = randInt(0, m_soundBuffers.size() - 1);
				m_sound.setBuffer(*m_soundBuffers[randIndex]);
				m_sound.play();
				m_sound2.setBuffer(*m_soundBuffers[randIndex]);
				m_sound2.play();
			}

			updateRaysForPlayer(deltaTime, dist, angle); // follow the player
		}
		else
		{
			updateTargets(deltaTime);
		}
	}
	else
	{
		updateTargets(deltaTime);
	}

	updateMoveToPlayer(deltaTime);
}

void EnemyRatWatcher::updateNearPlayer(float deltaTime, float dist, float angle)
{
	m_moveMode = MoveMode::Run;
}


EnemyRatSniffer::EnemyRatSniffer(const SpriteData& spriteData, Player* playerPtr)
	: EnemyRat(spriteData, RatData{ 3.5f, 7.0f, 12.0f }, playerPtr),
	Sprite(spriteData)
{
}

void EnemyRatSniffer::managePlayer(float deltaTime, float dist, float angle)
{
	const float minDist = 12.0f;
	if (dist < minDist || m_alarmTime > 0.0f)
		updateRaysForPlayer(deltaTime, dist, angle); // follow the player
	else
		updateTargets(deltaTime);

	updateMoveToPlayer(deltaTime);
}

void EnemyRatSniffer::updateNearPlayer(float deltaTime, float dist, float angle)
{
	const float minDist = 8.9f;
	if (m_alarmTime > 0.0f)
	{
		m_alarmTime -= m_colTimeMax;
		m_moveMode = MoveMode::Run;
	}
	else
	{
		m_moveMode = dist < minDist ? MoveMode::Run : MoveMode::Walk;
	}

	if (dist < minDist)
	{
		m_alarmTime = m_alarmTimeMax;
	}
}