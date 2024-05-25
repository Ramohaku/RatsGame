#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Entities/EnemyRats.h"
#include "Entities/Light.h"
#include "Entities/UI/UISprite.h"

#define RAT_SOUNDS_COUNT 23

struct AppSceneData
{
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	GLint maxTextureUnits = 0;

	std::unordered_map<std::string, SoundBuffer> soundBuffers;

	std::unique_ptr<Shader> textureShader;
	std::unique_ptr<Shader> textureEditorShader;
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
	enum class EntityType
	{
		TextureSprite = 0, Player, EnemyRatWatcher, EnemyRatSniffer
	};

	struct ExtraData
	{
		char name[20]{};
		EntityType entityType = EntityType::TextureSprite;
		std::string textureName;
		bool shadows = true;
		bool blocking = false;
	};
public:
	Scene(Window* window, AppSceneData& appSceneData, bool pauseable = false);
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	virtual ~Scene();

	virtual void onUpdate(float deltaTime);
	virtual void onRender();

	Player* getPlayer() { return m_player; }

	void createLight(const Light& light);
	Button* createButton(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos, const ButtonPresses& buttonPresses);
	void addTargetPoint(EnemyRatSniffer* rat, const Vec2f& point);

public:
	static bool s_skip;
protected:
	void updateLights(const std::unique_ptr<Shader>& textureShader);
	void loadEntities(const char* fileName);

	virtual void onPauseStart() {}
	virtual void onPauseEnd() {}

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

	bool m_pauseable;
	bool m_paused = false;
	bool m_escPrePressed = false;
	bool m_rangeOptimize = true;
};
