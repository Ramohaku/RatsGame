#pragma once
#include "Scene.h"

class Level : public Scene
{
public:
	Level(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction);
	virtual ~Level() {}
protected:
	std::function<void()> m_transitionFunction;
};

class Level1 : public Level
{
public:
	Level1(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction);
	~Level1() {}

	void onUpdate(float deltaTime) override;
private:
	TextureSprite* m_closingDoor = nullptr;
};

class Level2 : public Level
{
public:
	Level2(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction);
	~Level2() {}

	void onUpdate(float deltaTime) override;
};