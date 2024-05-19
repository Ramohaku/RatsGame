#include "stdafx.h"

#include "Application.h"
#include "Debug.h"



int main(void)
{
    std::unique_ptr<Application> application = std::make_unique<Application>();
    application->run();

    return 0;
}