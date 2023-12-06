#include "stdafx.h"
#include "LevelScenes.h"


Level1::Level1(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader)
	: Scene(window, textures, maxTextureUnits, textureShader, shadowShader, uiShader)
{
    loadEntities("res/scenesData/level1.json");

    /*
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 0.2f }, Vec2f{ -16.0f, 0.0f }, 0.1f, .50f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -2.0f, 6.0f }, 0.1f, 3.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 9.0f, 1.0f }, 0.02f, 3.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 19.0f, 10.0f }, 0.1f, 2.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -12.0f, 33.0f }, 0.1f, 2.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 30.0f, 15.0f }, 0.1f, 2.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 40.0f, 9.0f }, 0.1f, 2.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -28.0f, -30.0f }, 0.1f, 2.0f });
    createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 26.0f, -20.0f }, 0.1f, 2.0f });

    createTextureSpriteBack(Vec2f{ 0.0f, 0.0f }, Vec2f{ 100.0f, 100.0f }, 0.0f, m_textures.at("TestBlock").get());
    createTextureBlockBack(Vec2f{ 7.0f, 17.0f }, Vec2f{ 6.0f, 3.0f }, 0.0f, m_textures.at("TestBlock2").get());
    createTextureBlockBack(Vec2f{ -12.0f, 12.0f }, Vec2f{ 1.0f, 2.0f }, 0.2f, m_textures.at("TestBlock2").get());
    createTextureBlockBack(Vec2f{ 11.0f, 8.0f }, Vec2f{ 1.0f, 3.0f }, -0.1f, m_textures.at("TestBlock2").get());
    createTextureBlockBack(Vec2f{ 11.0f, 3.0f }, Vec2f{ 1.0f, 3.0f }, 0.1f, m_textures.at("TestBlock2").get());
    createTextureBlockBack(Vec2f{ -3.0f, -5.0f }, Vec2f{ 8.0f, 1.0f }, 0.0f, m_textures.at("TestBlock2").get());
    createPlayer(Vec2f{ 0.0f, 0.0f }, m_textures.at("Player").get());
    auto rat1 = createEnemyRatSniffer(Vec2f{ -15.0f, 9.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_textures.at("Player").get());
    addTargetPoint(rat1, Vec2f{ 20.0f, 0.0f });
    addTargetPoint(rat1, Vec2f{ 18.0f, 20.0f });
    addTargetPoint(rat1, Vec2f{ 6.0f, 6.0f });
    createEnemyRatSniffer(Vec2f{ -20.0f, 9.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_textures.at("Player").get());
    createEnemyRatSniffer(Vec2f{ 30.0f, -4.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_textures.at("Player").get());
    */
}
