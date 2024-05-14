#include "stdafx.h"

#include "Application.h"
#include "Debug.h"



int main(void)
{
    // Initialize OpenAL
    ALCdevice* device = alcOpenDevice(NULL);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return 1;
    }

    ALCcontext* context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    std::unique_ptr<Application> application = std::make_unique<Application>();
    application->run();

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}