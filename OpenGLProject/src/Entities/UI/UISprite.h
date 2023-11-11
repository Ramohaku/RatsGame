#pragma once
#include "../Entity.h"
#include "Window.h"

using UIPos = int;

class UISprite : public Sprite
{
public:
	enum UIPosition
	{
		Center = 1,
		Left = 2,
		Right = 4,
		Up = 8,
		Down = 16
	};

	UISprite(const SpriteData& spriteData, Window* window, UIPos uiPos);
	virtual ~UISprite() {}

	const std::array<UIVertex, 4>& getVertices(int slot);
protected:
	Window* m_windowPtr;
private:
	std::array<UIVertex, 4> m_vertices;
};

struct ButtonPresses
{
	std::function<void()> onHoverOver = nullptr;
	std::function<void()> onPress = nullptr;
	std::function<void()> onRelease = nullptr;
};

class Button : public UISprite
{
public:
	Button(const SpriteData& spriteData, Window* window, UIPos uiPos, const ButtonPresses& buttonPresses);
	virtual ~Button() {}

	virtual void onUpdate(float deltaTime) override;
protected:
	ButtonPresses m_buttonPresses;
	bool m_hovering = false;
	bool m_pressed = false;
	bool m_pressedGood = false;
};