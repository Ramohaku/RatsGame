#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Entities/EnemyRats.h"
#include "Entities/Light.h"
#include "Entities/UI/UISprite.h"

using UpdateFunc = std::function<void(float)>;

class Scene
{
public:
	Scene(Window* window, const UpdateFunc& updateFunc, GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader);
	virtual ~Scene() {}

	virtual void onUpdate(float deltaTime);
	virtual void onRender();

	void createLight(const Light& light);
	Player* createPlayer(const Vec2f& center, Texture* texture);
	TextureSprite* createTextureSpriteBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	TextureSprite* createTextureBlockBack(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	EnemyRatGuard* createEnemyRatGuard(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	EnemyRatSniffer* createEnemyRatSniffer(const Vec2f& center, const Vec2f& halfSize, float rotation, Texture* texture);
	UISprite* createUISprite(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos);
	Button* createButton(const Vec2f& center, const Vec2f& halfSize, Texture* texture, UIPos uiPos, const ButtonPresses& buttonPresses);
	void addTargetPoint(EnemyRatSniffer* rat, const Vec2f& point);

public:
	static bool s_skip;
private:
	template <typename VertexType, typename SpriteType>
	void renderSprites(
		std::vector<VertexArray<VertexType>>& vertexArrays,
		std::vector<std::unique_ptr<SpriteType>>& sprites,
		Shader* shader);
private:
	Window* m_window;
	UpdateFunc m_updateFunc = nullptr;

	const GLint m_maxTextureUnits;
	Shader* m_textureShader;
	Shader* m_shadowShader;
	Shader* m_uiShader;

	std::vector<VertexArray<TextureVertex>> m_textureVertexArraysBack;
	std::vector<VertexArray<ShadowVertex>> m_shadowVertexArraysMiddle;
	std::vector<VertexArray<TextureVertex>> m_textureVertexArraysMiddle;
	std::vector<VertexArray<ShadowVertex>> m_shadowVertexArraysFront;
	std::vector<VertexArray<TextureVertex>> m_textureVertexArraysFront;
	std::vector<VertexArray<UIVertex>> m_uiVertexArrays;
	
	Player* m_player = nullptr;
	std::vector<Light> m_lights;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesBack;
	std::vector<std::unique_ptr<ShadowSprite>> m_shadowSpritesMiddle;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesMiddle;
	std::vector<std::unique_ptr<ShadowSprite>> m_shadowSpritesFront;
	std::vector<std::unique_ptr<TextureSprite>> m_textureSpritesFront;
	std::vector<std::unique_ptr<UISprite>> m_uiSprites;
};
