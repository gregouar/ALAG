#include "states/TestingState.h"

#include "ALAGE/core/GApp.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"
#include "ALAGE/core/AssetHandler.h"

#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/gfx/Texture3DAsset.h"


#include "ALAGE/core/Config.h"

using namespace alag;

TestingState::TestingState()
{
    //ctor
    showfirstsecond = true;
    m_firstEntering = true;

    m_camMove.x = 0;
    m_camMove.y = 0;
}

TestingState::~TestingState()
{
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

    TextureHandler->LoadAssetFromFile("../data/abbaye_heightmap.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/abbaye_normal.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/abbaye_color.png",LoadTypeInThread);

    AssetHandler<Texture3DAsset>* Texture3DHandler =  AssetHandler<Texture3DAsset>::Instance();
    Texture3DAsset *t3D =  Texture3DHandler->LoadAssetFromFile("../data/sarcoXML.txt");

    TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_depth.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_normal.png",LoadTypeInThread);

    m_mainScene.SetViewAngle({.xyAngle = 45, .zAngle=30});
    m_mainScene.InitRenderer(Config::GetInt("window","width",GApp::DEFAULT_WINDOW_WIDTH),
                             Config::GetInt("window","height",GApp::DEFAULT_WINDOW_HEIGHT));

    SceneNode* rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    RectEntity *rectEntity = m_mainScene.CreateRectEntity(sf::Vector2f(1024,1024));
    rectEntity->SetCenter(sf::Vector2f(512,  512));
    rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand.png",LoadTypeInThread));
    rectNode->AttachEntity(rectEntity);

    m_sarcoNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarcoNode->SetPosition(-400,0);
    SpriteEntity *sarcoEntity = m_mainScene.CreateSpriteEntity(sf::Vector2i(256,256));
    sarcoEntity->SetTexture(t3D);
    m_sarcoNode->AttachEntity(sarcoEntity);

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

    if(event_manager->KeyIsPressed(sf::Keyboard::Left))
        m_camMove.x = -1;
    else if(event_manager->KeyIsPressed(sf::Keyboard::Right))
        m_camMove.x = 1;
    else
        m_camMove.x = 0;

    if(event_manager->KeyIsPressed(sf::Keyboard::Up))
        m_camMove.y = -1;
    else if(event_manager->KeyIsPressed(sf::Keyboard::Down))
        m_camMove.y = 1;
    else
        m_camMove.y = 0;

    if(event_manager->IsAskingToClose())
        m_manager->Switch(nullptr);
}

void TestingState::Update(sf::Time time)
{
    m_totalTime += time;

    m_mainScene.MoveView(m_mainScene.ConvertCartesianToIso(m_camMove)*(100*time.asSeconds()));
    m_sarcoNode->Move(20*time.asSeconds(),0,0);

    m_mainScene.Update(time);
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
