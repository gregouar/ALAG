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
    //m_cameraNode->SetPosition(sf::Vector3f(1000,1000,750));
    m_cameraNode->SetPosition(sf::Vector3f(3000,3000,2500));
    m_camera = m_mainScene.CreateCamera(sf::Vector2f(
                             Config::GetInt("window","width",GApp::DEFAULT_WINDOW_WIDTH),
                             Config::GetInt("window","height",GApp::DEFAULT_WINDOW_HEIGHT)));
    m_cameraNode->AttachObject(m_camera);
    m_mainScene.SetCurrentCamera(m_camera);

    PBRTextureAsset *t3D =  PBRTextureHandler->LoadAssetFromFile("../data/sarcoXML.txt");

    TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadType_InThread);
    TextureHandler->LoadAssetFromFile("../data/sand_depth.png",LoadType_InThread);
    TextureHandler->LoadAssetFromFile("../data/sand_normal.png",LoadType_InThread);


    m_sarco3DNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarco3DNode->SetPosition(100,250,0);
    //Sprite3DEntity *sarco3DEntity = m_mainScene.CreateSprite3DEntity(sf::Vector2i(256,256));
    SpriteEntity *torusEntity = m_mainScene.CreateIsoSpriteEntity();
    //sarco3DEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sarco-color.png"));
    torusEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/torusbXML.txt",LoadType_InThread));
    //torusEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sarcoXML.txt",LoadType_InThread));
    //sarco3DEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/treeXML.txt",LoadType_InThread));
    //sarco3DEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/bedrockXML.txt",LoadType_InThread));
    //torusEntity->SetCenter(128,166);
    torusEntity->SetCenter(128,128);
    torusEntity->SetShadowCastingType(DirectionnalShadow);
    //torusEntity->SetColor(sf::Color(255,64,255,128));
    //torusEntity->SetColor(sf::Color(64,128,255,128));
    torusEntity->SetColor(sf::Color(128,128,128));
    torusEntity->SetStatic(false);
    //sarco3DEntity->DesactivateLighting();
    m_sarco3DNode->AttachObject(torusEntity);

    m_sarcoNode = m_mainScene.GetRootNode()->CreateChildNode();
    m_sarcoNode->SetPosition(-100,-30);
    IsoSpriteEntity *sarcoEntity = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarcoEntity->SetTexture(t3D);
    sarcoEntity->SetCenter(128,148);
    sarcoEntity->SetShadowCastingType(DirectionnalShadow);
    m_sarcoNode->AttachObject(sarcoEntity);


    IsoSpriteEntity *sarco3DEntitybis = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntitybis->SetTexture(t3D);
    //sarco3DEntitybis->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sandXML.txt"));
    //sarco3DEntitybis->SetCenter(128,148);
    sarco3DEntitybis->SetCenter(128,148);
    sarco3DEntitybis->SetShadowCastingType(DirectionnalShadow);
    m_sarcoNode->CreateChildNode(sf::Vector2f(-15,70))->AttachObject(sarco3DEntitybis);

    sarco3DEntitybis->SetScale(.75,.75);


    IsoSpriteEntity *sarco3DEntityThird = m_mainScene.CreateIsoSpriteEntity(sf::Vector2i(256,256));
    sarco3DEntityThird->SetTexture(t3D);
    sarco3DEntityThird->SetCenter(128,148);
    sarco3DEntityThird->SetShadowCastingType(DirectionnalShadow);
    sarco3DEntityThird->SetStatic(true);
    m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(100,100))->AttachObject(sarco3DEntityThird);

   /* Light* sunLight = m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(-1,.5,-1), sf::Color(255,255,160));
    sunLight->SetConstantAttenuation(2);*/
    m_sunLight = m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(-1 ,.2,-1), sf::Color(255,255,255));
    m_sunLight->SetIntensity(15);
    m_sunLight->EnableShadowCasting();
    m_mainScene.GetRootNode()->AttachObject(m_sunLight);
    m_mainScene.SetAmbientLight(sf::Color(96,127,255,160));
    //m_mainScene.SetAmbientLight(sf::Color(64,64,128,24));
    //m_mainScene.SetAmbientLight(sf::Color(96,96,128));
   // m_mainScene.SetAmbientLight(sf::Color(32,48,128));
   // m_mainScene.SetAmbientLight(sf::Color(96,96,128));
    //m_mainScene.GetRootNode()->AttachObject(m_mainScene.CreateLight(DirectionnalLight,sf::Vector3f(0,-1,0), sf::Color::Red));

    m_chene_node = m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(150,-100));
    IsoSpriteEntity *cheneEntity = m_mainScene.CreateIsoSpriteEntity();
    cheneEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/treeXML.txt"));
    cheneEntity->SetCenter(256,526);
    cheneEntity->SetShadowCastingType(DirectionnalShadow);
    cheneEntity->SetShadowVolumeType(TwoSidedShadow);
    cheneEntity->SetStatic(true);
    m_chene_node->SetPosition(150,-100,-78);
    m_chene_node->AttachObject(cheneEntity);

    /*for(int i = -10 ; i < 30 ; ++i)
    for(int j = -10 ; j < 30 ; ++j)
    {
        SceneNode *n = m_mainScene.GetRootNode()->CreateChildNode();
        IsoSpriteEntity *e = m_mainScene.CreateIsoSpriteEntity();
       // e->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/treeXML.txt"));
        e->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/torusbXML.txt"));
        //e->SetCenter(256,526);
        e->SetCenter(128,160);
        e->SetStatic(true);
        //e->SetShadowCastingType(DirectionnalShadow);
        //e->SetShadowVolumeType(TwoSidedShadow);
        //n->SetPosition(i*100,j*100,-78);
        n->SetPosition(i*50,j*50,0);
        n->AttachObject(e);
    }*/

    SceneNode* tree_shadow_node = m_chene_node->CreateChildNode(-12,-12,78);
    IsoGeometricShadowCaster* tree_dynamic_shadow = m_mainScene.CreateIsoGeometricShadowCaster();
    tree_dynamic_shadow->ConstructCylinder(12,70);
    tree_shadow_node->AttachObject(tree_dynamic_shadow);


    IsoSpriteEntity *abbayeEntity = m_mainScene.CreateIsoSpriteEntity();
    abbayeEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/abbayeXML.txt",LoadType_InThread));
    abbayeEntity->SetCenter(960,540);
    abbayeEntity->SetShadowCastingType(DirectionnalShadow);
    abbayeEntity->SetStatic(true);
    m_mainScene.GetRootNode()->AttachObject(abbayeEntity);
    //m_mainScene.GetRootNode()->CreateChildNode(sf::Vector2f(0,0))->AttachObject(abbayeEntity);


    /*IsoSpriteEntity *herbeEntity = m_mainScene.CreateIsoSpriteEntity();
    herbeEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/herbeXML.txt",LoadType_InThread));
    herbeEntity->SetCenter(960,540);
    herbeEntity->SetShadowCastingType(DirectionnalShadow);
    m_mainScene.GetRootNode()->AttachObject(herbeEntity);*/



    SceneNode* rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    IsoRectEntity *rectEntity = m_mainScene.CreateIsoRectEntity(sf::Vector2f(2048,2048));
    rectEntity->SetCenter(sf::Vector2f(512,  512));
   // rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/cobble_color.png",LoadType_InThread));
   // rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/waterXML.txt"));
   // rectEntity->SetColor(sf::Color(255,255,255,196));
    //rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/cobble2XML.txt",LoadType_InThread));
    //rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/sandXML.txt",LoadType_InThread));
    rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/wetsandXML.txt",LoadType_InThread));
    //rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/wallXML.txt",LoadType_InThread));
   // rectEntity->SetTexture(PBRTextureHandler->LoadAssetFromFile("../data/waterXML.txt",LoadType_InThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/cobble_color.png",LoadType_InThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand_color.png",LoadType_InThread));
    //rectEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/sand.png",LoadType_InThread));
    rectEntity->SetTextureRect(sf::IntRect(0,0,1024,1024));
    rectEntity->SetParallax(true);
    rectEntity->SetStatic(true);
    rectNode->AttachObject(rectEntity);


    rectNode = m_mainScene.GetRootNode()->CreateChildNode(0,0,10);
    IsoWaterEntity *waterEntity = m_mainScene.CreateIsoWaterEntity(sf::Vector2f(2048,2048));
    //waterEntity->SetTextureRect(sf::IntRect(0,0,1024,1024));
    waterEntity->SetWaterResolution(sf::Vector2u(1024,1024));
    waterEntity->SetCenter(sf::Vector2f(512,  512));
    waterEntity->SetHeightFactor(150.0);
    //waterEntity->SetTexture(TextureHandler->LoadAssetFromFile("../data/water_normal.png"));
    rectNode->SetPosition(0,0,0);
    rectNode->AttachObject(waterEntity);

    /*sf::FloatRect f = rectEntity->GetScreenBoundingRect(m_mainScene.GetIsoToCartMat());
    rectNode = m_mainScene.GetRootNode()->CreateChildNode();
    IsoSpriteEntity *r = m_mainScene.CreateIsoSpriteEntity();
    r->SetScale(f.width,f.height);
    sf::Vector2f v(f.left, f.top);
    rectNode->Move(m_mainScene.GetCartToIsoMat()*v);
    std::cout<<f.left<<" "<<f.top<<" "<<f.width<<" "<<f.height<<std::endl;
    r->SetTexture(TextureHandler->GetDummyAsset());
    rectNode->AttachObject(r);*/



    SceneNode* nodeGeoShaCas = m_mainScene.GetRootNode()->CreateChildNode(-86,-7);
    IsoGeometricShadowCaster* geoShaCas = m_mainScene.CreateIsoGeometricShadowCaster();
    geoShaCas->ConstructCube(20,20,55);
    nodeGeoShaCas->AttachObject(geoShaCas);


    nodeGeoShaCas = m_mainScene.GetRootNode()->CreateChildNode(-86,-147);
    geoShaCas = m_mainScene.CreateIsoGeometricShadowCaster();
    geoShaCas->ConstructCube(20,20,55);
    nodeGeoShaCas->AttachObject(geoShaCas);


    nodeGeoShaCas = m_mainScene.GetRootNode()->CreateChildNode(-86,136);
    geoShaCas = m_mainScene.CreateIsoGeometricShadowCaster();
    geoShaCas->ConstructCube(20,20,55);
    nodeGeoShaCas->AttachObject(geoShaCas);



    SceneNode *lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    Light* light = m_mainScene.CreateLight();
    light->SetDiffuseColor(sf::Color(255,190,64));
    light->SetIntensity(5);
    light->SetRadius(300);
    lightNode->SetPosition(130,720,75);
    lightNode->AttachObject(light);

    lightNode = m_mainScene.GetRootNode()->CreateChildNode();
    light = m_mainScene.CreateLight();
    light->SetDiffuseColor(sf::Color(255,190,64));
    light->SetIntensity(5);
    light->SetRadius(300);
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
    light2->SetRadius(300);
    light2->EnableShadowCasting();
    light2->SetShadowMapSize(256,256);
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

    if(event_manager->KeyPressed(sf::Keyboard::B))
        m_mainScene.SetBloom(false);
    if(event_manager->KeyReleased(sf::Keyboard::B))
        m_mainScene.SetBloom(true);


    if(event_manager->KeyPressed(sf::Keyboard::S))
        m_mainScene.SetShadowCasting(NoShadow);
    if(event_manager->KeyReleased(sf::Keyboard::S))
        m_mainScene.SetShadowCasting(AllShadows);

    if(event_manager->KeyPressed(sf::Keyboard::R))
        m_mainScene.SetSSR(false);
    if(event_manager->KeyReleased(sf::Keyboard::R))
        m_mainScene.SetSSR(true);


    if(event_manager->KeyPressed(sf::Keyboard::G))
        m_mainScene.DisableGammaCorrection();
    if(event_manager->KeyReleased(sf::Keyboard::G))
        m_mainScene.EnableGammaCorrection();


    if(event_manager->MouseButtonIsPressed(sf::Mouse::Left))
    {
        sf::Vector2i p(event_manager->MousePosition());
        m_sarco3DNode->SetPosition(m_mainScene.ConvertMouseToScene(p));
      //  std::cout<<m_mainScene.ConvertMouseToScene(p).x<<" "
               // <<m_mainScene.ConvertMouseToScene(p).y<<std::endl;
    }

    if(event_manager->MouseButtonIsPressed(sf::Mouse::Right))
    {
        sf::Vector2i p(event_manager->MousePosition());
        m_chene_node->SetPosition(m_mainScene.ConvertMouseToScene(p));
        //m_cameraNode->SetPosition(m_mainScene.ConvertMouseToScene(p)+sf::Vector2f(3000,3000));
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

    if(time.asMicroseconds() > m_worstTime.asMicroseconds())
        m_worstTime = time;

    //m_chene_node->Move(m_mainScene.ConvertCartesianToIso(m_camMove.x,m_camMove.y)*(500*time.asSeconds()));
    m_cameraNode->Move(m_mainScene.ConvertCartesianToIso(m_camMove.x,m_camMove.y)*(500*time.asSeconds()));
    //m_cameraNode->Move(sf::Vector3f(0,0,m_camMove.z)*(100*time.asSeconds()));
    m_camera->Zoom((1-m_camMove.z*time.asSeconds()));

  //  m_cameraNode->Move(0,0,m_camMove.z *(500*time.asSeconds()));

    m_sarcoNode->Move(20*time.asSeconds(),0,0);

    m_mainScene.Update(time);
}

void TestingState::Draw(sf::RenderTarget* renderer)
{

    m_mainScene.RenderScene(renderer);

    std::ostringstream buf;
    buf<<m_nbrFPS<<" ("<<1.0/m_worstTime.asSeconds()<<")";
    m_fpsText.setString(buf.str());
    renderer->resetGLStates();
    renderer->draw(m_fpsText);

    if(m_totalTime.asSeconds() > 1.0)
    {
        m_nbrFPS = (float)m_fpsCounter/m_totalTime.asSeconds();
        m_totalTime = sf::Time::Zero;
        m_fpsCounter = 0;
        m_worstTime = sf::Time::Zero;
    }
}
