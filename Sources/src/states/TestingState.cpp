#include "states/TestingState.h"

#include "ALAGE/core/GApp.h"
#include "ALAGE/core/EventManager.h"
#include "ALAGE/core/StateManager.h"
#include "ALAGE/core/AssetHandler.h"

#include "ALAGE/gfx/TextureAsset.h"
#include "ALAGE/gfx/PBRTextureAsset.h"
#include "ALAGE/gfx/FontAsset.h"
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
    AssetHandler<PBRTextureAsset>* PBRTextureHandler =  AssetHandler<PBRTextureAsset>::Instance();

    sf::Image bluePixel;
    bluePixel.create(1,1,sf::Color::Blue);
    TextureHandler->SetDummyAsset(TextureAsset(bluePixel));


    m_mainScene.InitRenderer(m_manager->GetGApp()->GetWindowSize());
    m_mainScene.SetViewAngle({.xyAngle = 45, .zAngle=30});
    m_cameraNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_cameraNode->SetPosition(sf::Vector3f(1000,1000,750));
    m_camera = m_mainScene.CreateCamera(sf::Vector2f(
                             Config::GetInt("window","width",GApp::DEFAULT_WINDOW_WIDTH),
                             Config::GetInt("window","height",GApp::DEFAULT_WINDOW_HEIGHT)));
    m_cameraNode->AttachObject(m_camera);
    m_mainScene.SetCurrentCamera(m_camera);

    PBRTextureAsset *t3D =  PBRTextureHandler->LoadAssetFromFile("../data/sarcoXML.txt");

    TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_depth.png",LoadTypeInThread);
    TextureHandler->LoadAssetFromFile("../data/sand_normal.png",LoadTypeInThread);


    m_sarco3DNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarco3DNode->SetPosition(0,0,0);
    //Sprite3DEntity *sarco3DEntity = m_mainScene.CreateSprite3DEntity(sf::Vector2i(256,256));
    SpriteEntity *sarco3DEntity = m_mainScene.CreateIsoSpriteEntity();
    //sarco3DEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sarco-color.png"));
    sarco3DEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/torusbXML.txt",LoadTypeInThread));
    //sarco3DEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/treeXML.txt",LoadTypeInThread));
    //sarco3DEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/bedrockXML.txt",LoadTypeInThread));
    sarco3DEntity->SetCenter(128,128);
    //sarco3DEntity->SetShadowCastingType(DirectionnalShadow);
    sarco3DEntity->SetColor(sf::Color(32,196,255,160));
    //sarco3DEntity->DesactivateLighting();
    m_sarco3DNode->AttachObject(sarco3DEntity);

    m_sarcoNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarcoNode->SetPosition(-100,-30);
    IsoSpriteEntity *sarcoEntity = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarcoEntity->SetTexture(t3D);
    sarcoEntity->SetCenter(128,128);
    sarcoEntity->SetShadowCastingType(DirectionnalShadow);
    m_sarcoNode->AttachObject(sarcoEntity);


    IsoSpriteEntity *sarco3DEntitybis = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntitybis->SetTexture(t3D);
    //sarco3DEntitybis->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sandXML.txt"));
    sarco3DEntitybis->SetCenter(128,128);
    sarco3DEntitybis->SetShadowCastingType(DirectionnalShadow);
    m_sarcoNode->CreateChildNode(sf::Vector2f(-15,70))->AttachObject(sarco3DEntitybis);

    sarco3DEntitybis->SetScale(.75,.75);


    IsoSpriteEntity *sarco3DEntityThird = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntityThird->SetTexture(t3D);
    sarco3DEntityThird->SetCenter(128,128);
    sarco3DEntityThird->SetShadowCastingType(DirectionnalShadow);
    m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(100,100))->AttachObject(sarco3DEntityThird);

   /* Light* sunLight = m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(-1,.5,-1), sf::Color(255,255,160));
    sunLight->SetConstantAttenuation(2);*/
    m_sunLight = m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(-1,.2,-1), sf::Color(255,255,255));
    m_sunLight->SetIntensity(10);
    m_sunLight->EnableShadowCasting();
    m_mainScene.GetRootNode()->AttachObject(m_sunLight);
    m_mainScene.SetAmbientLight(sf::Color(96,127,255,96));
    //m_mainScene.SetAmbientLight(sf::Color(64,64,128,24));
    //m_mainScene.SetAmbientLight(sf::Color(96,96,128));
   // m_mainScene.SetAmbientLight(sf::Color(32,48,128));
   // m_mainScene.SetAmbientLight(sf::Color(96,96,128));
    //m_mainScene.GetRootNode()->AttachObject(m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(0,-1,0), sf::Color::Red));

    m_chene_node = m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(150,-100));
    IsoSpriteEntity *cheneEntity = m_mainScene.CreateIsoSpriteEntity();
    cheneEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/treeXML.txt"));
    //cheneEntity->SetCenter(192,320);
    cheneEntity->SetCenter(256,512);
    cheneEntity->SetShadowCastingType(DirectionnalShadow);
    m_chene_node->SetPosition(150,-100,-78);
    m_chene_node->AttachObject(cheneEntity);

    IsoSpriteEntity *abbayeEntity = m_mainScene.CreateIsoSpriteEntity();
    abbayeEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/abbayeXML.txt",LoadTypeInThread));
    abbayeEntity->SetCenter(960,540);
    abbayeEntity->SetShadowCastingType(DirectionnalShadow);
    m_mainScene.GetRootNode()->AttachObject(abbayeEntity);
    //m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(0,0))->AttachObject(abbayeEntity);


    /*IsoSpriteEntity *herbeEntity = m_mainScene.CreateIsoSpriteEntity();
    herbeEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/herbeXML.txt",LoadTypeInThread));
    herbeEntity->SetCenter(960,540);
    herbeEntity->SetShadowCastingType(DirectionnalShadow);
    m_mainScene.GetRootNode()->AttachObject(herbeEntity);*/



    SceneNode* rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    IsoRectEntity *rectEntity = m_mainScene.CreateIsoRectEntity(sf::Vector2f(2048,2038));
    rectEntity->SetCenter(sf::Vector2f(512,  512));
   // rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/cobble_color.png",LoadTypeInThread));
    //rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sandXML.txt",LoadTypeInThread));
    rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/cobbleXML.txt",LoadTypeInThread));
    //rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sandXML.txt",LoadTypeInThread));
    rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/wetsandXML.txt",LoadTypeInThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/cobble_color.png",LoadTypeInThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadTypeInThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand.png",LoadTypeInThread));
    rectEntity->SetTextureRect(sf::IntRect(0,0,2048,2048));
    rectNode->AttachObject(rectEntity);



    SceneNode *lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    Light* light = m_mainScene.CreateLight();
    light->SetDiffuseColor(sf::Color(255,190,64));
    light->SetIntensity(5);
    light->SetRadius(3);
    lightNode->SetPosition(130,720,75);
    lightNode->AttachObject(light);

    lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    light = m_mainScene.CreateLight();
    light->SetDiffuseColor(sf::Color(255,190,64));
    light->SetIntensity(5);
    light->SetRadius(3);
    lightNode->SetPosition(50,-175,75);
    lightNode->AttachObject(light);




    m_lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    Light* light2 = m_mainScene.CreateLight();
    light2->SetDiffuseColor(sf::Color(160,96,160));
   // light->SetConstantAttenuation(5);
    //light->SetLinearAttenuation(1);
    //light->SetQuadraticAttenuation(.00001);
    //light->SetQuadraticAttenuation(1);
    light2->SetIntensity(10);
    light2->SetRadius(3);
    m_lightNode->AttachObject(light2);

   // font.loadFromFile("../data/arial.ttf");
    m_fpsText.setFont(*AssetHandler<FontAsset>::Instance()->LoadAssetFromFile("../data/arial.ttf")->GetFont());
    //m_fpsText.setFont(font);
    m_fpsText.setCharacterSize(24);
    m_fpsText.setFillColor(sf::Color::White);
    m_fpsText.setPosition(0,0);

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


    if(event_manager->KeyPressed(sf::Keyboard::O))
        m_mainScene.SetSSAO(false);
    if(event_manager->KeyReleased(sf::Keyboard::O))
        m_mainScene.SetSSAO(true);


    if(event_manager->KeyPressed(sf::Keyboard::S))
        m_mainScene.SetShadowCasting(NoShadow);
    if(event_manager->KeyReleased(sf::Keyboard::S))
        m_mainScene.SetShadowCasting(DirectionnalShadow);


    if(event_manager->KeyPressed(sf::Keyboard::G))
        m_mainScene.DisableGammaCorrection();
    if(event_manager->KeyReleased(sf::Keyboard::G))
        m_mainScene.EnableGammaCorrection();


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
    ++m_fpsCounter;
    m_totalTime += time;

    m_cameraNode->Move(m_mainScene.ConvertCartesianToIso(m_camMove.x,m_camMove.y)*(500*time.asSeconds()));
  //  m_cameraNode->Move(sf::Vector3f(0,0,m_camMove.z)*(100*time.asSeconds()));
    //m_camera->Zoom((1-m_camMove.z*time.asSeconds()));

    m_cameraNode->Move(0,0,m_camMove.z *(500*time.asSeconds()));
    m_sarcoNode->Move(20*time.asSeconds(),0,0);

    m_mainScene.Update(time);
}

void TestingState::Draw(sf::RenderTarget* renderer)
{
    if(m_totalTime.asSeconds() > .1)
    {
        m_nbrFPS = (float)m_fpsCounter/m_totalTime.asSeconds();
        m_totalTime = sf::Time::Zero;
        m_fpsCounter = 0;
    }

    m_mainScene.RenderScene(renderer);

    std::ostringstream buf;
    buf<<m_nbrFPS;
    m_fpsText.setString(buf.str());
    renderer->resetGLStates();
    renderer->draw(m_fpsText);
}
