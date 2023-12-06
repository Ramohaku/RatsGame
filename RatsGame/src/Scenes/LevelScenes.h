#pragma once
#include "Scene.h"

class Level1 : public Scene
{
public:
	Level1(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader);
	~Level1() {}
};