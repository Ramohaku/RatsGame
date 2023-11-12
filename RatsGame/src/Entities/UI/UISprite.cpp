#include "stdafx.h"
#include "UISprite.h"

UISprite::UISprite(const SpriteData& spriteData, Window* window, UIPos uiPos)
	: Sprite(spriteData), m_windowPtr(window)
{
    const float windowWidthHalf = static_cast<float>(window->getWidth() / 2);
    const float windowHeightHalf = static_cast<float>(window->getHeight() / 2);

    if (uiPos & (UIPos)UIPosition::Left)
        m_spriteData.center.x -= windowWidthHalf;

    if (uiPos & (UIPos)UIPosition::Right)
        m_spriteData.center.x += windowWidthHalf;

    if (uiPos & (UIPos)UIPosition::Up)
        m_spriteData.center.y += windowHeightHalf;

    if (uiPos & (UIPos)UIPosition::Down)
        m_spriteData.center.y -= windowHeightHalf;
}

const std::array<UIVertex, 4>& UISprite::getVertices(int slot)
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
    m_vertices[0].offset = m_spriteData.center - halfSize;
    m_vertices[0].texCoords = Vec2f{ texCoords_x0, texCoords_y0 };
    m_vertices[0].texID = static_cast<float>(slot);

    // 1,0
    m_vertices[1].offset = m_spriteData.center + Vec2f{ halfSize.x, -halfSize.y };
    m_vertices[1].texCoords = Vec2f{ texCoords_x1, texCoords_y0 };
    m_vertices[1].texID = static_cast<float>(slot);

    // 1,1
    m_vertices[2].offset = m_spriteData.center + halfSize;
    m_vertices[2].texCoords = Vec2f{ texCoords_x1, texCoords_y1 };
    m_vertices[2].texID = static_cast<float>(slot);

    // 0,1
    m_vertices[3].offset = m_spriteData.center + Vec2f{ -halfSize.x, halfSize.y };
    m_vertices[3].texCoords = Vec2f{ texCoords_x0, texCoords_y1 };
    m_vertices[3].texID = static_cast<float>(slot);

    return m_vertices;
}

Button::Button(const SpriteData& spriteData, Window* window, UIPos uiPos, const ButtonPresses& buttonPresses)
    : UISprite(spriteData, window, uiPos), m_buttonPresses(buttonPresses)
{
}

void Button::onUpdate(float deltaTime)
{
    UISprite::onUpdate(deltaTime);

    double mouseX, mouseY;
    glfwGetCursorPos(m_windowPtr->getGlfwWindow(), &mouseX, &mouseY);

    //std::cout << mouseX << ' ' << mouseY << '\n';

    const Vec2f mousePos = {
        static_cast<float>(mouseX)
        - static_cast<float>(m_windowPtr->getWidth()) / 2.0f,
        static_cast<float>(m_windowPtr->getHeight()) - static_cast<float>(mouseY)
        - static_cast<float>(m_windowPtr->getHeight()) / 2.0f
    };

    const int mouseState = glfwGetMouseButton(m_windowPtr->getGlfwWindow(), GLFW_MOUSE_BUTTON_LEFT);
    
    const bool hoveringNow = mousePos.x < m_spriteData.center.x + m_spriteData.halfSize.x
        && mousePos.x > m_spriteData.center.x - m_spriteData.halfSize.x
        && mousePos.y < m_spriteData.center.y + m_spriteData.halfSize.y
        && mousePos.y > m_spriteData.center.y - m_spriteData.halfSize.y;

    if (hoveringNow && !m_hovering)
    {
        if (m_buttonPresses.onHoverOver)
            m_buttonPresses.onHoverOver();
        m_hovering = true;
    }
    else if (!hoveringNow)
    {
        m_hovering = false;
    }

    if (mouseState == GLFW_PRESS && !m_pressed)
    {
        if (hoveringNow)
        {
            if (m_buttonPresses.onPress)
                m_buttonPresses.onPress();
            m_pressedGood = true;
        }
        m_pressed = true;
    }
    else if (mouseState == GLFW_RELEASE && m_pressed)
    {
        if (hoveringNow && m_pressedGood)
        {
            if (m_buttonPresses.onRelease)
                m_buttonPresses.onRelease();
        }
        m_pressed = false;
        m_pressedGood = false;
    }
}
