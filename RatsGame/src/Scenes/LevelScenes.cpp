#include "stdafx.h"
#include "LevelScenes.h"


Level::Level(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader, const std::function<void()>& transitionFunction)
    : Scene(window, textures, maxTextureUnits, textureShader, shadowShader, uiShader), m_transitionFunction(transitionFunction)
{
}

Level1::Level1(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader, const std::function<void()>& transitionFunction)
	: Level(window, textures, maxTextureUnits, textureShader, shadowShader, uiShader, transitionFunction)
{
    loadEntities("res/scenesData/level1.json");

    auto& rat1 = *static_cast<EnemyRat*>(m_namedSprites["rat1"]);
    rat1.setRayLengthMax(5.0f);
    rat1.setNextIndexTimeMax(5.2f);
    rat1.addTargetPoint(Vec2f{ -28.0f, -3.0f });
    rat1.addTargetPoint(Vec2f{ -28.0f, 11.0f });
}

void Level1::onUpdate(float deltaTime)
{
    Level::onUpdate(deltaTime);

    if (m_player->getCenter().x < -41.0f)
    {
        m_transitionFunction();
    }
}

Level2::Level2(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader, const std::function<void()>& transitionFunction)
    : Level(window, textures, maxTextureUnits, textureShader, shadowShader, uiShader, transitionFunction)
{
    loadEntities("res/scenesData/level2.json");
}

void Level2::onUpdate(float deltaTime)
{
    Level::onUpdate(deltaTime);
}
