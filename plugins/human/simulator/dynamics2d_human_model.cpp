/*
 * dynamics2d_human_model.cpp
 *
 *  Created on: 17 nov. 2014
 *      Author: anthonydebruyn
 */

#include <argos3/plugins/human/simulator/dynamics2d_human_model.h>
#include <argos3/plugins/simulator/physics_engines/dynamics2d/dynamics2d_engine.h>

namespace argos {

    /****************************************/
    /****************************************/

    static const Real HUMAN_INTERWHEEL_DISTANCE      = 0.053f; // TODO Already defined in human entity!

    static const Real HUMAN_MAX_FORCE                = 300.0f; // TODO Need to verify that. Not important right now.
    static const Real HUMAN_MAX_TORQUE               = 300.0f;
    static const Real HUMAN_MASS = 80.0f;

    enum HUMAN_WHEELS {
        HUMAN_LEFT_WHEEL = 0,
        HUMAN_RIGHT_WHEEL = 1
    };

    /****************************************/
    /****************************************/

    CDynamics2DHumanModel::CDynamics2DHumanModel(CDynamics2DEngine& c_engine, CHumanEntity& c_entity) :
              CDynamics2DModel(c_engine, c_entity.GetEmbodiedEntity()),
              m_cHumanEntity(c_entity),
              m_cWheeledEntity(m_cHumanEntity.GetWheeledEntity()),
              m_cDiffSteering(c_engine,
                      HUMAN_MAX_FORCE,
                      HUMAN_MAX_TORQUE,
                      HUMAN_INTERWHEEL_DISTANCE),
                      m_fMass(HUMAN_MASS),
                      m_fCurrentWheelVelocity(m_cWheeledEntity.GetWheelVelocities()) {

        /* Create the actual body with initial position and orientation */
        m_ptActualBaseBody =
                cpSpaceAddBody(m_cDyn2DEngine.GetPhysicsSpace(),
                        cpBodyNew(m_fMass,
                                cpMomentForCircle(m_fMass,
                                        0.0f,
                                        2 * m_cHumanEntity.GetRadius(),
                                        cpvzero)));
        const CVector3& cPosition = GetEmbodiedEntity().GetPosition();
        m_ptActualBaseBody->p = cpv(cPosition.GetX(), cPosition.GetY());
        CRadians cXAngle, cYAngle, cZAngle;
        GetEmbodiedEntity().GetOrientation().ToEulerAngles(cZAngle, cYAngle, cXAngle);
        cpBodySetAngle(m_ptActualBaseBody, cZAngle.GetValue());

        /* Associate this model to the body data for ray queries */
        m_ptActualBaseBody->data = this;

        /* Create the actual body shape */
        m_ptBaseShape =
                cpSpaceAddShape(m_cDyn2DEngine.GetPhysicsSpace(),
                        cpCircleShapeNew(m_ptActualBaseBody,
                                c_entity.GetRadius(),
                                cpvzero));
        m_ptBaseShape->e = 0.0; // No elasticity
        m_ptBaseShape->u = 0.7; // Lots of friction

        /* Constrain the actual base body to follow the diff steering control */
        m_cDiffSteering.AttachTo(m_ptActualBaseBody);

        /* Calculate bounding box */
        GetBoundingBox().MinCorner.SetZ(GetEmbodiedEntity().GetPosition().GetZ());
        GetBoundingBox().MaxCorner.SetZ(GetEmbodiedEntity().GetPosition().GetZ() + c_entity.GetHeight());
        CalculateBoundingBox();
    }

    /****************************************/
    /****************************************/

    CDynamics2DHumanModel::~CDynamics2DHumanModel() {
        m_cDiffSteering.Detach();
        cpSpaceRemoveBody(m_cDyn2DEngine.GetPhysicsSpace(), m_ptActualBaseBody);
        cpSpaceRemoveShape(m_cDyn2DEngine.GetPhysicsSpace(), m_ptBaseShape);
        cpShapeFree(m_ptBaseShape);
        cpBodyFree(m_ptActualBaseBody);
    }

    /****************************************/
    /****************************************/

