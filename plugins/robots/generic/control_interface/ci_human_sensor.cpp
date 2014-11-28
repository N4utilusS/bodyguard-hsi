/*
 * ci_human_sensor.cpp
 *
 *  Created on: 25 nov. 2014
 *      Author: anthonydebruyn
 */

#include "ci_human_sensor.h"

#ifdef ARGOS_WITH_LUA
#include <argos3/core/wrappers/lua/lua_utility.h>
#endif


namespace argos {

    /****************************************/
    /****************************************/

#ifdef ARGOS_WITH_LUA
   void CCI_HumanSensor::CreateLuaState(lua_State* pt_lua_state) {
      CLuaUtility::StartTable(pt_lua_state, "human");
      for(size_t i = 0; i < m_tReadings.size(); ++i) {
         CLuaUtility::AddToTable(pt_lua_state, i+1, m_tReadings[i]);
      }
      CLuaUtility::EndTable(pt_lua_state);
   }
#endif

   /****************************************/
   /****************************************/

#ifdef ARGOS_WITH_LUA
   void CCI_HumanSensor::ReadingsToLuaState(lua_State* pt_lua_state) {
      lua_getfield(pt_lua_state, -1, "human");
      for(size_t i = 0; i < m_tReadings.size(); ++i) {
         lua_pushnumber(pt_lua_state, i+1           );
         lua_pushnumber(pt_lua_state, m_tReadings[i]);
         lua_settable  (pt_lua_state, -3            );
      }
      lua_pop(pt_lua_state, 1);
   }
#endif


   /****************************************/
   /****************************************/

} /* namespace argos */
