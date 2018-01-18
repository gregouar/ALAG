#include "states/TestingState.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"

#include "ALAGE/core/AssetHandler.h"
#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/gfx/Texture3DAsset.h"

using namespace alag;

TestingState::TestingState()
{
    //ctor
    showfirstsecond= true;
}

TestingState::~TestingState()
{
    //dtor
}

void TestingState::Entered()
{
    m_totalTime = sf::Time::Zero;

    AssetHandler<TextureAsset>* TextureHandler =  AssetHandler<TextureAsset>::Instance();
    /*TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/sarco-color.png");
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/sarco-normal.png");

    AssetTypeID newID = TextureHandler->GenerateID();
    TextureHandler->LoadAssetFromFile(newID,"../data/sarco-heightmap.png",LoadTypeInThread);
    TextureHandler->AddToObsolescenceList(newID,2);*/

    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/abbaye_color.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/abbaye_heightmap.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/abbaye_normal.png",LoadTypeInThread);



    AssetHandler<Texture3DAsset>* Texture3DHandler =  AssetHandler<Texture3DAsset>::Instance();
    Texture3DHandler->LoadAssetFromFile(Texture3DHandler->GenerateID(),
                                      "../data/sarcoXML.txt");
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
    if(m_totalTime.asSeconds() > 1)
    {
        showfirstsecond = false;
        m_totalTime = sf::Time::Zero;
        AssetHandler<TextureAsset>::Instance()->DescreaseObsolescenceLife();
    }
}
