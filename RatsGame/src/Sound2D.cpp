#include "stdafx.h"
#include "Sound2D.h"

float Sound2D::s_globalVolume = 100.0f;

Sound2D::Sound2D(const sf::SoundBuffer& soundBuffer)
	: m_sound1(soundBuffer), m_sound2(soundBuffer)
{
}

void Sound2D::updatePosition(const Vec2f& playerCenter)
{

}

void Sound2D::setSoundBuffer(const sf::SoundBuffer& soundBuffer)
{
	m_sound1.setBuffer(soundBuffer);
	m_sound2.setBuffer(soundBuffer);
}
