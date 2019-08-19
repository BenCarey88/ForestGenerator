//----------------------------------------------------------------------------------------------------------------------
/// @file Instance.h
/// @author Ben Carey
/// @version 1.0
/// @date 21/07/19
//----------------------------------------------------------------------------------------------------------------------

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <ngl/Mat4.h>


//----------------------------------------------------------------------------------------------------------------------
/// @class Instance
/// @brief this class stores the data that constitutes an instance, to fill the instance cache in the LSystems
//----------------------------------------------------------------------------------------------------------------------

class Instance
{
public:

  //CONSTRUCTORS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for Instance class
  //--------------------------------------------------------------------------------------------------------------------
  Instance() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief ctor allowing us to set the transform variable
  //--------------------------------------------------------------------------------------------------------------------
  Instance(ngl::Mat4 _transform);

  //EXIT POINT STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct ExitPoint
  /// @brief this struct stores the data for the exit points of each instance, where we will call for a new instance
  //--------------------------------------------------------------------------------------------------------------------
  struct ExitPoint
  {
    /// @brief struct constructor, which sets all member variables
    ExitPoint(size_t _exitId, size_t _exitAge, ngl::Mat4 _transform);
    /// @brief the id of the instance we'll need to call at this point
    size_t m_exitId;
    /// @brief the age of the instance we'll need to call at this point
    size_t m_exitAge;
    /// @brief the transform of the exit point relative to the base of the current instance
    ngl::Mat4 m_exitTransform;
  };


  //MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief m_transform, the transform of the instance relative to the base of the tree
  //--------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_transform;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief indexes to the beginning and end of the instance in the index buffer m_heroIndices
  /// from the corrseponding LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_instanceStart;
  size_t m_instanceEnd;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief indexes to the beginning and end of the instance in the index buffer m_heroLeafIndices
  /// from the corrseponding LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_instanceLeafStart;
  size_t m_instanceLeafEnd;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief indexes to the beginning and end of the instance in the index buffer m_heroPolygonIndices
  /// from the corrseponding LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_instancePolygonStart;
  size_t m_instancePolygonEnd;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief list of exit points of the instance where we cill for a new instance in the forest creation algorithm
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ExitPoint> m_exitPoints;
};


#endif //INSTANCE_H_
