#include "stdafx.h"
#include "Entity.h"


Sprite::Sprite(const SpriteData& spriteData)
    : m_spriteData(spriteData)
{
    calculateRange();
}

void Sprite::calculateRange()
{
    m_range = hypot(m_spriteData.halfSize.x, m_spriteData.halfSize.y);
}

TextureSprite::TextureSprite(const SpriteData& spriteData)
    : Sprite(spriteData)
{
}

const std::array<TextureVertex, 4>& TextureSprite::getVertices(int slot)
{
    const float texCoords_x0 = m_spriteData.texPartScale.x *  m_spriteData.texPartIndex.x;
    const float texCoords_x1 = m_spriteData.texPartScale.x * (m_spriteData.texPartIndex.x + 1.0f);
    const float texCoords_y0 = m_spriteData.texPartScale.y *  m_spriteData.texPartIndex.y;
    const float texCoords_y1 = m_spriteData.texPartScale.y * (m_spriteData.texPartIndex.y + 1.0f);

    const Vec2f halfSize
    {
        m_spriteData.halfSize.x * m_spriteData.flipHorizontal,
        m_spriteData.halfSize.y * m_spriteData.flipVertical
    };

    // 0,0
    m_vertices[0].center = m_spriteData.center;
    m_vertices[0].offset = -halfSize;
    m_vertices[0].texCoords = Vec2f{ texCoords_x0, texCoords_y0 };
    m_vertices[0].texID = static_cast<float>(slot);
    m_vertices[0].rotation = m_spriteData.rotation;

    // 1,0
    m_vertices[1].center = m_spriteData.center;
    m_vertices[1].offset = Vec2f{ halfSize.x, -halfSize.y };
    m_vertices[1].texCoords = Vec2f{ texCoords_x1, texCoords_y0 };
    m_vertices[1].texID = static_cast<float>(slot);
    m_vertices[1].rotation = m_spriteData.rotation;

    // 1,1
    m_vertices[2].center = m_spriteData.center;
    m_vertices[2].offset = halfSize;
    m_vertices[2].texCoords = Vec2f{ texCoords_x1, texCoords_y1 };
    m_vertices[2].texID = static_cast<float>(slot);
    m_vertices[2].rotation = m_spriteData.rotation;

    // 0,1
    m_vertices[3].center = m_spriteData.center;
    m_vertices[3].offset = Vec2f{ -halfSize.x, halfSize.y };
    m_vertices[3].texCoords = Vec2f{ texCoords_x0, texCoords_y1 };
    m_vertices[3].texID = static_cast<float>(slot);
    m_vertices[3].rotation = m_spriteData.rotation;

    return m_vertices;
}
ShadowSprite::ShadowSprite(const SpriteData& spriteData, Sprite* sprite, int lightIndex)
    : Sprite(spriteData),
    m_sprite(sprite), m_lightIndex(lightIndex)
{
}

void ShadowSprite::updateLights(const std::vector<Vec4f>& lightsData)
{
    const Vec2f& spriteCenter = m_sprite->getCenter();

    float lightAngle = atan2(spriteCenter.y - lightsData[m_lightIndex].y, spriteCenter.x - lightsData[m_lightIndex].x);
    const float lightDistance = hypot(spriteCenter.x - lightsData[m_lightIndex].x, spriteCenter.y - lightsData[m_lightIndex].y);

    const float rotation = m_sprite->getRotation();
    lightAngle += rotation;
    if (lightAngle < -PI_F)
        lightAngle += 2.0f * PI_F;
    else if (lightAngle > PI_F)
        lightAngle -= 2.0f * PI_F;

    setHalfSize(m_sprite->getHalfSize());
    setRotation(rotation);
    setLightAngle(lightAngle);
    setScale(log(lightDistance * 0.8f + 1.0f));
    setCenter(spriteCenter);
    setFlipHorizontal(m_sprite->getFlipHorizontal());
    setFlipVertical(m_sprite->getFlipVertical());
}

void ShadowSprite::setLightActive(bool active)
{
    const float ac = static_cast<float>(active);
    for (auto& vertex : m_vertices)
        vertex.lightActive = ac;
}

