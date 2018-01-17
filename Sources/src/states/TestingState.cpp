#include "TestingState.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/TextureAsset.h"
#include "../src/core/AssetHandler.cpp"

using namespace alag;

TestingState::TestingState()
{
    //ctor
}

TestingState::~TestingState()
{
    //dtor
}

void TestingState::Entered()
{
    m_totalTime = sf::Time::Zero;

    AssetHandler<TextureAsset>* TextureHandler =  AssetHandler<TextureAsset>::Instance();
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),"../data/sarco-color.png");
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),"../data/sarco-normal.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),"../data/sarco-heightmap.png",LoadTypeInThread);
}

void TestingState::Leaving()
{

}

void TestingState::Revealed()
{

}

void TestingState::Obscuring()
{

}

void TestingState::HandleEvents(alag::EventManager *event_manager)
{
    if(event_manager->KeyReleased(sf::Keyboard::Escape))
        m_manager->Switch(NULL);


    if(event_manager->IsAskingToClose())
        m_manager->Switch(NULL);
}

void TestingState::Update(sf::Time time)
{
    m_totalTime += time;

    /*if(m_totalTime.asSeconds() > 1)
        m_manager->Switch(NULL);*/
}

void TestingState::Draw(sf::RenderTarget* renderer)
{
    if(m_totalTime.asSeconds() < .2)
    std::cout<<m_totalTime.asSeconds()<<std::endl;
}