    bool CDynamics2DHumanModel::CheckIntersectionWithRay(Real& f_t_on_ray,
            const CRay3& c_ray) const {
        cpSegmentQueryInfo tInfo;
        if(cpShapeSegmentQuery(m_ptBaseShape,
                cpv(c_ray.GetStart().GetX(), c_ray.GetStart().GetY()),
                cpv(c_ray.GetEnd().GetX()  , c_ray.GetEnd().GetY()  ),
                &tInfo)) {
            CVector3 cIntersectionPoint;
            c_ray.GetPoint(cIntersectionPoint, tInfo.t);
            if((cIntersectionPoint.GetZ() >= GetEmbodiedEntity().GetPosition().GetZ()                 ) &&
                    (cIntersectionPoint.GetZ() <= GetEmbodiedEntity().GetPosition().GetZ() + m_cHumanEntity.GetHeight()) ) {
                f_t_on_ray = tInfo.t;
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    /****************************************/
    /****************************************/

    bool CDynamics2DHumanModel::MoveTo(const CVector3& c_position,
            const CQuaternion& c_orientation,
            bool b_check_only) {

        /* Save body position and orientation */
        cpVect tOldPos = m_ptActualBaseBody->p;
        cpFloat fOldA = m_ptActualBaseBody->a;

        /* Move the body to the desired position */
        m_ptActualBaseBody->p = cpv(c_position.GetX(), c_position.GetY());
        CRadians cXAngle, cYAngle, cZAngle;
        c_orientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);
        cpBodySetAngle(m_ptActualBaseBody, cZAngle.GetValue());

        /* Create a shape sensor to test the movement */
        cpShape* ptTestShape = cpCircleShapeNew(m_ptActualBaseBody,
                m_cHumanEntity.GetRadius(),
                cpvzero);

        /* Check if there is a collision */
        SInt32 nCollision = cpSpaceShapeQuery(m_cDyn2DEngine.GetPhysicsSpace(), ptTestShape, NULL, NULL);

        /* Dispose of the sensor shape */
        cpShapeFree(ptTestShape);

        /* Should we keep this movement? */
        if(b_check_only || nCollision) {
            /*
             * No, because it was only a check or there was a collision
             * Restore old body state
             */
            m_ptActualBaseBody->p = tOldPos;
            cpBodySetAngle(m_ptActualBaseBody, fOldA);
        }
        else {
            /*
             * It wasn't a check and there were no collisions
             */
            /* Update the active space hash */
            cpSpaceReindexShape(m_cDyn2DEngine.GetPhysicsSpace(), m_ptBaseShape);
            /* Update bounding box */
            CalculateBoundingBox();
        }
        /* The movement is allowed if there is no collision */
        return !nCollision;
    }


    /****************************************/
    /****************************************/

    void CDynamics2DHumanModel::Reset() {
        /* Reset body position */
        const CVector3& cPosition = GetEmbodiedEntity().GetInitPosition();
        m_ptActualBaseBody->p = cpv(cPosition.GetX(), cPosition.GetY());
        /* Reset body orientation */
        CRadians cXAngle, cYAngle, cZAngle;
        GetEmbodiedEntity().GetInitOrientation().ToEulerAngles(cZAngle, cYAngle, cXAngle);
        cpBodySetAngle(m_ptActualBaseBody, cZAngle.GetValue());
        /* Zero speed and applied forces of actual base body */
        m_ptActualBaseBody->v = cpvzero;
        m_ptActualBaseBody->w = 0.0f;
        cpBodyResetForces(m_ptActualBaseBody);
        /* Zero speed and applied forces of base control body */
        m_cDiffSteering.Reset();
        /* Update bounding box */
        cpShapeCacheBB(m_ptBaseShape);
        CalculateBoundingBox();
    }

    /****************************************/
    /****************************************/

    void CDynamics2DHumanModel::CalculateBoundingBox() {
        GetBoundingBox().MinCorner.SetX(m_ptBaseShape->bb.l);
        GetBoundingBox().MinCorner.SetY(m_ptBaseShape->bb.b);
        GetBoundingBox().MaxCorner.SetX(m_ptBaseShape->bb.r);
        GetBoundingBox().MaxCorner.SetY(m_ptBaseShape->bb.t);
    }


    /****************************************/
    /****************************************/

    void CDynamics2DHumanModel::UpdateEntityStatus() {

        /* Update bounding box */
        CalculateBoundingBox();

        /* Update human body position */
        m_cDyn2DEngine.PositionPhysicsToSpace(m_cSpacePosition, GetEmbodiedEntity().GetPosition(), m_ptActualBaseBody);
        GetEmbodiedEntity().SetPosition(m_cSpacePosition);

        /* Update human body orientation */
        m_cDyn2DEngine.OrientationPhysicsToSpace(m_cSpaceOrientation, m_ptActualBaseBody);
        GetEmbodiedEntity().SetOrientation(m_cSpaceOrientation);

        /* Update human components */
        m_cHumanEntity.UpdateComponents();

        /* Check whether a transfer is necessary */
        if(m_cDyn2DEngine.IsEntityTransferActive()) {
            std::string strEngineId;
            if(m_cDyn2DEngine.CalculateTransfer(GetEmbodiedEntity().GetPosition().GetX(),
                    GetEmbodiedEntity().GetPosition().GetY(),
                    strEngineId)) {
                m_cDyn2DEngine.ScheduleEntityForTransfer(m_cHumanEntity, strEngineId);
            }
        }
    }

    /****************************************/
    /****************************************/

    void CDynamics2DHumanModel::UpdateFromEntityStatus() {

        /* Do we want to move? */
        if((m_fCurrentWheelVelocity[HUMAN_LEFT_WHEEL] != 0.0f) ||
                (m_fCurrentWheelVelocity[HUMAN_RIGHT_WHEEL] != 0.0f)) {
            m_cDiffSteering.SetWheelVelocity(m_fCurrentWheelVelocity[HUMAN_LEFT_WHEEL],
                    m_fCurrentWheelVelocity[HUMAN_RIGHT_WHEEL]);
        }
        else {
            /* No, we don't want to move - zero all speeds */
            m_cDiffSteering.Reset();
        }
    }

    /****************************************/
    /****************************************/

    bool CDynamics2DHumanModel::IsCollidingWithSomething() const {
        return cpSpaceShapeQuery(m_cDyn2DEngine.GetPhysicsSpace(), m_ptBaseShape, NULL, NULL) > 0;
    }

    /****************************************/
    /****************************************/

    REGISTER_STANDARD_DYNAMICS2D_OPERATIONS_ON_ENTITY(CHumanEntity, CDynamics2DHumanModel);

    /****************************************/
    /****************************************/

} /* namespace argos */
