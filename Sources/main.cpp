#include <iostream>
#include "ALAGE/core/GApp.h"
#include "ALAGE/utils/Logger.h"
#include "include/states/TestingState.h"

using namespace std;

int main()
{
    //Add logger to the bottom of singletons list, so that it will be killed last
    Logger::Instance();

    alag::GApp app("ALAG");

    return app.Run(TestingState::Instance());
}
