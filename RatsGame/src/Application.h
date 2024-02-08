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
	void clean();

	void createNewScene(const UpdateFunc& updateFunc = nullptr);
	void createMainMenu();
	void createEditorScene();

	void createLevel1();
	void createLevel2();

	/*template<typename T>
	void createLevel(const std::function<void()>& transitionFunction, SpriteData* playerSpriteData)
	{
		Scene::s_skip = true;

		m_currentScene->onClean();
		delete m_currentScene;
		m_currentScene = new T(&m_window, m_textures, m_maxTextureUnits, m_textureShader.get(), m_shadowShader.get(), m_uiShader.get(), transitionFunction);

		if (playerSpriteData)
		{
			m_currentScene->getPlayer()->setSpriteData(*playerSpriteData);
		}
	}*/

	void createTexture(std::string&& name, std::string&& filepath);
private:
	Window m_window;
	float m_deltaTime = 0.0f;
	Scene* m_currentScene = nullptr;
	GLint m_maxTextureUnits = 0;

	GLuint m_fbo = 0;
	GLuint m_screenTexture = 0;
	ScreenVertex m_screenVertices[4];

	std::unique_ptr<Shader> m_textureShader;
	std::unique_ptr<Shader> m_shadowShader;
	std::unique_ptr<Shader> m_uiShader;

	std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

	std::unique_ptr<Shader> m_screenShader;
	std::unique_ptr<VertexArray<ScreenVertex>> m_screenVertexArray;
};
