#pragma once
#include "Scene.h"

class EditorScene : public Scene
{
public:
	EditorScene(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures,/* const UpdateFunc& updateFunc,*/ GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader);
	EditorScene(const EditorScene&) = delete;
	EditorScene(EditorScene&&) = delete;

	void onUpdate(float deltaTime) override;
	void onRender() override;
private:
	void createImGuiEntities(std::vector<std::unique_ptr<TextureSprite>>& sprites, std::vector<ExtraData>& extraData, const std::string labelName, char* buffer);
private:
	char m_texturesBackBuffer[30]{};
	char m_texturesMiddleBuffer[30]{};
	char m_texturesFrontBuffer[30]{};
	//std::vector<int> m_currentItemsBack;
	//std::vector<int> m_currentItemsMiddle;
	//std::vector<int> m_currentItemsFront;
	//std::vector<std::string> m_textureNamesBack;
	//std::vector<std::string> m_textureNamesMiddle;
	//std::vector<std::string> m_textureNamesFront;
	std::vector<ExtraData> m_extraDataBack;
	std::vector<ExtraData> m_extraDataMiddle;
	std::vector<ExtraData> m_extraDataFront;
	const char* m_textureNames[4];
	float m_windowScale = 0.03f;
};

