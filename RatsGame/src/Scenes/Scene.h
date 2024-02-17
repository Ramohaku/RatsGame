#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Entities/EnemyRats.h"
#include "Entities/Light.h"
#include "Entities/UI/UISprite.h"

struct AppSceneData
{
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	GLint maxTextureUnits = 0;

	std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;

	std::unique_ptr<Shader> textureShader;
	std::unique_ptr<Shader> shadowShader;
	std::unique_ptr<Shader> uiShader;

	std::vector<VertexArray<TextureVertex>> textureVertexArraysBack;
	std::vector<VertexArray<ShadowVertex>> shadowVertexArraysMiddle;
	std::vector<VertexArray<TextureVertex>> textureVertexArraysMiddle;
	std::vector<VertexArray<ShadowVertex>> shadowVertexArraysFront;
	std::vector<VertexArray<TextureVertex>> textureVertexArraysFront;
	std::vector<VertexArray<UIVertex>> uiVertexArrays;
};

class Scene
{
protected:
	struct ExtraData
	{
		char name[20]{};
		int entityType = 0;
		std::string textureName;
		bool shadows = true;
		bool blocking = false;
	};
public:
	Scene(Window* window, AppSceneData& appSceneData);
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	virtual ~Scene();

	virtual void onUpdate(float deltaTime);
	virtual void onRender();

	Player* getPlayer() { return m_player; }

	void createLight(const Light& light);
	Player* createPlayer(const Vec2f& center, Texture* texture);
	TextureSprite* createTextureSpriteBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	TextureSprite* createTextureBlockBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	EnemyRatWatcher* createEnemyRatWatcher(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	EnemyRatSniffer* createEnemyRatSniffer(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	UISprite* createUISprite(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos);
	Button* createButton(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos, const ButtonPresses& buttonPresses);
	void addTargetPoint(EnemyRatSniffer* rat, const Vec2f& point);

public:
	static bool s_skip;
protected:
	void updateLights();
	void loadEntities(const char* fileName);
private:
	template <typename VertexType, typename SpriteType>
	void renderSprites(
		std::vector<VertexArray<VertexType>>& vertexArrays,
		std::vector<std::unique_ptr<SpriteType>>& sprites,
		Shader* shader);
protected:
	Window* m_window;

	AppSceneData& m_appSceneData;
	
	Player* m_player = nullptr;
	std::vector<Light> m_lights;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesBack;
	std::vector<std::unique_ptr<ShadowSprite>> m_shadowSpritesMiddle;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesMiddle;
	std::vector<std::unique_ptr<ShadowSprite>> m_shadowSpritesFront;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesFront;
	std::vector<std::unique_ptr<UISprite>> m_uiSprites;

	std::unordered_map<std::string, TextureSprite*> m_namedSprites;
};
