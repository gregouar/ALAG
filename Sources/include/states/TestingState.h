#ifndef TESTINGSTATE_H
#define TESTINGSTATE_H

#include "ALAGE/core/GState.h"
#include "ALAGE/utils/singleton.h"
#include "ALAGE/gfx/iso/PBRIsoScene.h"


class TestingState : public alag::GState, public Singleton<TestingState>
{
    friend class Singleton<TestingState>;

    public:
        void Entered();
        void Leaving();
        void Revealed();
        void Obscuring();

        void HandleEvents(alag::EventManager*);
        void Update(sf::Time);
        void Draw(sf::RenderTarget*);

        void Init();

    protected:
        TestingState();
        virtual ~TestingState();

    private:
        sf::Time m_totalTime;
        bool m_firstEntering;
        bool showfirstsecond;

        alag::PBRIsoScene m_mainScene;

        sf::Text m_fpsText;
        float m_nbrFPS, m_fpsCounter;
        sf::Time m_worstTime;

        sf::Vector3f m_camMove;

        alag::SceneNode *m_cameraNode;
        alag::SceneNode *m_sarcoNode;
        alag::SceneNode *m_sarco3DNode;
        alag::SceneNode *m_chene_node;
        alag::SceneNode *m_lightNode;

        alag::Light *m_sunLight;

        alag::Camera *m_camera;
};

#endif // TESTINGSTATE_H
