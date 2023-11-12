#pragma once
#include "Player.h"
#include "Debug.h"

#define DEBUG_LINES 0

class EnemyRat : public Rat
{
public:
	EnemyRat(const SpriteData& spriteData, const RatData& ratData,
		float sightDist, Player* playerPtr);
	virtual ~EnemyRat() {}

	virtual void onUpdate(float deltaTime) override;
	virtual void managePlayer(float deltaTime, float dist, float angle) = 0;
protected:
	enum class MoveMode
	{
		Stand = 0,
		Walk,
		Run
	};

	Player* m_playerPtr;
	float m_sightDist;
	Vec2f m_destPoint;
	MoveMode m_moveMode;
};

class EnemyRatGuard : public EnemyRat
{
public:
	EnemyRatGuard(const SpriteData& spriteData, Player* playerPtr);
	~EnemyRatGuard() {}

	void managePlayer(float deltaTime, float dist, float angle) override;
private:
	float m_alarmTime = 0.0f;
};

class EnemyRatSniffer : public EnemyRat
{
public:
	EnemyRatSniffer(const SpriteData& spriteData, Player* playerPtr);
	~EnemyRatSniffer() {}

	void onUpdate(float deltaTime) override;
	void managePlayer(float deltaTime, float dist, float angle) override;
	void addTargetPoint(const Vec2f& point);
private:
	void updateRaysForPlayer(float deltaTime, float dist, float angle);
	void updateRaysForTarget(float deltaTime, float dist, float angle);
	void updateCollisions(int startIndex, int endIndex);
	void updateTargetCollisionRay(float deltaTime, float angle);
	bool updateCollisionRays(float deltaTime);
	void updateMoveWeights(float deltaTime, float angle);
private:
	std::array<Vec2f, COL_RAYS + 1> m_collisionRays;
	std::array<float, COL_RAYS + 1> m_moveWeights;
	float m_colTime = 0.0f;
	float m_colTimeMax = 0.5f;
	float m_colTimeP = 0.1f;
	float m_colTimeMaxP = 0.1f;
	float m_rayLength = 10.0f;
	float m_alarmTime = 0.0f;
	float m_alarmTimeMax = 3.0f;
	std::vector<Vec2f> m_targetPoints;
	size_t m_targetIndex = 0;
	float m_nextIndexTime = 0.0f;
	float m_nextIndexTimeMax = 10.0f;
};