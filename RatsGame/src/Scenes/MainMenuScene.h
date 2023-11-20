#pragma once
#include "Scene.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene(Window* window, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader);
	~MainMenuScene() {}
};

