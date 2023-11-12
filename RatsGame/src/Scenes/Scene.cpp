#include "stdafx.h"
#include "Scene.h"
#include "Debug.h"

bool Scene::s_skip = false;

Scene::Scene(Window* window, const UpdateFunc& updateFunc, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader)
    : m_window(window), m_updateFunc(updateFunc),
    m_maxTextureUnits(maxTextureUnits),
    m_textureShader(textureShader), m_shadowShader(shadowShader), m_uiShader(uiShader)
{
    m_textureVertexArraysBack.emplace_back(4);
    m_shadowVertexArraysMiddle.emplace_back(4);
    m_textureVertexArraysMiddle.emplace_back(4);
    m_shadowVertexArraysFront.emplace_back(4);
    m_textureVertexArraysFront.emplace_back(4);
    m_uiVertexArrays.emplace_back(4);

    const glm::mat4 proj = glm::ortho(0.0f,
        static_cast<float>(m_window->getWidth()), 0.0f,
        static_cast<float>(m_window->getHeight()), -1.0f, 1.0f
    );

    const glm::mat4 view = glm::translate(glm::mat4(1.0f),
        glm::vec3(
            static_cast<float>(m_window->getWidth()) / 2.0f,
            static_cast<float>(m_window->getHeight()) / 2.0f,
            0.0f));

    const glm::mat4 mvp = proj * view;

    m_uiShader->bind();
    m_uiShader->setUniformMat4f("u_MVP", mvp);
}

void Scene::onUpdate(float deltaTime)
{
    if (m_lights.size() > 0)
    {
        static float time = 0.0f;
        time += deltaTime;
        if (time > 2.0f)
        {
            time = 0.0f;
            m_lights[1].active = !m_lights[1].active;
        }
    }

    for (auto& sprite : m_textureSpritesBack)
        sprite->onUpdate(deltaTime);

    for (auto& sprite : m_shadowSpritesMiddle)
    {
        sprite->onUpdate(deltaTime);
        sprite->setLightActive(m_lights[sprite->getLightIndex()].active);
    }

    for (auto& sprite : m_textureSpritesMiddle)
        sprite->onUpdate(deltaTime);

    for (auto& sprite : m_shadowSpritesFront)
    {
        sprite->onUpdate(deltaTime);
        sprite->setLightActive(m_lights[sprite->getLightIndex()].active);
    }

    for (auto& sprite : m_textureSpritesFront)
        sprite->onUpdate(deltaTime);

    for (auto& sprite : m_uiSprites)
    {
        sprite->onUpdate(deltaTime);

        // skip the update if the scene is deleted
        if (s_skip)
        {
            s_skip = false;
            return;
        }
    }

    if (m_updateFunc)
        m_updateFunc(deltaTime);

    //static float rotation = 0.0f;
    //rotation += 0.1 * deltaTime;
    //m_textureSpritesBack[1]->setRotation(rotation);


    //static float angle = 0.0f;
    //angle += deltaTime * 3.0f;
    //m_lights[1].center = Vec2f{ -2.0f + 5.0f * sin(angle), 6.0f };


    if (m_player)
        m_window->setCenter(m_player->getCenter());

    std::vector<Vec4f> lightsColors;
    std::vector<Vec4f> lightsData;
    for (const auto& light : m_lights)
    {
        lightsColors.push_back(Vec4f{ light.color.r, light.color.g, light.color.b, static_cast<float>(light.active) });
        lightsData.push_back(Vec4f{ light.center.x, light.center.y, light.vanish, light.distance });
    }

    for (auto& shadowSprite : m_shadowSpritesMiddle)
        shadowSprite->updateLights(lightsData);

    for (auto& shadowSprite : m_shadowSpritesFront)
        shadowSprite->updateLights(lightsData);

    const glm::mat4 mvp = m_window->getProj() * m_window->getView();
    const int lightsCount = lightsColors.size();

    m_textureShader->bind();
    m_textureShader->setUniformMat4f("u_MVP", mvp);
    m_textureShader->setUniform1i("u_LightsCount", lightsCount);
    m_textureShader->setUniform4fv("u_LightsColors", lightsCount, lightsColors.data());
    m_textureShader->setUniform4fv("u_LightsData", lightsCount, lightsData.data());

    m_shadowShader->bind();
    m_shadowShader->setUniformMat4f("u_MVP", mvp);
    m_shadowShader->setUniform1i("u_LightsCount", lightsCount);
    m_shadowShader->setUniform4fv("u_LightsData", lightsCount, lightsData.data());
}

void Scene::onRender()
{
    renderSprites(m_textureVertexArraysBack, m_textureSpritesBack, m_textureShader);
    renderSprites(m_shadowVertexArraysMiddle, m_shadowSpritesMiddle, m_shadowShader);
    renderSprites(m_textureVertexArraysMiddle, m_textureSpritesMiddle, m_textureShader);
    renderSprites(m_shadowVertexArraysFront, m_shadowSpritesFront, m_shadowShader);
    renderSprites(m_textureVertexArraysFront, m_textureSpritesFront, m_textureShader);
    renderSprites(m_uiVertexArrays, m_uiSprites, m_uiShader);
}

void Scene::createLight(const Light& light)
{
    m_lights.push_back(light);
}

Player* Scene::createPlayer(const Vec2f& center, Texture* texture)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = Vec2f{ 2.0f, 1.0f };
    spriteData.texturePtr = texture;
    spriteData.texPartScale = Vec2f{ 1.0f / 2.0f, 1.0f / 10.0f };
    spriteData.texPartIndex = Vec2f{ 1.0f, 9.0f };

    std::unique_ptr<Player> player = std::make_unique<Player>(spriteData, m_window);
    m_player = player.get();

    for (int i = 0; i < m_lights.size(); i++)
        m_shadowSpritesFront.push_back(std::make_unique<CharacterShadow>(spriteData, i, m_player));
    m_textureSpritesFront.push_back(std::move(player));

    return m_player;
}

