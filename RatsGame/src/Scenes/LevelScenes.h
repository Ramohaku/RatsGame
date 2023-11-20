#pragma once
#include "Scene.h"

class Level1 : public Scene
{
public:
	Level1(Window* window, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader);
	~Level1() {}
};