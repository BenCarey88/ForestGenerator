//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_MouseEvents.cpp
/// @brief implementation file for NGLScene class events
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/Vec4.h>
#include "NGLScene.h"


ngl::Vec3 NGLScene::getProjectedPointOnTerrain(float _screenX, float _screenY)
{
  ///@ref based on concepts presented in http://antongerdelan.net/opengl/raycasting.html

  //find MVP
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  ngl::Mat4 currentTransform = (*m_currentMouseTransform);
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MVPinverse = MVP.inverse();

  //get x and y coordinates in projected device space
  float projDevX = 2*_screenX/m_win.width-1;
  float projDevY = 1-2*_screenY/m_win.height;

  //reverse the process of the MVP matrix to find a vector pointing into the screen from the mouse
  ngl::Vec4 rayClip = ngl::Vec4(projDevX,projDevY, -1.0, 1.0);
  ngl::Vec4 rayEye = m_project.inverse() * rayClip;
  rayEye = {rayEye.m_x, rayEye.m_y, -1, 0};
  ngl::Vec4 rayWorldDirection = currentTransform.inverse() * m_view.inverse() * rayEye;
  ngl::Vec3 rayDir = ngl::Vec3(rayWorldDirection.m_x, rayWorldDirection.m_y, rayWorldDirection.m_z);
  rayDir.normalize();

  //find a position vector rayStart in world space somwhere along the ray from the mouse into the screen
  //note that rayStart was found in a nonstandard way here somewhat by accident and I believe it's specific to the FOV etc.
  ngl::Vec4 rayStart4 = MVPinverse * ngl::Vec4(projDevX,projDevY,0.9f,1);
  ngl::Vec3 rayStart = ngl::Vec3(rayStart4.m_x, rayStart4.m_y, rayStart4.m_z);

  //find point to start searching from that's relatively close to the terrain to reduce search time
  float maxTerrainHeight = m_terrainGen.m_amplitude*2;
  float stepSize = m_rayPickTolerance;
  float n = (maxTerrainHeight-rayStart.m_y)/rayDir.m_y;
  ngl::Vec3 rayEnd = rayStart + n*rayDir;

  //set perlinModule equal to terrain values
  m_perlinModule.SetOctaveCount(m_terrainGen.m_octaves);
  m_perlinModule.SetFrequency(m_terrainGen.m_frequency);
  m_perlinModule.SetPersistence(m_terrainGen.m_persistence);
  m_perlinModule.SetLacunarity(m_terrainGen.m_lacunarity);

  bool rayEndHasBeenFound = false;
  //now move in small steps from that point til we are within an error tolerance of the terrain
  while(rayEnd.m_y >= -maxTerrainHeight-1 && rayEnd.m_y <= maxTerrainHeight+1)
  {
    n += stepSize;
    rayEnd = rayStart + n*rayDir;
    float yPos = m_terrainGen.m_amplitude * float(m_perlinModule.GetValue(double(rayEnd.m_x),
                                                                          double(rayEnd.m_z),
                                                                          m_terrainGen.m_seed));
    if(abs(rayEnd.m_y - yPos) < stepSize)
    {
      rayEndHasBeenFound = true;
      break;
    }
  }

  //if we were unsuccessful in the first attempt, try going in the opposite direction
  if(!rayEndHasBeenFound)
  {
    float n = (maxTerrainHeight-rayStart.m_y)/rayDir.m_y;
    ngl::Vec3 rayEnd = rayStart + n*rayDir;

    while(rayEnd.m_y >= -maxTerrainHeight-1 && rayEnd.m_y <= maxTerrainHeight+1)
    {
      n -= stepSize;
      rayEnd = rayStart + n*rayDir;
      float yPos = m_terrainGen.m_amplitude * float(m_perlinModule.GetValue(double(rayEnd.m_x),
                                                                            double(rayEnd.m_z),
                                                                            m_terrainGen.m_seed));
      if(abs(rayEnd.m_y - yPos) < stepSize)
      {
        rayEndHasBeenFound = true;
        break;
      }
    }
  }

  return rayEnd;
}

//------------------------------------------------------------------------------------------------------------------------


void NGLScene::addPointToPaintedForest(ngl::Vec3 &_point)
{
  float yPos = m_terrainGen.m_amplitude * float(m_perlinModule.GetValue(double(_point.m_x),
                                                                        double(_point.m_z),
                                                                        m_terrainGen.m_seed));

  //point is only viable if it lies on the terrain
  bool pointIsViable = (abs(_point.m_x)<m_width/2-1 &&
                        abs(_point.m_z)<m_width/2-1 &&
                        abs(_point.m_y-yPos)<m_rayPickTolerance);
  //and point is not viable if it is too close to some other point
  if(pointIsViable)
  {
    for(auto &p : m_paintPoints)
    {
      if((_point-p).length()<m_minTreeDist)
      {
        pointIsViable=false;
        break;
      }
    }
  }

  if(pointIsViable)
  {
    m_paintPoints.push_back(_point);
    m_paintPointIndices.push_back(GLshort(m_paintPoints.size()-1));
    m_paintedForest.addTreeToForest(_point, m_paintBrushNum);
  }
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
    ngl::Vec3 point = getProjectedPointOnTerrain(_event->x(), _event->y());
    m_paintLineVertices.push_back(point);
    m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));
    m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));
    m_buildPaintLineVAO = true;

    addPointToPaintedForest(point);

    update();
  }

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if (_event->button() == Qt::LeftButton )
  {
    if(m_treePaintMode)
    {
      ngl::Vec3 point = getProjectedPointOnTerrain(_event->x(), _event->y());

      m_paintLineVertices.push_back(point);
      m_paintLineIndices.push_back(GLshort(m_paintLineVertices.size()-1));

      addPointToPaintedForest(point);

      m_buildPaintLineVAO = true;
      m_drawingLine = true;
    }
    else
    {
      m_win.origX  = _event->x();
      m_win.origY  = _event->y();
      m_win.rotate = true;
    }
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

