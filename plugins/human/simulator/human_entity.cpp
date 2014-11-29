/*
 * human_entity.cpp
 *
 *  Created on: 5 nov. 2014
 *      Author: anthonydebruyn
 */

#include "human_entity.h"
#include <argos3/core/utility/math/matrix/rotationmatrix3.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/core/simulator/entity/controllable_entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>

namespace argos {

    /****************************************/
    /****************************************/

    /* Humans have wheels. They just do not know it yet. */
    static const Real INTERWHEEL_DISTANCE               = 0.053f;
    static const Real HALF_INTERWHEEL_DISTANCE          = INTERWHEEL_DISTANCE * 0.5f;
    static const Real WHEEL_RADIUS                      = 0.0205f;

    /****************************************/
    /****************************************/

    CHumanEntity::CHumanEntity() :
                                      CComposableEntity(NULL),
                                      m_pcControllableEntity(NULL),
                                      m_pcEmbodiedEntity(NULL),
                                      m_pcWheeledEntity(NULL),
                                      m_fRadius(0.4f),
                                      m_fHeight(1.8f),
                                      m_fMass(80.0f),
                                      m_fIntensity(1.0f) {
    }

    /****************************************/
    /****************************************/

    void CHumanEntity::Init(TConfigurationNode& t_tree) {
        try {
            /*
             * Init parent
             */
            CComposableEntity::Init(t_tree);
            /*
             * Create and init components
             */

            /* Parse XML to get the radius */
            GetNodeAttribute(t_tree, "radius", m_fRadius);
            /* Parse XML to get the height */
            GetNodeAttribute(t_tree, "height", m_fHeight);
            /* Parse XML to get the mass */
            GetNodeAttribute(t_tree, "mass", m_fMass);
            /* Parse XML to get the intensity */
            GetNodeAttribute(t_tree, "intensity", m_fIntensity);

            /* Embodied entity */
            m_pcEmbodiedEntity = new CEmbodiedEntity(this);
            AddComponent(*m_pcEmbodiedEntity);
            m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));

            /* Wheeled entity and wheel positions (left, right) */
            m_pcWheeledEntity = new CWheeledEntity(this, "wheels_0", 2);
            AddComponent(*m_pcWheeledEntity);
            m_pcWheeledEntity->SetWheel(0, CVector3(0.0f,  HALF_INTERWHEEL_DISTANCE, 0.0f), WHEEL_RADIUS);
            m_pcWheeledEntity->SetWheel(1, CVector3(0.0f, -HALF_INTERWHEEL_DISTANCE, 0.0f), WHEEL_RADIUS);

            /* Controllable entity
                    It must be the last one, for actuators/sensors to link to composing entities correctly */
            m_pcControllableEntity = new CControllableEntity(this);
            AddComponent(*m_pcControllableEntity);
            m_pcControllableEntity->Init(GetNode(t_tree, "controller"));

            /* Update components */
            UpdateComponents();
        }
        catch(CARGoSException& ex) {
            THROW_ARGOSEXCEPTION_NESTED("Failed to initialize human entity \"" << GetId() << "\".", ex);
        }
    }

    /****************************************/
    /****************************************/

    void CHumanEntity::Reset() {
        /* Reset all components */
        CComposableEntity::Reset();
        /* Update components */
        UpdateComponents();
    }

    /****************************************/
    /****************************************/

    void CHumanEntity::Destroy() {
        CComposableEntity::Destroy();
    }

    /****************************************/
    /****************************************/

#define UPDATE(COMPONENT) if(COMPONENT->IsEnabled()) COMPONENT->Update();

    void CHumanEntity::UpdateComponents() {
    }

    /****************************************/
    /****************************************/

    REGISTER_ENTITY(CHumanEntity,
            "human",
            "Anthony Debruyn",
            "1.0",
            "Normal human.",
            "REQUIRED XML CONFIGURATION\n\n"
            "  <arena ...>\n"
            "    ...\n"
            "    <human id=\"h0\" radius=\"0.3\" height=\"1.8\" mass=\"80.0\" >\n"
            "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
            "      <controller config=\"mycntrl\" />\n"
            "    </human>\n"
            "    ...\n"
            "  </arena>\n\n"
            "The 'id' attribute is necessary and must be unique among the entities. If two\n"
            "entities share the same id, initialization aborts.\n"
            "The 'body/position' attribute specifies the position of the pucktom point of the\n"
            "human in the arena. When the human is untranslated and unrotated, the\n"
            "pucktom point is in the origin and it is defined as the middle point between\n"
            "the two wheels on the XY plane and the lowest point of the human on the Z\n"
            "axis, that is the point where the wheels touch the floor. The attribute values\n"
            "are in the X,Y,Z order.\n"
            "The 'body/orientation' attribute specifies the orientation of the human. All\n"
            "rotations are performed with respect to the pucktom point. The order of the\n"
            "angles is Z,Y,X, which means that the first number corresponds to the rotation\n"
            "around the Z axis, the second around Y and the last around X. This reflects\n"
            "the internal convention used in ARGoS, in which rotations are performed in\n"
            "that order. Angles are expressed in degrees. When the human is unrotated, it\n"
            "is oriented along the X axis.\n"
            "The 'controller/config' attribute is used to assign a controller to the\n"
            "human. The value of the attribute must be set to the id of a previously\n"
            "defined controller. Controllers are defined in the <controllers> XML subtree.\n\n"
            "OPTIONAL XML CONFIGURATION\n\n"
            "None for the time being.\n",
            "Under development"
    );

    /****************************************/
    /****************************************/

    REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CHumanEntity);

    /****************************************/
    /****************************************/

} /* namespace argos */
