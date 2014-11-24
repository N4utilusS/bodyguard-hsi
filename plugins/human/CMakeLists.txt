#
# Human headers
#
# argos3/plugins/human/simulator
if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_HEADERS_PLUGINS_HUMAN_SIMULATOR
    simulator/dynamics2d_human_model.h
    simulator/human_entity.h)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Human sources
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  set(ARGOS3_SOURCES_PLUGINS_HUMAN
    ${ARGOS3_SOURCES_PLUGINS_HUMAN}
    ${ARGOS3_HEADERS_PLUGINS_HUMAN_SIMULATOR}
    simulator/dynamics2d_human_model.cpp
    simulator/human_entity.cpp)
  # Compile the graphical visualization only if the necessary libraries have been found
  include(ARGoSCheckQTOpenGL)
  if(ARGOS_COMPILE_QTOPENGL)
    set(ARGOS3_HEADERS_PLUGINS_HUMAN_SIMULATOR
      ${ARGOS3_HEADERS_PLUGINS_HUMAN_SIMULATOR}
      simulator/qtopengl_human.h)
    set(ARGOS3_SOURCES_PLUGINS_HUMAN
      ${ARGOS3_SOURCES_PLUGINS_HUMAN}
      simulator/qtopengl_human.h
      simulator/qtopengl_human.cpp)
  endif(ARGOS_COMPILE_QTOPENGL)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Create e-puck plugin
#
add_library(argos3plugin_${ARGOS_BUILD_FOR}_human SHARED ${ARGOS3_SOURCES_PLUGINS_HUMAN})
target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_human
  argos3plugin_${ARGOS_BUILD_FOR}_genericrobot
  argos3plugin_${ARGOS_BUILD_FOR}_actuators
  argos3plugin_${ARGOS_BUILD_FOR}_sensors
  argos3plugin_${ARGOS_BUILD_FOR}_dynamics2d)
if(ARGOS_BUILD_FOR_SIMULATOR AND ARGOS_COMPILE_QTOPENGL)
  target_link_libraries(argos3plugin_${ARGOS_BUILD_FOR}_human argos3plugin_${ARGOS_BUILD_FOR}_qtopengl)
endif(ARGOS_BUILD_FOR_SIMULATOR AND ARGOS_COMPILE_QTOPENGL)

install(FILES ${ARGOS3_HEADERS_PLUGINS_HUMAN_CONTROLINTERFACE} DESTINATION include/argos3/plugins/human/control_interface)

if(ARGOS_BUILD_FOR_SIMULATOR)
  install(FILES ${ARGOS3_HEADERS_PLUGINS_HUMAN_SIMULATOR}      DESTINATION include/argos3/plugins/human/simulator)
endif(ARGOS_BUILD_FOR_SIMULATOR)

install(TARGETS argos3plugin_${ARGOS_BUILD_FOR}_human
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/argos3
  ARCHIVE DESTINATION lib/argos3)
