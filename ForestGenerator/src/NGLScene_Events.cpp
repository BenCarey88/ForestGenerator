//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_MouseEvents.cpp
/// @brief implementation file for NGLScene class events
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/Vec4.h>
#include "NGLScene.h"

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
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if (m_rotate==true && _event->button() == Qt::LeftButton )
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

  /*std::cout<<"("<<_event->x()<<", "<<_event->y()<<")\n";

  float gridSize = 25*80;
  float screenSize;
  float disparity;
  float xVal;
  float zVal;

  if(m_win.height>m_win.width)
  {
    screenSize = m_win.width;
    disparity = m_win.height - m_win.width;
    xVal = _event->x()*gridSize/screenSize - gridSize/2;
    zVal = gridSize/2 - (_event->y()-disparity/2)*gridSize/screenSize;
  }
  else
  {
    screenSize = m_win.height;
    disparity = m_win.width - m_win.height;
    xVal = (_event->x()-disparity/2)*gridSize/screenSize - gridSize/2;
    zVal = gridSize/2 - _event->y()*gridSize/screenSize;
  }

  std::cout<<"("<<xVal<<", "<<zVal<<")\n\n";

  m_points.push_back({xVal,0,zVal});*/

  update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
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

