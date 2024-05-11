#pragma once
#include "Utils.h"

class Sound2D
{
public:
	Sound2D(const sf::SoundBuffer& soundBuffer);
	Sound2D(const Sound2D& other) = default;
	Sound2D(Sound2D&& other) = default;
	~Sound2D() {}

	void updatePosition(const Vec2f& playerCenter);
	void setSoundBuffer(const sf::SoundBuffer& soundBuffer);

	static float s_globalVolume;
private:
	sf::Sound m_sound1;
	sf::Sound m_sound2;
	Vec2f m_center;
};

