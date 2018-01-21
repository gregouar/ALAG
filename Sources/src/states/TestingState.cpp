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
    showfirstsecond = true;
    m_firstEntering = true;
}

TestingState::~TestingState()
{
    //dtor
}

void TestingState::Init()
{
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
    TextureHandler->LoadAssetFromFile(TextureHandler->GenerateID(),
                                      "../data/abbaye_color.png",LoadTypeInThread);

    AssetHandler<Texture3DAsset>* Texture3DHandler =  AssetHandler<Texture3DAsset>::Instance();
    Texture3DAsset *t3D =  Texture3DHandler->LoadAssetFromFile(Texture3DHandler->GenerateID(),
                                      "../data/sarcoXML.txt");

    m_mainScene.SetViewAngle({.xyAngle = 45, .zAngle=30});
    m_mainScene.InitRenderer();

    SceneNode *rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    RectEntity *rectEntity = m_mainScene.CreateRectEntity(sf::Rect(0,0,128,128));
    rectEntity.SetTexture(t3D);
    rectNode->AttachEntity(rectEntity);

    m_firstEntering = false;
}

void TestingState::Entered()
{
    m_totalTime = sf::Time::Zero;

    if(m_firstEntering)
        Init();
}

void TestingState::Leaving()
{
    AssetHandler<Texture3DAsset>::Instance()->CleanAll();
    AssetHandler<TextureAsset>::Instance()->CleanAll();
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
        m_manager->Switch(nullptr);


    if(event_manager->IsAskingToClose())
        m_manager->Switch(nullptr);
}

void TestingState::Update(sf::Time time)
{
    m_totalTime += time;
}

void TestingState::Draw(sf::RenderTarget* renderer)
{
    if(m_totalTime.asSeconds() > 1)
    {
        showfirstsecond = false;
        m_totalTime = sf::Time::Zero;
        AssetHandler<TextureAsset>::Instance()->DescreaseObsolescenceLife();
    }

    m_mainScene.RenderScene(renderer);
}
