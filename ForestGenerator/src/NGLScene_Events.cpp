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

  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  ngl::Mat4 currentTransform = (*m_currentMouseTransform);//*m_initialRotation;
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MVPinverse = MVP.inverse();

  float projDevX = (2*float(_event->x())/m_win.width)-1;
  float projDevY = 1-2*(float(_event->y())/m_win.height);

  ngl::Vec4 ray_clip = ngl::Vec4(projDevX,projDevY, -1.0, 1.0);
  ngl::Vec4 ray_eye = m_project.inverse() * ray_clip;
  ray_eye = {ray_eye.m_x, ray_eye.m_y, -1, 0};

  ngl::Vec4 ray_world = m_view.inverse() * ray_eye;

  ngl::Vec4 ray_wor = currentTransform.inverse() * ray_eye;

  ngl::Vec3 ray = ngl::Vec3(ray_wor.m_x, ray_wor.m_y, ray_wor.m_z);

  // don't forget to normalise the vector at some point
  ray.normalize();

  print(ray);
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

//  if(m_addRay)
//  {
//    m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
//    ngl::Mat4 currentTransform = (*m_currentMouseTransform);//*m_initialRotation;
//    ngl::Mat4 MVP= m_project*m_view*currentTransform;
//    ngl::Mat4 MVPinverse = MVP.inverse();




//    float projDevX = (2*float(_event->x())/m_win.width)-1;
//    float projDevY = 1-2*(float(_event->y())/m_win.height);

//    ngl::Vec4 ray_clip = ngl::Vec4(projDevX,projDevY, -1.0, 1.0);
//    ngl::Vec4 ray_eye = m_project.inverse() * ray_clip;
//    ray_eye = {ray_eye.m_x, ray_eye.m_y, -1, 0};

//    ngl::Vec4 ray_world = m_view.inverse() * ray_eye;
//    ngl::Vec3 ray_wor = ngl::Vec3(ray_world.m_x, ray_world.m_y, ray_world.m_z);
//    // don't forget to normalise the vector at some point
//    ray_wor.normalize();




//    ngl::Vec4 rayStart4 = MVPinverse * ngl::Vec4(projDevX,projDevY,0.9,1);
//    ngl::Vec4 rayEnd4 = MVPinverse * ngl::Vec4(projDevX,projDevY,1,1);

//    ngl::Vec3 rayStart = ngl::Vec3(rayStart4.m_x, rayStart4.m_y, rayStart4.m_z);
//    ngl::Vec3 rayEnd = ngl::Vec3(rayEnd4.m_x, rayEnd4.m_y, rayEnd4.m_z);
//    //ngl::Vec3 rayEnd = rayStart + 10*ray_wor;

////    m_projDevRays.push_back({projDevX, projDevY, -1});
////    m_projDevRays.push_back({projDevX, projDevY, 1});

////    print("\n");
////    print(rayStart);
////    print(rayEnd);

//    m_worldSpaceRays.push_back(rayStart);
//    m_rayIndices.push_back(GLshort(m_worldSpaceRays.size()-1));
//    m_worldSpaceRays.push_back(rayEnd);
//    m_rayIndices.push_back(GLshort(m_worldSpaceRays.size()-1));

//    m_buildRayVAO = true;
//    m_addRay = false;
//  }

  if ( _event->button() == Qt::RightButton )
  {
    m_addRay = true;
  }

//  for(size_t i=0; i<m_worldSpaceRays.size(); i+=2)
//  {
//    print("\nRAY START: ");
//    print(m_worldSpaceRays[i]);
//    print("RAY END: ");
//    print(m_worldSpaceRays[i+1]);
//  }




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

