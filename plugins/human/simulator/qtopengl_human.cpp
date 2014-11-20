/*
 * qtopengl_human.cpp
 *
 *  Created on: 13 nov. 2014
 *      Author: anthonydebruyn
 */

#include <argos3/plugins/human/simulator/qtopengl_human.h>
#include <argos3/plugins/human/simulator/human_entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_widget.h>

namespace argos {

    /****************************************/
    /****************************************/

    const GLfloat BODY_COLOR[]       = { 1.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat DIRECTION_COLOR[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat SPECULAR[]         = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat SHININESS[]        = { 0.0f                   };
    const GLfloat EMISSION[]         = { 0.0f, 0.0f, 0.0f, 1.0f };

    /****************************************/
    /****************************************/

    CQTOpenGLHuman::CQTOpenGLHuman() :
              m_unVertices(20) {

        /* Reserve the needed display lists */
        m_unBaseList = glGenLists(1);
        m_unBodyList = m_unBaseList;

        /* Make body list */
        glNewList(m_unBodyList, GL_COMPILE);
        RenderBody();
        glEndList();

    }

    /****************************************/
    /****************************************/

    CQTOpenGLHuman::~CQTOpenGLHuman() {
        glDeleteLists(m_unBaseList, 1);
    }

    /****************************************/
    /****************************************/

    void CQTOpenGLHuman::Draw(CHumanEntity& c_entity) {
        /* Draw the body */
        glPushMatrix();
        glScalef(c_entity.GetRadius(), c_entity.GetRadius(), c_entity.GetHeight());
        glCallList(m_unBodyList);
        glPopMatrix();
    }

    /****************************************/
    /****************************************/

    void CQTOpenGLHuman::RenderBody() {
        /* Since this shape can be stretched,
             make sure the normal vectors are unit-long */
        glEnable(GL_NORMALIZE);

        /* Set the material */
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BODY_COLOR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SPECULAR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, SHININESS);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, EMISSION);

        /* Let's start the actual shape */
        /* Side surface */
        CVector2 cVertex(1.0f, 0.0f);
        CRadians cAngle(CRadians::TWO_PI / m_unVertices);
        glBegin(GL_QUAD_STRIP);
        for(GLuint i = 0; i <= m_unVertices; i++) {
            glNormal3f(cVertex.GetX(), cVertex.GetY(), 0.0f);
            glVertex3f(cVertex.GetX(), cVertex.GetY(), 1.0f);
            glVertex3f(cVertex.GetX(), cVertex.GetY(), 0.0f);
            cVertex.Rotate(cAngle);
        }
        glEnd();
        /* Top disk */
        cVertex.Set(1.0f, 0.0f);
        glBegin(GL_POLYGON);
        glNormal3f(0.0f, 0.0f, 1.0f);
        for(GLuint i = 0; i <= m_unVertices; i++) {
            glVertex3f(cVertex.GetX(), cVertex.GetY(), 1.0f);
            cVertex.Rotate(cAngle);
        }
        glEnd();
        /* Bottom disk */
        cVertex.Set(1.0f, 0.0f);
        cAngle = -cAngle;
        glBegin(GL_POLYGON);
        glNormal3f(0.0f, 0.0f, -1.0f);
        for(GLuint i = 0; i <= m_unVertices; i++) {
            glVertex3f(cVertex.GetX(), cVertex.GetY(), 0.0f);
            cVertex.Rotate(cAngle);
        }
        glEnd();

        /* Triangle to set the direction */
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, DIRECTION_COLOR);
        glBegin(GL_TRIANGLES);
        glVertex3f( 1.0f,  0.0f, 1.001f);
        glVertex3f(-0.7f,  0.3f, 1.001f);
        glVertex3f(-0.7f, -0.3f, 1.001f);
        glEnd();
        /* The shape definition is finished */

        /* We don't need it anymore */
        glDisable(GL_NORMALIZE);

    }

    /****************************************/
    /****************************************/

    class CQTOpenGLOperationDrawHumanNormal : public CQTOpenGLOperationDrawNormal {
        public:
            void ApplyTo(CQTOpenGLWidget& c_visualization,
                    CHumanEntity& c_entity) {
                static CQTOpenGLHuman m_cModel;
                c_visualization.DrawPositionalEntity(c_entity.GetEmbodiedEntity());
                m_cModel.Draw(c_entity);
            }
    };

    class CQTOpenGLOperationDrawHumanSelected : public CQTOpenGLOperationDrawSelected {
        public:
            void ApplyTo(CQTOpenGLWidget& c_visualization,
                    CHumanEntity& c_entity) {
                c_visualization.DrawBoundingBox(c_entity.GetEmbodiedEntity());
            }
    };

    REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawNormal, CQTOpenGLOperationDrawHumanNormal, CHumanEntity);

    REGISTER_QTOPENGL_ENTITY_OPERATION(CQTOpenGLOperationDrawSelected, CQTOpenGLOperationDrawHumanSelected, CHumanEntity);

    /****************************************/
    /****************************************/

} /* namespace argos */
