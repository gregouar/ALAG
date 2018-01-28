#include "states/TestingState.h"

#include "ALAGE/core/GApp.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"
#include "ALAGE/core/AssetHandler.h"

#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/gfx/Texture3DAsset.h"
#include "ALAGE/gfx/iso/IsoSpriteEntity.h"


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
    AssetHandler<Texture3DAsset>* Texture3DHandler =  AssetHandler<Texture3DAsset>::Instance();

    sf::Image bluePixel;
    bluePixel.create(1,1,sf::Color::Blue);
    TextureHandler->SetDummyAsset(TextureAsset(bluePixel));


    m_mainScene.SetViewAngle({.xyAngle = 45, .zAngle=30});
    m_cameraNode = m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(0,0));
    m_camera = m_mainScene.CreateCamera(sf::Vector2f(
                             Config::GetInt("window","width",GApp::DEFAULT_WINDOW_WIDTH),
                             Config::GetInt("window","height",GApp::DEFAULT_WINDOW_HEIGHT)));
    m_cameraNode->AttachObject(m_camera);
    m_mainScene.SetCurrentCamera(m_camera);

    m_mainScene.SetAmbientLight(sf::Color(32,48,128));

    Texture3DAsset *t3D =  Texture3DHandler->LoadAssetFromFile("../data/sarcoXML.txt");

    TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_depth.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_normal.png",LoadTypeInThread);


    SceneNode* rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    RectEntity *rectEntity = m_mainScene.CreateRectEntity(sf::Vector2f(1024,1024));
    rectEntity->SetCenter(sf::Vector2f(512,  512));
   // rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/cobble_color.png",LoadTypeInThread));
    //rectEntity->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/wallXML.txt",LoadTypeInThread));
    //rectEntity->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/cobbleXML.txt",LoadTypeInThread));
    rectEntity->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/sandXML.txt",LoadTypeInThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadTypeInThread));
  //  rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand.png",LoadTypeInThread));
   // rectEntity->SetTextureRect(sf::IntRect(0,0,4096,4096));
    rectNode->AttachObject(rectEntity);

    m_sarcoNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarcoNode->SetPosition(-100,-30);
    IsoSpriteEntity *sarcoEntity = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarcoEntity->SetTexture(t3D);
    sarcoEntity->SetCenter(128,128);
    m_sarcoNode->AttachObject(sarcoEntity);

    m_sarco3DNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarco3DNode->SetPosition(0,0);
    //Sprite3DEntity *sarco3DEntity = m_mainScene.CreateSprite3DEntity(sf::Vector2i(256,256));
    SpriteEntity *sarco3DEntity = m_mainScene.CreateIsoSpriteEntity();
    //sarco3DEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sarco-color.png"));
    sarco3DEntity->SetTexture(t3D);
    sarco3DEntity->SetCenter(128,160);
   // sarco3DEntity->DesactivateLighting();
    m_sarco3DNode->AttachObject(sarco3DEntity);


    IsoSpriteEntity *sarco3DEntitybis = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntitybis->SetTexture(t3D);
    //sarco3DEntitybis->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/sandXML.txt"));
    sarco3DEntitybis->SetCenter(128,128);
    m_sarcoNode->CreateChildNode(sf::Vector2f(-15,70))->AttachObject(sarco3DEntitybis);

    sarco3DEntitybis->scale(.75,.75);


    IsoSpriteEntity *sarco3DEntityThird = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntityThird->SetTexture(t3D);
    sarco3DEntityThird->SetCenter(128,128);
    m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(-100,100))->AttachObject(sarco3DEntityThird);

    Light* sunLight = m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(-1,.5,-1), sf::Color(255,255,160));
    sunLight->SetConstantAttenuation(2);
    m_mainScene.GetRootNode()->AttachObject(sunLight);
    //m_mainScene.GetRootNode()->AttachObject(m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(0,-1,0), sf::Color::Red));

    m_chene_node = m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(150,-100));
    IsoSpriteEntity *cheneEntity = m_mainScene.CreateIsoSpriteEntity();
    cheneEntity->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/cheneXML.txt"));
    cheneEntity->SetCenter(192,320);
    m_chene_node->AttachObject(cheneEntity);

    IsoSpriteEntity *abbayeEntity = m_mainScene.CreateIsoSpriteEntity();
    abbayeEntity->SetTexture(Texture3DHandler->LoadAssetFromFile("../data/abbayeXML.txt"));
    abbayeEntity->SetCenter(960,540);
    m_mainScene.GetRootNode()->AttachObject(abbayeEntity);
    //m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(0,0))->AttachObject(abbayeEntity);


    m_lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    Light* light = m_mainScene.CreateLight();
    light->SetDiffuseColor(sf::Color(255,190,64));
    light->SetLinearAttunation(.00001);
    light->SetQuadraticAttenuation(.00001);
    m_lightNode->AttachObject(light);

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

    if(event_manager->KeyIsPressed(sf::Keyboard::PageUp))
        m_camMove.z = 1;
    else if(event_manager->KeyIsPressed(sf::Keyboard::PageDown))
        m_camMove.z = -1;
    else
        m_camMove.z = 0;


    if(event_manager->KeyPressed(sf::Keyboard::A))
        m_sarco3DNode->Move(0,0,10);
    if(event_manager->KeyPressed(sf::Keyboard::Q))
        m_sarco3DNode->Move(0,0,-10);


    if(event_manager->MouseButtonIsPressed(sf::Mouse::Left))
    {
        sf::Vector2i p(event_manager->MousePosition());
        m_sarco3DNode->SetPosition(m_mainScene.ConvertMouseToScene(p));
    }

    if(event_manager->MouseButtonIsPressed(sf::Mouse::Right))
    {
        sf::Vector2i p(event_manager->MousePosition());
        m_chene_node->SetPosition(m_mainScene.ConvertMouseToScene(p));
    }


    sf::Vector2f p = m_mainScene.ConvertMouseToScene(event_manager->MousePosition());

    m_lightNode->SetPosition(p.x,p.y,50);

    if(event_manager->IsAskingToClose())
        m_manager->Switch(nullptr);
}

void TestingState::Update(sf::Time time)
{
    m_totalTime += time;

    m_cameraNode->Move(m_mainScene.ConvertCartesianToIso(m_camMove.x,m_camMove.y)*(500*time.asSeconds()));
  //  m_cameraNode->Move(sf::Vector3f(0,0,m_camMove.z)*(100*time.asSeconds()));
    m_camera->Zoom((1-m_camMove.z*time.asSeconds()));
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
