/*
 * human_default_sensor.h
 *
 *  Created on: 25 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef HUMAN_DEFAULT_SENSOR_H
#define HUMAN_DEFAULT_SENSOR_H

#include <string>
#include <map>

namespace argos {
   class CHumanDefaultSensor;
   class CHumanSensorEquippedEntity;
}

#include <core/simulator/sensor.h>
#include <plugins/robots/generic/control_interface/ci_human_sensor.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/simulator/space/space.h>

namespace argos {

    class CHumanDefaultSensor: public CSimulatedSensor, public CCI_HumanSensor {

        public:
            CHumanDefaultSensor();

            virtual ~CHumanDefaultSensor() {}

            virtual void SetRobot(CComposableEntity& c_entity);

            virtual void Init(TConfigurationNode& t_tree);

            virtual void Update();

            virtual void Reset();

            /**
             * Calculates the human reading resulting from a human source at the given distance.
             * Denoting the intensity with <em>i</em> and the distance <em>x</em>, this function calculates <em>i</em> = (<em>I</em> / <em>x<em>)^2.
             * <em>I</em> is the reference intensity of the human emitter, that is, the distance at which the human reading saturates.
             * It is dependent on the human entity being considered.
             * @param f_distance The distance of the considered human source.
             * @param f_intensity The reference intensity of the considered human source.
             * @returns A value in the range [0:1], where 0 means that the human is too far to be sensed, and 1 means the human is so close that it saturates the sensor.
             */
            virtual Real CalculateReading(Real f_distance, Real f_intensity);

        protected:

            /** Reference to embodied entity associated to this sensor */
            CEmbodiedEntity* m_pcEmbodiedEntity;

            /** Reference to light sensor equipped entity associated to this sensor */
            CHumanSensorEquippedEntity* m_pcHumanEntity;

            /** Reference to controllable entity associated to this sensor */
            CControllableEntity* m_pcControllableEntity;

            /** Flag to show rays in the simulator */
            bool m_bShowRays;

            /** Random number generator */
            CRandom::CRNG* m_pcRNG;

            /** Whether to add noise or not */
            bool m_bAddNoise;

            /** Noise range */
            CRange<Real> m_cNoiseRange;

            /** Reference to the space */
            CSpace& m_cSpace;
    };

} /* namespace argos */

#endif /* HUMAN_DEFAULT_SENSOR_H */
