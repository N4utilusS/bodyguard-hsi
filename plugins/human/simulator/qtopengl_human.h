/*
 * qtopengl_human.h
 *
 *  Created on: 13 nov. 2014
 *      Author: anthonydebruyn
 */

#ifndef QTOPENGL_HUMAN_H
#define QTOPENGL_HUMAN_H

namespace argos {
    class CQTOpenGLHuman;
    class CHumanEntity;
}


#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

namespace argos {

    class CQTOpenGLHuman {

        public:

            CQTOpenGLHuman();

            virtual ~CQTOpenGLHuman();

            void DrawLEDs(CHumanEntity& c_entity);
            virtual void Draw(CHumanEntity& c_entity);

        private:

            void RenderBody();
            void MakeLED();

        private:

            GLuint m_unBaseList;
            GLuint m_unBodyList;
            GLuint m_unVertices;
    };

} /* namespace argos */

#endif /* QTOPENGL_HUMAN_H */
