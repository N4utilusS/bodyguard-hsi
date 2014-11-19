/*
 * dynamics2d_human_model.h
 *
 *  Created on: 17 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef DYNAMICS2D_HUMAN_MODEL_H
#define DYNAMICS2D_HUMAN_MODEL_H

namespace argos {
   class CDynamics2DDifferentialSteeringControl;
}

#include <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_model.h>
#include <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_differentialsteering_control.h>
#include <argos3/plugins/human/simulator/human_entity.h>

namespace argos {

    class CDynamics2DHumanModel : public CDynamics2DModel {

        public:

            CDynamics2DHumanModel(CDynamics2DEngine& c_engine, CHumanEntity& c_entity);
            virtual ~CDynamics2DHumanModel();

            virtual bool CheckIntersectionWithRay(Real& f_t_on_ray,
                    const CRay3& c_ray) const;

            virtual bool MoveTo(const CVector3& c_position,
                    const CQuaternion& c_orientation,
                    bool b_check_only = false);

            virtual void Reset();

            virtual void CalculateBoundingBox();

            virtual void UpdateEntityStatus();
            virtual void UpdateFromEntityStatus();

            virtual bool IsCollidingWithSomething() const;

        private:

            CHumanEntity& m_cHumanEntity;
            CWheeledEntity& m_cWheeledEntity;

            CDynamics2DDifferentialSteeringControl m_cDiffSteering;

            cpFloat  m_fMass;
            cpShape* m_ptBaseShape;
            cpBody*  m_ptActualBaseBody;

            const Real* m_fCurrentWheelVelocity;
    };

} /* namespace argos */

#endif /* DYNAMICS2D_HUMAN_MODEL_H */
