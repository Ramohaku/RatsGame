#pragma once
#include "Player.h"
#include "Debug.h"

#define DEBUG_LINES 0

class EnemyRat : public Rat
{
public:
	EnemyRat(const SpriteData& spriteData, const RatData& ratData,
		Player* playerPtr);
	EnemyRat(const EnemyRat&) = delete;
	EnemyRat(EnemyRat&&) = delete;
	virtual ~EnemyRat() {}

	virtual void onUpdate(float deltaTime) override;
	virtual void managePlayer(float deltaTime, float dist, float angle) = 0;
	void addTargetPoint(const Vec2f& point);
	void setNextIndexTimeMax(float nextIndexTimeMax) { m_nextIndexTimeMax = nextIndexTimeMax; }
	void setRayLengthMax(float rayLengthMax) { m_rayLengthMax = rayLengthMax; m_rayLength = rayLengthMax; }
protected:
	enum class MoveMode
	{
		Stand = 0,
		Walk,
		Run
	};

	// updates the collision rays and the move weights to follow the player
	void updateRaysForPlayer(float deltaTime, float dist, float angle);

	// updates the collision rays and the move weights to follow the current target
	void updateRaysForTarget(float deltaTime, float dist, float angle);

	// goes through all the colliding sprites (s_collidingSprites)
	// for each colliding sprite looks for intersections of collision rays from range <startIndex, endIndex>
	// if any collisions are found, decrease the move weight with the right index
	void updateCollisions(int startIndex, int endIndex);

	// updates the target collision ray (the direct one) according to the time m_colTimeP
	// sets its x and y (m_collisionRays[COL_RAYS]) based on the angle
	// sets its move weight (m_moveWeights[COL_RAYS]) to be > 2.0f
	// then updates the collisions (looks for intersections)
	void updateTargetCollisionRay(float deltaTime, float angle);

	// updates all the collision rays except the direct one according to the time m_colTime
	// returns true when it is not waiting
	// goes through the collision rays and sets their values to next values from the range <-pi, pi)
	bool updateCollisionRays(float deltaTime);

	// updates the move weights by increasing the values to be bigger if they are closer to the angle
	// !!! sets m_moving to true !!!
	void updateMoveWeights(float deltaTime, float angle);

	// updates the current target and takes control of the m_targetIndex if there are any targets
	// otherwise stands and set m_moving to false
	void updateTargets(float deltaTime);

	// moves directly to the player if nothing is blocking the direct path (m_moveWeights[COL_RAYS] > 2.0f)
	// otherwise tries to find a path to the player by increasing the ray length 
	void updateMoveToPlayer(float deltaTime);

	// does something when the player is near
	virtual void updateNearPlayer(float deltaTime, float dist, float angle) {}
protected:
	Player* m_playerPtr;
	Vec2f m_destPoint;
	MoveMode m_moveMode;

	std::array<Vec2f, COL_RAYS + 1> m_collisionRays;
	std::array<float, COL_RAYS + 1> m_moveWeights;
	float m_colTime = 0.0f;
	float m_colTimeMax = 0.5f;
	float m_colTimeP = 0.1f;
	float m_colTimeMaxP = 0.1f;
	float m_rayLength = 10.0f;
	float m_rayLengthMax = 10.0f;
	std::vector<Vec2f> m_targetPoints;
	size_t m_targetIndex = 0;
	float m_nextIndexTime = 0.0f;
	float m_nextIndexTimeMax = 10.0f;
	
	bool m_playerFollowing = false;
	bool m_playerLostGo = false;
	Vec2f m_playerLostPos;
	float m_lostGoTime = 0.0f;

	Vec2f m_headCenter;
};

class EnemyRatWatcher : public EnemyRat
{
public:
	EnemyRatWatcher(const SpriteData& spriteData, Player* playerPtr, std::vector<sf::SoundBuffer*>&& soundBuffers);
	EnemyRatWatcher(const EnemyRatWatcher&) = delete;
	EnemyRatWatcher(EnemyRatWatcher&&) = delete;
	~EnemyRatWatcher() {}

	void managePlayer(float deltaTime, float dist, float angle) override;
private:
	void updateNearPlayer(float deltaTime, float dist, float angle) override;
private:
	std::vector<sf::SoundBuffer*> m_soundBuffers;
	sf::Sound m_sound;
	sf::Sound m_sound2;
};

class EnemyRatSniffer : public EnemyRat
{
public:
	EnemyRatSniffer(const SpriteData& spriteData, Player* playerPtr);
	~EnemyRatSniffer() {}

	void managePlayer(float deltaTime, float dist, float angle) override;
private:
	void updateNearPlayer(float deltaTime, float dist, float angle) override;
private:
	float m_alarmTime = 0.0f;
	float m_alarmTimeMax = 3.0f;
};