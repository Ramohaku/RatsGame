#pragma once
#include "Scenes/EditorScene.h"
#include "Scenes/LevelScenes.h"

class Application
{
public:
	Application();
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	~Application();

	void run();
private:
	void update();
	void render();

	void createNewScene(const UpdateFunc& updateFunc = nullptr);
	void createMainMenu();
	void createEditorScene();
	void createTestLevel();

	void createTexture(std::string&& name, std::string&& filepath);
private:
	Window m_window;
	float m_deltaTime = 0.0f;
	Scene* m_currentScene = nullptr;
	GLint m_maxTextureUnits = 0;

	std::unique_ptr<Shader> m_textureShader;
	std::unique_ptr<Shader> m_shadowShader;
	std::unique_ptr<Shader> m_uiShader;

	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<VertexArray<ScreenVertex>> m_screenVertexArray;
};
