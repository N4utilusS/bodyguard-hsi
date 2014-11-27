/*
 * ci_human_sensor.h
 *
 *  Created on: 25 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef CCI_HUMAN_SENSOR_H
#define CCI_HUMAN_SENSOR_H

namespace argos {
    class CCI_HumanSensor;
}

#include <argos3/core/control_interface/ci_sensor.h>

namespace argos {

    class CCI_HumanSensor: public CCI_Sensor {

        public:

            virtual ~CCI_HumanSensor() {}

            inline const std::vector<Real>& GetReadings() const {
                return m_tReadings;
            }

#ifdef ARGOS_WITH_LUA
            virtual void CreateLuaState(lua_State* pt_lua_state);

            virtual void ReadingsToLuaState(lua_State* pt_lua_state);
#endif

        protected:

            std::vector<Real> m_tReadings;

    };

} /* namespace argos */

#endif /* CCI_HUMAN_SENSOR_H */
