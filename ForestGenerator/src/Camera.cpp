//----------------------------------------------------------------------------------------------------------------------
/// @file Camera.cpp
/// @brief implementation file for Camera struct
//----------------------------------------------------------------------------------------------------------------------

#include "Camera.h"

Camera::Camera()
{
  reset();
}

Camera::Camera(ngl::Vec3 _initialFrom, ngl::Vec3 _initialTo,
               ngl::Vec3 _initialUp, ngl::Vec3 _initialAxis) :
  m_initialFrom(_initialFrom), m_initialTo(_initialTo),
  m_initialUp(_initialUp), m_initialAxis(_initialAxis)
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
