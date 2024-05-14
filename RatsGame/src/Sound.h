#pragma once

bool loadSound(const char* filepath, ALuint* buffer);

class SoundBuffer
{
public:
    ~SoundBuffer()
    {
        alDeleteBuffers(1, &m_buffer);
    }

    inline bool loadFromFile(const char* filepath)
    {
        return loadSound(filepath, &m_buffer);
    }

    inline auto getBuffer() const { return m_buffer; }
private:
    ALuint m_buffer;
};

class Sound
{
public:
    Sound(const SoundBuffer& buffer);
    ~Sound();

    void setBuffer(const SoundBuffer& buffer);
    void play();
    bool isPlaying() const;
    void updatePosition(float dist, float angle);
private:
    ALuint m_sources[2];
};