#include "ALAGE/core/GState.h"

namespace alag
{

GState::GState()
{
    //ctor
}

GState::~GState()
{
    //dtor
}

void GState::SetManager(StateManager *manager)
{
    m_manager = manager;
}

void GState::Pause()
{
    m_running = false;
}

void GState::Resume()
{
    m_running = true;
}

}
