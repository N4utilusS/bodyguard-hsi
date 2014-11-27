/*
 * human_default_sensor.cpp
 *
 *  Created on: 25 nov. 2014
 *      Author: anthonydebruyn
 */

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/plugins/human/simulator/human_entity.h>
#include <argos3/plugins/simulator/entities/human_sensor_equipped_entity.h>

#include "human_default_sensor.h"

namespace argos {

    /****************************************/
    /****************************************/

    static CRange<Real> UNIT(0.0f, 1.0f);

    /****************************************/
    /****************************************/

    CHumanDefaultSensor::CHumanDefaultSensor() :
               m_pcEmbodiedEntity(NULL),
               m_bShowRays(false),
               m_pcRNG(NULL),
               m_bAddNoise(false),
               m_cSpace(CSimulator::GetInstance().GetSpace()) {}

    /****************************************/
    /****************************************/

    void CHumanDefaultSensor::SetRobot(CComposableEntity& c_entity) {
        try {
            m_pcEmbodiedEntity = &(c_entity.GetComponent<CEmbodiedEntity>("body"));
            m_pcControllableEntity = &(c_entity.GetComponent<CControllableEntity>("controller"));
            m_pcHumanEntity = &(c_entity.GetComponent<CLightSensorEquippedEntity>("human_sensors"));
            m_pcHumanEntity->SetCanBeEnabledIfDisabled(true);
            m_pcHumanEntity->Enable();
        }
        catch(CARGoSException& ex) {
            THROW_ARGOSEXCEPTION_NESTED("Can't set robot for the human default sensor", ex);
        }
    }

    /****************************************/
    /****************************************/

    void CHumanDefaultSensor::Init(TConfigurationNode& t_tree) {
        try {
            CCI_LightSensor::Init(t_tree);
            /* Show rays? */
            GetNodeAttributeOrDefault(t_tree, "show_rays", m_bShowRays, m_bShowRays);
            /* Parse noise level */
            Real fNoiseLevel = 0.0f;
            GetNodeAttributeOrDefault(t_tree, "noise_level", fNoiseLevel, fNoiseLevel);
            if(fNoiseLevel < 0.0f) {
                THROW_ARGOSEXCEPTION("Can't specify a negative value for the noise level of the human sensor");
            }
            else if(fNoiseLevel > 0.0f) {
                m_bAddNoise = true;
                m_cNoiseRange.Set(-fNoiseLevel, fNoiseLevel);
                m_pcRNG = CRandom::CreateRNG("argos");
            }
            m_tReadings.resize(m_pcHumanEntity->GetNumSensors());
        }
        catch(CARGoSException& ex) {
            THROW_ARGOSEXCEPTION_NESTED("Initialization error in default human sensor", ex);
        }
    }

    /****************************************/
    /****************************************/

    void CHumanDefaultSensor::Update() {
        /* Erase readings */
        for(size_t i = 0; i < m_tReadings.size(); ++i)  m_tReadings[i] = 0.0f;
        /* Ray used for scanning the environment for obstacles */
        CRay3 cScanningRay;
        CVector3 cRayStart;
        CVector3 cSensorToHuman;
        /* Buffers to contain data about the intersection */
        SEmbodiedEntityIntersectionItem sIntersection;
        /* Get the map of human entities */
        CSpace::TMapPerTypePerId::iterator itHumans = m_cSpace.GetEntityMapPerTypePerId().find("human");
        if (itHumans != m_cSpace.GetEntityMapPerTypePerId().end()) {
            CSpace::TMapPerType& mapHumans = itHumans->second;
            /* Go through the sensors */
            for(UInt32 i = 0; i < m_tReadings.size(); ++i) {
                /* Set ray start */
                cRayStart = m_pcHumanEntity->GetSensor(i).Position;
                cRayStart.Rotate(m_pcEmbodiedEntity->GetOrientation());
                cRayStart += m_pcEmbodiedEntity->GetPosition();
                /* Go through all the human entities */
                for(CSpace::TMapPerType::iterator it = mapHumans.begin();
                        it != mapHumans.end();
                        ++it) {
                    /* Get a reference to the human */
                    CHumanEntity& cHuman = *any_cast<CHumanEntity*>(it->second);
                    /* Consider the human only if it has non zero intensity */
                    if(cHuman.GetIntensity() > 0.0f) {
                        /* Set ray end to human position */
                        cScanningRay.Set(cRayStart, cHuman.GetPosition());
                        /* Check occlusions */
                        if(! GetClosestEmbodiedEntityIntersectedByRay(sIntersection,
                                cScanningRay)) {
                            /* No occlusion, the human is visibile */
                            if(m_bShowRays) {
                                m_pcControllableEntity->AddCheckedRay(false, cScanningRay);
                            }
                            /* Calculate reading */
                            cScanningRay.ToVector(cSensorToHuman);
                            m_tReadings[i] += CalculateReading(cSensorToHuman.Length(),
                                    cHuman.GetIntensity());
                        }
                        else {
                            /* There is an occlusion, the human is not visible */
                            if(m_bShowRays) {
                                m_pcControllableEntity->AddIntersectionPoint(cScanningRay,
                                        sIntersection.TOnRay);
                                m_pcControllableEntity->AddCheckedRay(true, cScanningRay);
                            }
                        }
                    }
                }
                /* Apply noise to the sensor */
                if(m_bAddNoise) {
                    m_tReadings[i] += m_pcRNG->Uniform(m_cNoiseRange);
                }
                /* Trunc the reading between 0 and 1 */
                UNIT.TruncValue(m_tReadings[i]);
            }
        }
        else {
            /* There are no humans in the environment */
            if(m_bAddNoise) {
                /* Go through the sensors */
                for(UInt32 i = 0; i < m_tReadings.size(); ++i) {
                    /* Apply noise to the sensor */
                    m_tReadings[i] += m_pcRNG->Uniform(m_cNoiseRange);
                    /* Trunc the reading between 0 and 1 */
                    UNIT.TruncValue(m_tReadings[i]);
                }
            }
        }
    }

