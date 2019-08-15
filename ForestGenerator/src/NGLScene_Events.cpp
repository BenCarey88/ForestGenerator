//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_MouseEvents.cpp
/// @brief implementation file for NGLScene class events
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/Vec4.h>
#include "NGLScene.h"


ngl::Vec3 NGLScene::getProjectedPointOnPlane(float _screenX, float _screenY)
{
  ///@ref http://antongerdelan.net/opengl/raycasting.html
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  ngl::Mat4 currentTransform = (*m_currentMouseTransform);
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MVPinverse = MVP.inverse();

  float projDevX = 2*_screenX/m_win.width-1;
  float projDevY = 1-2*_screenY/m_win.height;

  ngl::Vec4 rayClip = ngl::Vec4(projDevX,projDevY, -1.0, 1.0);
  ngl::Vec4 rayEye = m_project.inverse() * rayClip;
  rayEye = {rayEye.m_x, rayEye.m_y, -1, 0};
  ngl::Vec4 rayWorldDirection = currentTransform.inverse() * m_view.inverse() * rayEye;
  ngl::Vec3 rayDir = ngl::Vec3(rayWorldDirection.m_x, rayWorldDirection.m_y, rayWorldDirection.m_z);
  rayDir.normalize();

  ngl::Vec4 rayStart4 = MVPinverse * ngl::Vec4(projDevX,projDevY,0.9f,1);
  ngl::Vec3 rayStart = ngl::Vec3(rayStart4.m_x, rayStart4.m_y, rayStart4.m_z);

  float n = -rayStart.m_y/rayDir.m_y;
  ngl::Vec3 rayEnd = rayStart + n*rayDir;

  return rayEnd;
}

//------------------------------------------------------------------------------------------------------------------------
///MOUSE EVENTS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if ( m_win.rotate && _event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_win.origX;
    int diffy = _event->y() - m_win.origY;
    m_win.spinXFace = static_cast<int>( 0.4f * diffy );
    m_win.spinYFace = static_cast<int>( 0.4f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();

    ngl::Mat4 rotX;
    ngl::Mat4 rotY;
    rotX.rotateX(m_win.spinXFace);
    m_currentCamera->m_axis = rotX*m_currentCamera->m_axis;
    rotY.euler(m_win.spinYFace,
               m_currentCamera->m_axis.m_x,
               m_currentCamera->m_axis.m_y,
               m_currentCamera->m_axis.m_z);
    *m_currentMouseTransform = rotY*rotX*(*m_currentMouseTransform);
    update();
  }

  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::MidButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_win.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();
    ngl::Real multiplier = (m_currentCamera->m_to - m_currentCamera->m_from).length()/100;

    ngl::Mat4 trans;
    trans.translate(multiplier*INCREMENT*diffX,-multiplier*INCREMENT*diffY,0);
    *m_currentMouseTransform = trans*(*m_currentMouseTransform);

    update();

  }

  if(m_drawingLine)
  {
    ngl::Vec3 point = getProjectedPointOnPlane(_event->x(), _event->y());
    m_paintLineVertices.push_back(point);
    m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));
    m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));
    m_buildPaintLineVAO = true;
    update();

    bool pointIsViable=true;
    for(auto p : m_points)
    {
      if((point-p).length()<m_minTreeDist)
      {
        pointIsViable=false;
        break;
      }
    }

    if(pointIsViable)
    {
      m_points.push_back(point);
      m_pointIndices.push_back(GLshort(m_points.size()-1));
      m_paintedForest.addTreeToForest(point, 0);
    }
  }

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  if(m_treePaintMode)
  {
    ngl::Vec3 point = getProjectedPointOnPlane(_event->x(), _event->y());

    m_paintLineVertices.push_back(point);
    m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));

    m_points.push_back(point);
    m_pointIndices.push_back(GLshort(m_points.size()-1));
    m_paintedForest.addTreeToForest(point, 0);

    m_buildPaintLineVAO = true;
    m_drawingLine = true;
  }

  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  else if (m_rotate==true && _event->button() == Qt::LeftButton )
  {
    m_win.origX  = _event->x();
    m_win.origY  = _event->y();
    m_win.rotate = true;
  }
  // middle mouse translate mode
  else if ( _event->button() == Qt::MidButton )
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }

  update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  if(m_drawingLine)
  {
    //m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));
    m_paintLineVertices.clear();
    m_paintLineIndices.clear();
    m_buildPaintLineVAO = true;
    m_drawingLine = false;
    update();
  }

  // this event is called when the mouse button is released
  // we then set rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
    m_win.spinXFace = 0;
    m_win.spinYFace = 0;
  }
  // middle mouse translate mode
  if ( _event->button() == Qt::MidButton )
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{

  // check the diff of the wheel position (0 means no change)
  if ( _event->delta() > 0 )
  {
    m_currentCamera->m_from += ZOOM*(m_currentCamera->m_to-m_currentCamera->m_from);
  }
  else if ( _event->delta() < 0 )
  {
    m_currentCamera->m_from -= ZOOM*(m_currentCamera->m_to-m_currentCamera->m_from);
  }
  update();
}

