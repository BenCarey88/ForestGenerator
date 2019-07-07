//----------------------------------------------------------------------------------------------------------------------
/// @file Camera.cpp
/// @brief implementation file for Camera class
//----------------------------------------------------------------------------------------------------------------------

#include "Camera.h"


void Camera::update()
{
  m_right = (m_to-m_from).cross(m_up);
  m_right.normalize();
  m_trueUp = m_right.cross(m_to-m_from);
  m_trueUp.normalize();
}


void Camera::initialise()
{
  m_from = {0,-50,50};
  m_to = {0,0,0};
  m_up = {0,0,1};
  update();
}
