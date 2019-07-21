//----------------------------------------------------------------------------------------------------------------------
/// @file Instance.cpp
/// @brief implementation file for Instance struct
//----------------------------------------------------------------------------------------------------------------------

#include "Instance.h"

Instance::Instance(ngl::Mat4 _transform) :
  m_transform(_transform) {}

Instance::ExitPoint::ExitPoint(size_t _exitId, size_t _exitAge, ngl::Mat4 _exitTransform) :
  m_exitId(_exitId), m_exitAge(_exitAge), m_exitTransform(_exitTransform) {}
