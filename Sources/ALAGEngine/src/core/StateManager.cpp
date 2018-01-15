#include <stdexcept>

#include "StateManager.h"

namespace alag
{

StateManager::StateManager()
{
    //ctor
}

StateManager::~StateManager()
{
    //dtor
}

void StateManager::Switch(GState* state)
{
    for(std::size_t i = 0; i < m_states.size() ; i++)
        m_states[i]->Leaving();

    m_states.clear();

    if(state != NULL)
    {
        m_states.push_back(state);
        m_states.back()->SetManager(this);
        m_states.back()->Entered();
    }
}

void StateManager::Push(GState* state)
{
    if(m_states.back())
        m_states.back()->Obscuring();
    m_states.push_back(state);
    m_states.back()->SetManager(this);
    m_states.back()->Entered();
}

GState* StateManager::Pop()
{
    if(m_states.empty())
      throw std::runtime_error("Attempted to pop from an empty game state stack");

    m_states.back()->Leaving();
    m_states.pop_back();

    if(m_states.empty())
        return (NULL);

    m_states.back()->Revealed();
    return m_states.back();
}

GState* StateManager::Peek()
{
    if(m_states.empty())
        return (NULL);
    return m_states.back();
}


void StateManager::HandleEvents(EventManager* events_manager)
{
    for(std::size_t i = 0; i < m_states.size() ; i++)
        m_states[i]->HandleEvents(events_manager);
}

void StateManager::Update(sf::Time time)
{
    for(std::size_t i = 0; i < m_states.size() ; i++)
        m_states[i]->Update(time);
}

void StateManager::Draw()
{
    for(std::size_t i = 0; i < m_states.size() ; i++)
        m_states[i]->Draw();
}

}
