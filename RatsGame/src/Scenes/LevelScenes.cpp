#include "stdafx.h"
#include "LevelScenes.h"


Level::Level(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction)
    : Scene(window, appSceneData, true), m_transitionFunction(transitionFunction)
{
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Level::onPauseStart()
{
    glfwSetInputMode(m_window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Level::onPauseEnd()
{
    glfwSetInputMode(m_window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(m_window->getGlfwWindow(), m_window->getWidth() / 2.0, m_window->getHeight() / 2.0);
}

Level1::Level1(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction)
	: Level(window, appSceneData, transitionFunction)
{
    loadEntities("res/scenesData/level1.json");

    auto& rat1 = *static_cast<EnemyRat*>(m_namedSprites["rat1"]);
    rat1.setRayLengthMax(5.0f);
    rat1.setNextIndexTimeMax(5.2f);
    rat1.addTargetPoint(Vec2f{ -28.0f, -3.0f });
    rat1.addTargetPoint(Vec2f{ -28.0f, 11.0f });

    m_closingDoor = m_namedSprites["closingDoor"];
}

void Level1::onUpdate(float deltaTime)
{
    Level::onUpdate(deltaTime);

    if (m_player->getCenter().x < -41.0f)
    {
        m_closingDoor->move(0.0f, -20.0f * deltaTime);
        if (m_closingDoor->getCenter().y <= -6.0f)
        {
            m_transitionFunction();
        }
    }
}

Level2::Level2(Window* window, AppSceneData& appSceneData, const std::function<void()>& transitionFunction)
    : Level(window, appSceneData, transitionFunction)
{
    loadEntities("res/scenesData/level2.json");
}

void Level2::onUpdate(float deltaTime)
{
    Level::onUpdate(deltaTime);
}
