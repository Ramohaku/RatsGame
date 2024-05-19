#include "stdafx.h"
#include "Scene.h"
#include "Debug.h"

using json = nlohmann::json;

bool Scene::s_skip = false;
 
Scene::Scene(Window* window, AppSceneData& appSceneData, bool pauseable)
    : m_window(window), m_appSceneData(appSceneData), m_pauseable(pauseable)
{
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

    m_appSceneData.uiShader->bind();
    m_appSceneData.uiShader->setUniformMat4f("u_MVP", mvp);
}

Scene::~Scene()
{
    Rat::s_collidingSprites.clear();

    for (auto& va : m_appSceneData.textureVertexArraysBack)
        va.clear();
    for (auto& va : m_appSceneData.shadowVertexArraysMiddle)
        va.clear();
    for (auto& va : m_appSceneData.textureVertexArraysMiddle)
        va.clear();
    for (auto& va : m_appSceneData.shadowVertexArraysFront)
        va.clear();
    for (auto& va : m_appSceneData.textureVertexArraysFront)
        va.clear();
    for (auto& va : m_appSceneData.uiVertexArrays)
        va.clear();
}

void Scene::onUpdate(float deltaTime)
{
    bool escPressed = glfwGetKey(m_window->getGlfwWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (m_paused)
    {
        if (escPressed && !m_escPrePressed && m_pauseable)
        {
            onPauseEnd();
            m_paused = false;
        }
    }
    else
    {
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

        if (escPressed && !m_escPrePressed && m_pauseable)
        {
            onPauseStart();
            m_paused = true;
        }
    }

    m_escPrePressed = escPressed;


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

    if (m_player)
    {
        m_window->updateView(m_player->getCenter(), m_player->getRotation() + PI_F / 2.0f);
    }

    updateLights();
}

void Scene::onRender()
{
    renderSprites(m_appSceneData.textureVertexArraysBack, m_textureSpritesBack, m_appSceneData.textureShader.get());
    renderSprites(m_appSceneData.shadowVertexArraysMiddle, m_shadowSpritesMiddle, m_appSceneData.shadowShader.get());
    renderSprites(m_appSceneData.textureVertexArraysMiddle, m_textureSpritesMiddle, m_appSceneData.textureShader.get());
    renderSprites(m_appSceneData.shadowVertexArraysFront, m_shadowSpritesFront, m_appSceneData.shadowShader.get());
    renderSprites(m_appSceneData.textureVertexArraysFront, m_textureSpritesFront, m_appSceneData.textureShader.get());
    renderSprites(m_appSceneData.uiVertexArrays, m_uiSprites, m_appSceneData.uiShader.get());
}

void Scene::createLight(const Light& light)
{
    m_lights.push_back(light);
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

void Scene::updateLights()
{
    float lightStrength = 0.0f;
    std::vector<Vec4f> lightsColors;
    std::vector<Vec4f> lightsData;
    for (const auto& light : m_lights)
    {
        float active = static_cast<float>(light.active);

        if (m_player && m_rangeOptimize)
        {
            const auto& center = m_player->getCenter();
            const float d = hypot(light.center.x - m_player->getCenter().x, light.center.y - m_player->getCenter().y);
            const float fn = std::clamp(light.distance / d - light.vanish, 0.0f, 3.0f);
            lightStrength += fn;

            const float radius = static_cast<float>(m_window->getHeight()) * m_window->getScale() * 0.5f;
            
            const float dLeft = center.x - radius - light.center.x;
            float fnLeft = std::clamp(light.distance / dLeft - light.vanish, 0.0f, 3.0f);
            const float dRight = light.center.x - center.x - radius;
            float fnRight = std::clamp(light.distance / dRight - light.vanish, 0.0f, 3.0f);
            const float dUp = light.center.y - center.y - radius;
            float fnUp = std::clamp(light.distance / dUp - light.vanish, 0.0f, 3.0f);
            const float dDown = center.y - radius - light.center.y;
            float fnDown = std::clamp(light.distance / dDown - light.vanish, 0.0f, 3.0f);

            if ((dLeft > 0.0f && fnLeft == 0.0f) ||
                (dRight > 0.0f && fnRight == 0.0f) ||
                (dUp > 0.0f && fnUp == 0.0f) ||
                (dDown > 0.0f && fnDown == 0.0f))
            {
                active = 0.0f;
            }
        }

        lightsColors.push_back(Vec4f{ light.color.r, light.color.g, light.color.b, active });
        lightsData.push_back(Vec4f{ light.center.x, light.center.y, light.vanish, light.distance });
    }

    if (m_player)
        m_player->setLightStrength(lightStrength);

    for (auto& shadowSprite : m_shadowSpritesMiddle)
        shadowSprite->updateLights(lightsData);

    for (auto& shadowSprite : m_shadowSpritesFront)
        shadowSprite->updateLights(lightsData);

    const auto mvp = m_window->getProj() * m_window->getView();
    const int lightsCount = lightsColors.size();

    m_appSceneData.textureShader->bind();
    m_appSceneData.textureShader->setUniformMat4f("u_MVP", mvp);
    m_appSceneData.textureShader->setUniform1i("u_LightsCount", lightsCount);
    m_appSceneData.textureShader->setUniform4fv("u_LightsColors", lightsCount, lightsColors.data());
    m_appSceneData.textureShader->setUniform4fv("u_LightsData", lightsCount, lightsData.data());

    m_appSceneData.shadowShader->bind();
    m_appSceneData.shadowShader->setUniformMat4f("u_MVP", mvp);
    m_appSceneData.shadowShader->setUniform1i("u_LightsCount", lightsCount);
    m_appSceneData.shadowShader->setUniform4fv("u_LightsData", lightsCount, lightsData.data());
}

void Scene::loadEntities(const char* fileName)
{
    std::ifstream file(fileName);
    if (file)
    {
        json j;
        file >> j;

        if (j["sceneData"].contains("windowScale"))
            m_window->setScale(j["sceneData"]["windowScale"].get<double, float>());

        const auto& lightsData = j["sceneData"]["lights"].get<std::vector<json>>();
        const auto& entitiesData = j["sceneData"]["entities"].get<std::vector<json>>();

        for (const auto& data : lightsData)
        {
            Light light;
            light.color.r = data["color"]["r"].get<double, float>();
            light.color.g = data["color"]["g"].get<double, float>();
            light.color.b = data["color"]["b"].get<double, float>();
            light.center.x = data["center"]["x"].get<double, float>();
            light.center.y = data["center"]["y"].get<double, float>();
            light.vanish = data["vanish"].get<double, float>();
            light.distance = data["distance"].get<double, float>();
            light.active = data["active"].get<bool>();
            m_lights.push_back(light);
        }

        for (const auto& data : entitiesData)
        {
            SpriteData spriteData;
            spriteData.center.x = data["center"]["x"].get<double, float>();
            spriteData.center.y = data["center"]["y"].get<double, float>();
            spriteData.halfSize.x = data["halfSize"]["x"].get<double, float>();
            spriteData.halfSize.y = data["halfSize"]["y"].get<double, float>();
            spriteData.rotation = data["rotation"].get<double, float>();
            spriteData.texturePtr = m_appSceneData.textures.at(data["textureName"].get<std::string>()).get();
            spriteData.texPartScale.x = data["texPartScale"]["x"].get<double, float>();
            spriteData.texPartScale.y = data["texPartScale"]["y"].get<double, float>();
            spriteData.texPartIndex.x = data["texPartIndex"]["x"].get<double, float>();
            spriteData.texPartIndex.y = data["texPartIndex"]["y"].get<double, float>();
            spriteData.flipHorizontal = data["flipHorizontal"].get<double, float>();
            spriteData.flipVertical = data["flipVertical"].get<double, float>();

            //std::unique_ptr<TextureSprite> sprite;

            bool shadows = data["shadows"].get<bool>();
            bool blocking = data["blocking"].get<bool>();

            enum
            {
                ShadowSpriteT = 0, CharacterShadowT
            } shadowType;

            std::vector<std::unique_ptr<ShadowSprite>>* shadowSprites = nullptr;
            std::vector<std::unique_ptr<TextureSprite>>* textureSprites = nullptr;
            const int layer = data["layer"].get<int>();
            switch (layer)
            {
            case 0:
                //m_textureSpritesBack.push_back(std::move(sprite));
                textureSprites = &m_textureSpritesBack;
                break;
            case 1:
                shadowSprites = &m_shadowSpritesMiddle;
                textureSprites = &m_textureSpritesMiddle;
                //if (shadows)
                //{
                //    switch (shadowType)
                //    {
                //    case ShadowSpriteT:
                //        for (int i = 0; i < m_lights.size(); i++)
                //            m_shadowSpritesMiddle.push_back(std::make_unique<ShadowSprite>(spriteData, spritePtr, i));
                //        break;
                //    case CharacterShadowT:
                //        for (int i = 0; i < m_lights.size(); i++)
                //            m_shadowSpritesMiddle.push_back(std::make_unique<CharacterShadow>(spriteData, i, reinterpret_cast<Character*>(spritePtr)));
                //        break;
                //    }
                //}
                //m_textureSpritesMiddle.push_back(std::move(sprite));
                break;
            case 2:
                shadowSprites = &m_shadowSpritesFront;
                textureSprites = &m_textureSpritesFront;
                //if (shadows)
                //{
                //    switch (shadowType)
                //    {
                //    case ShadowSpriteT:
                //        for (int i = 0; i < m_lights.size(); i++)
                //            m_shadowSpritesFront.push_back(std::make_unique<ShadowSprite>(spriteData, spritePtr, i));
                //        break;
                //    case CharacterShadowT:
                //        for (int i = 0; i < m_lights.size(); i++)
                //            m_shadowSpritesFront.push_back(std::make_unique<CharacterShadow>(spriteData, i, reinterpret_cast<Character*>(spritePtr)));
                //        break;
                //    }
                //}
                //m_textureSpritesFront.push_back(std::move(sprite));
                break;
            }

            const int entityType = data["entityType"].get<int>();
            switch (entityType)
            {
            case 0:
            {
                auto sprite = std::make_unique<TextureSprite>(spriteData);
                if (shadows)
                    for (int i = 0; i < m_lights.size(); i++)
                         shadowSprites->push_back(std::make_unique<ShadowSprite>(spriteData, sprite.get(), i));
                if (blocking)
                    EnemyRat::s_collidingSprites.push_back(sprite.get());
                textureSprites->push_back(std::move(sprite));
                break;
            }
            case 1:
            {
                auto sprite = std::make_unique<Player>(spriteData, m_window);
                m_player = sprite.get();
                if (shadows)
                    for (int i = 0; i < m_lights.size(); i++)
                        shadowSprites->push_back(std::make_unique<CharacterShadow>(spriteData, i, sprite.get()));
                if (blocking)
                    EnemyRat::s_collidingSprites.push_back(sprite.get());
                textureSprites->push_back(std::move(sprite));
                break;
            }
            case 2:
            {
                std::vector<SoundBuffer*> buffers(RAT_SOUNDS_COUNT);
                
                using namespace std::string_literals;
                for (int i = 0; i < RAT_SOUNDS_COUNT; i++)
                {
                    const auto iStr = std::to_string(i);
                    buffers[i] = &m_appSceneData.soundBuffers.at("RatSound"s + iStr);
                }

                auto sprite = std::make_unique<EnemyRatWatcher>(spriteData, m_player, std::move(buffers));
                if (shadows)
                    for (int i = 0; i < m_lights.size(); i++)
                        shadowSprites->push_back(std::make_unique<CharacterShadow>(spriteData, i, sprite.get()));
                if (blocking)
                    EnemyRat::s_collidingSprites.push_back(sprite.get());
                textureSprites->push_back(std::move(sprite));
                break;
            }
            case 3:
            {
                auto sprite = std::make_unique<EnemyRatSniffer>(spriteData, m_player);
                if (shadows)
                    for (int i = 0; i < m_lights.size(); i++)
                        shadowSprites->push_back(std::make_unique<CharacterShadow>(spriteData, i, sprite.get()));
                if (blocking)
                    EnemyRat::s_collidingSprites.push_back(sprite.get());
                textureSprites->push_back(std::move(sprite));
                break;
            }
            }

            std::string name = data["name"].get<std::string>();
            if (!name.empty())
            {
                m_namedSprites[std::move(name)] = textureSprites->back().get();
            }
        }
        file.close();
    }
    else
    {
        ASSERT(false);
    }
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
            if (m_player && m_rangeOptimize)
            {
                const Vec2f& playerCenter = m_player->getCenter();
                const Vec2f& spriteCenter = sprite->getCenter();

                float viewRange = 0.8f;

                /*float rotationDiff = m_player->getRotation()
                    + atan2(playerCenter.y - spriteCenter.y, playerCenter.x - spriteCenter.x);
                correctAngleManyIter(rotationDiff);

                if (rotationDiff > PI_F / 2.0f || rotationDiff < -PI_F / 2.0f)
                {
                    viewRange = 0.2f;
                }*/

                const float distance = hypot(playerCenter.x - spriteCenter.x, playerCenter.y - spriteCenter.y);
                const float totalRange = sprite->getRange() + static_cast<float>(m_window->getHeight()) * m_window->getScale() * viewRange;

                if (distance > totalRange)
                {
                    continue;
                }
            }

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

            if (texSlots.size() == m_appSceneData.maxTextureUnits)
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
        //std::cerr << elementsCount << '\n';
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
