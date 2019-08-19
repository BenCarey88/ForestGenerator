//----------------------------------------------------------------------------------------------------------------------
/// @file Camera.cpp
/// @brief implementation file for Camera class
//----------------------------------------------------------------------------------------------------------------------

#include "Camera.h"

Camera::Camera()
{
  reset();
}

Camera::Camera(ngl::Vec3 _initialFrom, ngl::Vec3 _initialTo) :
  m_initialFrom(_initialFrom), m_initialTo(_initialTo)
{
  reset();
}

void Camera::reset()
{
  m_from = m_initialFrom;
  m_to = m_initialTo;
  m_up = m_initialUp;
  m_axis = m_initialAxis;
}
