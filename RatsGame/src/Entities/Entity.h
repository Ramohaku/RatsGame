#pragma once
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

struct SpriteData
{
	Vec2f center = { 0.0f, 0.0f };
	Vec2f halfSize = { 1.0f, 1.0f };
	float rotation = 0.0f;
	Texture* texturePtr = nullptr;
	Vec2f texPartScale = { 1.0f, 1.0f };
	Vec2f texPartIndex = { 0.0f, 0.0f };
	float flipHorizontal = 1.0f;
	float flipVertical = 1.0f;
};

class Sprite
{
public:
	Sprite(const SpriteData& spriteData);
	Sprite(const Sprite&) = delete;
	Sprite(Sprite&&) = delete;
	virtual ~Sprite() {}
	virtual void onUpdate(float deltaTime) {}

	inline void setCenter(const Vec2f& center) { m_spriteData.center = center; }
	inline void setHalfSize(const Vec2f& halfSize) { m_spriteData.halfSize = halfSize; }
	inline void setRotation(float rotation) { m_spriteData.rotation = rotation; }
	inline void setTexPartScale(const Vec2f& texPartScale) { m_spriteData.texPartScale = texPartScale; }
	inline void setTexPartIndex(const Vec2f& texPartIndex) { m_spriteData.texPartIndex = texPartIndex; }
	inline void setFlipHorizontal(float flip) { m_spriteData.flipHorizontal = flip; }
	inline void setFlipVertical(float flip) { m_spriteData.flipVertical = flip; }

	inline const Vec2f& getCenter() const { return m_spriteData.center; }
	inline const Vec2f& getHalfSize() const { return m_spriteData.halfSize; }
	inline Texture* getTexturePtr() const { return m_spriteData.texturePtr; }
	inline float getRotation() const { return m_spriteData.rotation; }
	inline const Vec2f& getTexPartScale() const { return m_spriteData.texPartScale; }
	inline const Vec2f& getTexPartIndex() const { return m_spriteData.texPartIndex; }
	inline float getFlipHorizontal() const { return m_spriteData.flipHorizontal; }
	inline float getFlipVertical() const { return m_spriteData.flipVertical; }

	inline SpriteData& getSpriteData() { return m_spriteData; }
protected:
	SpriteData m_spriteData;
};

class TextureSprite : virtual public Sprite
{
public:
	TextureSprite(const SpriteData& spriteData);
	virtual ~TextureSprite() {}

	const std::array<TextureVertex, 4>& getVertices(int slot);
private:
	std::array<TextureVertex, 4> m_vertices;
};

class ShadowSprite : virtual public Sprite
{
public:
	ShadowSprite(const SpriteData& spriteData, Sprite* sprite, int lightIndex = 0);
	virtual ~ShadowSprite() {}

	void updateLights(const std::vector<Vec4f>& lightsData);
	void setLightActive(bool active);

	inline void setScale(float scale) { m_scale = scale; }
	inline float getScale() const { return m_scale; }
	inline void setLightAngle(float lightAngle) { m_lightAngle = lightAngle; }
	inline float getLightAngle() const { return m_lightAngle; }
	inline int getLightIndex() const { return m_lightIndex; }
	inline Sprite* getSprite() const { return m_sprite; }

	const std::array<ShadowVertex, 4>& getVertices(int slot);
private:
	Sprite* m_sprite;
	float m_lightAngle = 0.0f;
	float m_scale = 1.0f;
	int m_lightIndex;
	std::array<ShadowVertex, 4> m_vertices;
};