TextureSprite* Scene::createTextureSpriteBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.rotation = rotation;
    spriteData.texturePtr = texture;

    auto entity = std::make_unique<TextureSprite>(spriteData);
    auto* ePtr = entity.get();
    m_textureSpritesBack.push_back(std::move(entity));

    return ePtr;
}

TextureSprite* Scene::createTextureBlockBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.rotation = rotation;
    spriteData.texturePtr = texture;
    spriteData.blocking = true;

    auto entity = std::make_unique<TextureSprite>(spriteData);
    auto* ePtr = entity.get();
    for (int i = 0; i < m_lights.size(); i++)
        m_shadowSpritesMiddle.push_back(std::make_unique<ShadowSprite>(spriteData, ePtr, i));
    m_textureSpritesMiddle.push_back(std::move(entity));

    EnemyRat::s_collidingSprites.push_back(ePtr);

    return ePtr;
}

EnemyRatGuard* Scene::createEnemyRatGuard(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.rotation = rotation;
    spriteData.texturePtr = texture;
    spriteData.texPartScale = Vec2f{ 1.0f / 2.0f, 1.0f / 10.0f };
    spriteData.texPartIndex = Vec2f{ 1.0f, 9.0f };

    std::unique_ptr<EnemyRatGuard> entity = std::make_unique<EnemyRatGuard>(spriteData, m_player);
    auto* ePtr = entity.get();
    for (int i = 0; i < m_lights.size(); i++)
        m_shadowSpritesFront.push_back(std::make_unique<CharacterShadow>(spriteData, i, ePtr));
    m_textureSpritesFront.push_back(std::move(entity));

    EnemyRat::s_collidingSprites.push_back(ePtr);

    return ePtr;
}

EnemyRatSniffer* Scene::createEnemyRatSniffer(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.rotation = rotation;
    spriteData.texturePtr = texture;
    spriteData.texPartScale = Vec2f{ 1.0f / 2.0f, 1.0f / 10.0f };
    spriteData.texPartIndex = Vec2f{ 1.0f, 9.0f };

    std::unique_ptr<EnemyRatSniffer> entity = std::make_unique<EnemyRatSniffer>(spriteData, m_player);
    auto* ePtr = entity.get();
    for (int i = 0; i < m_lights.size(); i++)
        m_shadowSpritesFront.push_back(std::make_unique<CharacterShadow>(spriteData, i, ePtr));
    m_textureSpritesFront.push_back(std::move(entity));

    EnemyRat::s_collidingSprites.push_back(ePtr);

    return ePtr;
}

UISprite* Scene::createUISprite(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.texturePtr = texture;

    std::unique_ptr<UISprite> entity = std::make_unique<UISprite>(spriteData, m_window, uiPos);
    auto* ePtr = entity.get();
    m_uiSprites.push_back(std::move(entity));

    return ePtr;
}

Button* Scene::createButton(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos, const ButtonPresses& buttonPresses)
{
    SpriteData spriteData;
    spriteData.center = center;
    spriteData.halfSize = halfSize;
    spriteData.texturePtr = texture;

    std::unique_ptr<Button> entity = std::make_unique<Button>(spriteData, m_window, uiPos, buttonPresses);
    auto* ePtr = entity.get();
    m_uiSprites.push_back(std::move(entity));

    return ePtr;
}

void Scene::addTargetPoint(EnemyRatSniffer* rat, const Vec2f& point)
{
    rat->addTargetPoint(point);
}

template<typename VertexType, typename SpriteType>
void Scene::renderSprites(
    std::vector<VertexArray<VertexType>>& vertexArrays,
    std::vector<std::unique_ptr<SpriteType>>& sprites,
    Shader* shader)
{
    for (auto& va : vertexArrays)
        va.clear();

    {
        std::unordered_map<Texture*, int> texSlots;
        size_t index = 0;
        for (auto& sprite : sprites)
        {
            Texture* texturePtr = sprite->getTexturePtr();
            int slot = 0;
            if (texSlots.find(texturePtr) != texSlots.end())
            {
                slot = texSlots.at(texturePtr);
            }
            else
            {
                slot = texSlots.size();
                texSlots[texturePtr] = slot;
            }

            texturePtr->bind(slot);

            auto& vertices = sprite->getVertices(slot);
            vertexArrays[index].addGeometryPiece(vertices.data());

            if (texSlots.size() == m_maxTextureUnits)
            {
                index++;
                if (index == vertexArrays.size())
                    vertexArrays.emplace_back(vertexArrays[0].getVertexCount());
            }
        }
    }

    for (auto& va : vertexArrays)
        va.createSubData();

    shader->bind();
    for (auto& va : vertexArrays)
    {
        va.bind();
        const uint32_t elementsCount = va.getObjectsCount() * va.getIndicesCount();
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, nullptr);
    }
}

template void Scene::renderSprites<TextureVertex, TextureSprite>(
    std::vector<VertexArray<TextureVertex>>&vertexArrays,
    std::vector<std::unique_ptr<TextureSprite>>&sprites,
    Shader* shader);
template void Scene::renderSprites<ShadowVertex, ShadowSprite>(
    std::vector<VertexArray<ShadowVertex>>&vertexArrays,
    std::vector<std::unique_ptr<ShadowSprite>>&sprites,
    Shader* shader);
template void Scene::renderSprites<UIVertex, UISprite>(
    std::vector<VertexArray<UIVertex>>&vertexArrays,
    std::vector<std::unique_ptr<UISprite>>&sprites,
    Shader* shader);