const std::array<ShadowVertex, 4>& ShadowSprite::getVertices(int slot)
{
    const float texCoords_x0 = m_spriteData.texPartScale.x * m_spriteData.texPartIndex.x;
    const float texCoords_x1 = m_spriteData.texPartScale.x * (m_spriteData.texPartIndex.x + 1.0f);
    const float texCoords_y0 = m_spriteData.texPartScale.y * m_spriteData.texPartIndex.y;
    const float texCoords_y1 = m_spriteData.texPartScale.y * (m_spriteData.texPartIndex.y + 1.0f);

    const Vec2f halfSize
    {
        m_spriteData.halfSize.x * m_spriteData.flipHorizontal,
        m_spriteData.halfSize.y * m_spriteData.flipVertical
    };

    // 0,0
    m_vertices[0].center = m_spriteData.center;
    m_vertices[0].offset = -halfSize;
    m_vertices[0].texCoords = Vec2f{ texCoords_x0, texCoords_y0 };
    m_vertices[0].texID = static_cast<float>(slot);
    m_vertices[0].rotation = m_spriteData.rotation;
    m_vertices[0].lightIndex = static_cast<float>(m_lightIndex);

    // 1,0
    m_vertices[1].center = m_spriteData.center;
    m_vertices[1].offset = Vec2f{ halfSize.x, -halfSize.y };
    m_vertices[1].texCoords = Vec2f{ texCoords_x1, texCoords_y0 };
    m_vertices[1].texID = static_cast<float>(slot);
    m_vertices[1].rotation = m_spriteData.rotation;
    m_vertices[1].lightIndex = static_cast<float>(m_lightIndex);

    // 1,1
    m_vertices[2].center = m_spriteData.center;
    m_vertices[2].offset = halfSize;
    m_vertices[2].texCoords = Vec2f{ texCoords_x1, texCoords_y1 };
    m_vertices[2].texID = static_cast<float>(slot);
    m_vertices[2].rotation = m_spriteData.rotation;
    m_vertices[2].lightIndex = static_cast<float>(m_lightIndex);

    // 0,1
    m_vertices[3].center = m_spriteData.center;
    m_vertices[3].offset = Vec2f{ -halfSize.x, halfSize.y };
    m_vertices[3].texCoords = Vec2f{ texCoords_x0, texCoords_y1 };
    m_vertices[3].texID = static_cast<float>(slot);
    m_vertices[3].rotation = m_spriteData.rotation;
    m_vertices[3].lightIndex = static_cast<float>(m_lightIndex);

    //m_lightAngle = fmod(m_lightAngle, 3.1415f * 2.0f);

    const float rotCos = cos(m_lightAngle) * m_scale;
    const float rotSin = sin(m_lightAngle) * m_scale;

    if (m_lightAngle > -3.1415f / 2.0f && m_lightAngle < 3.1415f / 2.0f)
    {
        if (m_spriteData.flipHorizontal > 0.0f)
        {
            m_vertices[1].offset.x += rotCos;
            m_vertices[2].offset.x += rotCos;
        }
        else
        {
            m_vertices[0].offset.x += rotCos;
            m_vertices[3].offset.x += rotCos;
        }
    }
    else
    {
        if (m_spriteData.flipHorizontal > 0.0f)
        {
            m_vertices[0].offset.x += rotCos;
            m_vertices[3].offset.x += rotCos;
        }
        else
        {
            m_vertices[1].offset.x += rotCos;
            m_vertices[2].offset.x += rotCos;
        }
    }

    if (m_lightAngle < 0)
    {
        if (m_spriteData.flipVertical > 0.0f)
        {
            m_vertices[0].offset.y += rotSin;
            m_vertices[1].offset.y += rotSin;
        }
        else
        {
            m_vertices[2].offset.y += rotSin;
            m_vertices[3].offset.y += rotSin;
        }
    }
    else
    {
        if (m_spriteData.flipVertical > 0.0f)
        {
            m_vertices[2].offset.y += rotSin;
            m_vertices[3].offset.y += rotSin;
        }
        else
        {
            m_vertices[0].offset.y += rotSin;
            m_vertices[1].offset.y += rotSin;
        }
    }

    return m_vertices;
}
