#include "stdafx.h"
#include "Sound.h"

bool loadSound(const char* filepath, ALuint* buffer)
{
    SNDFILE* sndFile;
    SF_INFO sfInfo;

    sndFile = sf_open(filepath, SFM_READ, &sfInfo);
    if (!sndFile)
    {
#if _DEBUG
        std::cerr << "Cannot open file " << filepath << '\n';
#endif
        return false;
    }

    std::vector<ALshort> samples(sfInfo.frames * sfInfo.channels);

    sf_count_t readCount = sf_readf_short(sndFile, samples.data(), sfInfo.frames);
    if (readCount < 1)
    {
#if _DEBUG
        std::cerr << "Cannot read data from file " << filepath << '\n';
#endif
        sf_close(sndFile);
        return false;
    }

    sf_close(sndFile);

    alGenBuffers(1, buffer);
    alBufferData(*buffer, sfInfo.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
        samples.data(), samples.size() * sizeof(ALshort), sfInfo.samplerate);

    return true;
}

Sound::Sound()
{
    alGenSources(2, m_sources);
}

Sound::Sound(const SoundBuffer& buffer)
{
    alGenSources(2, m_sources);

    alSourcei(m_sources[0], AL_BUFFER, buffer.getBuffer());
    alSourcei(m_sources[1], AL_BUFFER, buffer.getBuffer());
}

Sound::~Sound()
{
    alDeleteSources(2, m_sources);
}

void Sound::setBuffer(const SoundBuffer& buffer)
{
    alSourcei(m_sources[0], AL_BUFFER, buffer.getBuffer());
    alSourcei(m_sources[1], AL_BUFFER, buffer.getBuffer());
}

void Sound::play()
{
    alSourcePlay(m_sources[0]);
    alSourcePlay(m_sources[1]);
}

bool Sound::isPlaying() const
{
    ALint state;
    alGetSourcei(m_sources[0], AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void Sound::updatePosition(float dist, float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    const ALfloat position1[] = { s, 0.0f, c };
    const ALfloat position2[] = { -s, 0.0f, -c };

    alSourcefv(m_sources[0], AL_POSITION, position1);
    alSourcefv(m_sources[1], AL_POSITION, position2);

    const ALfloat volume0 = std::max(atan(-dist / 18.0f) + 1.0f, 0.0f);

    const float base = dist / 5.0f;
    const ALfloat volume1 = std::max(-(base * base) + 1.0f, 0.0f);

    alSourcef(m_sources[0], AL_GAIN, volume0);
    alSourcef(m_sources[1], AL_GAIN, volume1);
}
