/*
 * human_entity.h
 *
 *  Created on: 5 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef HUMAN_ENTITY_H_
#define HUMAN_ENTITY_H_

namespace argos {
    class CControllableEntity;
    class CEmbodiedEntity;
    class CHumanEntity;
}

#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/plugins/simulator/entities/wheeled_entity.h>
#include <argos3/plugins/simulator/media/led_medium.h>

namespace argos {

    class CHumanEntity: public CComposableEntity {

        public:

            ENABLE_VTABLE();

        public:

            CHumanEntity();
            virtual ~CHumanEntity();

            virtual void Init(TConfigurationNode& t_tree);
            virtual void Reset();
            virtual void Destroy();

            virtual void UpdateComponents();

            inline CControllableEntity& GetControllableEntity() {
                return *m_pcControllableEntity;
            }

            inline CEmbodiedEntity& GetEmbodiedEntity() {
                return *m_pcEmbodiedEntity;
            }

            inline CWheeledEntity& GetWheeledEntity() {
                return *m_pcWheeledEntity;
            }

            inline Real GetRadius() const {
                return m_fRadius;
            }

            inline void SetRadius(Real c_radius) {
                m_fRadius = c_radius;
            }

            inline Real GetHeight() const {
                return m_fHeight;
            }

            inline void SetHeight(Real c_height) {
                m_fHeight = c_height;
            }

            inline Real GetMass() const {
                return m_fMass;
            }

            inline void SetMass(Real f_mass) {
                m_fMass = f_mass;
            }

            virtual std::string GetTypeDescription() const {
                return "human";
            }

        private:

            CControllableEntity*    m_pcControllableEntity;
            CEmbodiedEntity*        m_pcEmbodiedEntity;
            CWheeledEntity*         m_pcWheeledEntity;
            Real                    m_fRadius;
            Real                    m_fHeight;
            Real                    m_fMass;
    };

} /* namespace argos */

#endif /* HUMAN_ENTITY_H_ */
