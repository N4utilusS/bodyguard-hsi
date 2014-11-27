/*
 * human_sensor_equipped_entity.h
 *
 *  Created on: 24 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef HUMAN_SENSOR_EQUIPPED_ENTITY_H_
#define HUMAN_SENSOR_EQUIPPED_ENTITY_H_

namespace argos {
    class CHumanSensorEquippedEntity;
}

#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/entity.h>
#include <map>

namespace argos {

    class CHumanSensorEquippedEntity: public CEntity {

        public:

            ENABLE_VTABLE();

            struct SSensor {
                    CVector3 Position;
                    CVector3 Direction;
                    SSensor(const CVector3& c_position,
                            const CVector3& c_direction,
                            Real f_range) :
                                Position(c_position),
                                Direction(c_direction) {
                        Direction.Normalize();
                        Direction *= f_range;
                    }
            };

            typedef std::vector<SSensor*> TSensors;

        public:
            CHumanSensorEquippedEntity(CComposableEntity* pc_parent);

            CHumanSensorEquippedEntity(CComposableEntity* pc_parent,
                    const std::string& str_id);

            virtual ~CHumanSensorEquippedEntity();

            virtual void Init(TConfigurationNode& t_tree);

            virtual std::string GetTypeDescription() const {
                return "human_sensors";
            }

            inline size_t GetNumSensors() const {
                return m_tSensors.size();
            }

            inline SSensor& GetSensor(size_t un_idx) {
                return *m_tSensors[un_idx];
            }

            inline TSensors& GetSensors() {
                return m_tSensors;
            }

            void AddSensor(const CVector3& c_position,
                    const CVector3& c_direction,
                    Real f_range);

            void AddSensorRing(const CVector3& c_center,
                    Real f_radius,
                    const CRadians& c_start_angle,
                    Real f_range,
                    UInt32 un_num_sensors);

        protected:

            /** The list of sensors */
            TSensors m_tSensors;
    };

} /* namespace argos */

#endif /* HUMAN_SENSOR_EQUIPPED_ENTITY_H_ */