    /****************************************/
    /****************************************/

    void CHumanDefaultSensor::Reset() {
        for(UInt32 i = 0; i < GetReadings().size(); ++i) {
            m_tReadings[i] = 0.0f;
        }
    }

    /****************************************/
    /****************************************/

    Real CHumanDefaultSensor::CalculateReading(Real f_distance, Real f_intensity) {
        return (f_intensity * f_intensity) / (f_distance * f_distance);
    }

    /****************************************/
    /****************************************/

    REGISTER_SENSOR(CHumanDefaultSensor,
            "human", "default",
            "Anthony Debruyn",
            "1.0",
            "A generic human sensor.",
            "This sensor accesses a set of human sensors. The sensors all return a value\n"
            "between 0 and 1, where 0 means nothing within range and 1 means the perceived\n"
            "human saturates the sensor. Values between 0 and 1 depend on the distance of\n"
            "the perceived human. Each reading R is calculated with R=(I/x)^2, where x is the\n"
            "distance between a sensor and the human, and I is the reference intensity of the\n"
            "perceived human. The reference intensity corresponds to the minimum distance at\n"
            "which the human saturates a sensor. The reference intensity depends on the\n"
            "individual human, and it is set with the \"intensity\" attribute of the human\n"
            "entity. In case multiple humans are present in the environment, each sensor\n"
            "reading is calculated as the sum of the individual readings due to each human.\n"
            "In other words, human wave interference is not taken into account. In\n"
            "controllers, you must include the ci_human_sensor.h header.\n\n"
            "REQUIRED XML CONFIGURATION\n\n"
            "  <controllers>\n"
            "    ...\n"
            "    <my_controller ...>\n"
            "      ...\n"
            "      <sensors>\n"
            "        ...\n"
            "        <human implementation=\"default\" />\n"
            "        ...\n"
            "      </sensors>\n"
            "      ...\n"
            "    </my_controller>\n"
            "    ...\n"
            "  </controllers>\n\n"
            "OPTIONAL XML CONFIGURATION\n\n"
            "It is possible to draw the rays shot by the human sensor in the OpenGL\n"
            "visualization. This can be useful for sensor debugging but also to understand\n"
            "what's wrong in your controller. In OpenGL, the rays are drawn in cyan when\n"
            "they are not obstructed and in purple when they are. In case a ray is\n"
            "obstructed, a black dot is drawn where the intersection occurred.\n"
            "To turn this functionality on, add the attribute \"show_rays\" as in this\n"
            "example:\n\n"
            "  <controllers>\n"
            "    ...\n"
            "    <my_controller ...>\n"
            "      ...\n"
            "      <sensors>\n"
            "        ...\n"
            "        <human implementation=\"default\"\n"
            "                   show_rays=\"true\" />\n"
            "        ...\n"
            "      </sensors>\n"
            "      ...\n"
            "    </my_controller>\n"
            "    ...\n"
            "  </controllers>\n\n"
            "It is possible to add uniform noise to the sensors, thus matching the\n"
            "characteristics of a real robot better. This can be done with the attribute\n"
            "\"noise_level\", whose allowed range is in [-1,1] and is added to the calculated\n"
            "reading. The final sensor reading is always normalized in the [0-1] range.\n\n"
            "  <controllers>\n"
            "    ...\n"
            "    <my_controller ...>\n"
            "      ...\n"
            "      <sensors>\n"
            "        ...\n"
            "        <human implementation=\"default\"\n"
            "                   noise_level=\"0.1\" />\n"
            "        ...\n"
            "      </sensors>\n"
            "      ...\n"
            "    </my_controller>\n"
            "    ...\n"
            "  </controllers>\n\n"
            "OPTIONAL XML CONFIGURATION\n\n"
            "None.\n",
            "Usable"
    );

} /* namespace argos */
