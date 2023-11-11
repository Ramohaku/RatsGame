#pragma once
#include "Scenes/Scene.h"

class Application
{
public:
	Application();
	~Application();

	void run();
private:
	void update();
	void render();

	void createNewScene(const UpdateFunc& updateFunc = nullptr);
	void createMainMenu();
	void createTestLevel();
private:
	Window m_window;
	float m_deltaTime = 0.0f;
	Scene* m_currentScene = nullptr;
	GLint m_maxTextureUnits = 0;

	std::unique_ptr<Shader> m_textureShader;
	std::unique_ptr<Shader> m_shadowShader;
	std::unique_ptr<Shader> m_uiShader;
	std::unique_ptr<Texture> m_rat1Texture;
	std::unique_ptr<Texture> m_ratPlayerTexture;
	std::unique_ptr<Texture> m_testBlockTexture;
	std::unique_ptr<Texture> m_testBlock2Texture;
	std::unique_ptr<Texture> m_testButtonTexture;
};
