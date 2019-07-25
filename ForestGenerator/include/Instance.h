//----------------------------------------------------------------------------------------------------------------------
/// @file Instance.h
/// @author Ben Carey
/// @version 1.0
/// @date 21/07/
//----------------------------------------------------------------------------------------------------------------------

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <ngl/Mat4.h>


//----------------------------------------------------------------------------------------------------------------------
/// @class Instance
/// @brief this struct stores the data that constitutes an instance, to fill the instance cache in the LSystem
//----------------------------------------------------------------------------------------------------------------------

struct Instance
{
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for Instance struct
  //--------------------------------------------------------------------------------------------------------------------
  Instance() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for Instance struct
  //--------------------------------------------------------------------------------------------------------------------
  Instance(ngl::Mat4 _transform);

  ngl::Mat4 m_transform;
  //GLshort * m_instanceStart;
  //GLshort * m_instanceEnd;
  size_t m_instanceStart;
  size_t m_instanceEnd;
  //std::vector<GLshort> m_indices;

  struct ExitPoint
  {
    ExitPoint(size_t _exitId, size_t _exitAge, ngl::Mat4 _transform);
    size_t m_exitId;
    size_t m_exitAge;
    ngl::Mat4 m_exitTransform;
  };

  std::vector<ExitPoint> m_exitPoints;
};


#endif //INSTANCE_H_
