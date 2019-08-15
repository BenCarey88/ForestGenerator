//----------------------------------------------------------------------------------------------------------------------
/// @file Forest.h
/// @author Ben Carey
/// @version 1.0
/// @date 06/07/19
//----------------------------------------------------------------------------------------------------------------------

#ifndef FOREST_H_
#define FOREST_H_

#include <iostream>
#include <vector>
#include <random>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include "LSystem.h"
#include "TerrainGenerator.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class Forest
/// @brief this class stores details of the L-system
//----------------------------------------------------------------------------------------------------------------------


class Forest
{
public:

  //CONSTRUCTOR
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for Forest class
  //--------------------------------------------------------------------------------------------------------------------
  Forest() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief user ctor for Forest class
  //--------------------------------------------------------------------------------------------------------------------
  Forest(const std::vector<LSystem> &_treeTypes, float _width,
         size_t _numTrees, int _numHeroTrees, TerrainGenerator _terrainGen);

  Forest(const std::vector<LSystem> &_treeTypes, int _numHeroTrees);

  //TREE STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief simple struct for grouping together an LSystem type, position and rotation
  //--------------------------------------------------------------------------------------------------------------------
  struct Tree
  {
    //--------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for Tree struct
    //--------------------------------------------------------------------------------------------------------------------
    Tree(size_t _type, ngl::Mat4 _transform);
    //--------------------------------------------------------------------------------------------------------------------
    /// @brief tree type (ie. index of LSystem it's based on)
    //--------------------------------------------------------------------------------------------------------------------
    size_t m_type;
    //--------------------------------------------------------------------------------------------------------------------
    /// @brief transform matrix for tree, representing position and orientation
    //--------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_transform;
  };

  //OUTPUT DATA STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief
  //--------------------------------------------------------------------------------------------------------------------
  struct OutputData
  {
    //--------------------------------------------------------------------------------------------------------------------
    /// @brief
    //--------------------------------------------------------------------------------------------------------------------
    OutputData(ngl::Mat4 _transform, size_t _treeType, size_t _id, size_t _age, size_t _innerIndex);

    ngl::Mat4 m_transform;
    size_t m_treeType, m_id, m_age, m_innerIndex;
  };

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief base tree types for the forest
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<LSystem> m_treeTypes;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief width and length of forest
  //--------------------------------------------------------------------------------------------------------------------
  float m_width;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief number of trees in forest
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_numTrees;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief stores positions and types of all trees in the forest
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<Tree> m_treeData;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief seed for random scattering
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_seed = 0;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine if we should use a seed - if not, we seed by time
  //--------------------------------------------------------------------------------------------------------------------
  bool m_useSeed = false;



  ///@brief this is the number of hero trees PER tree type
  int m_numHeroTrees;

  //outputCache arranged to mimic the instanceCaches of each treeType
  //separated by: treeType / id / age / innerIndex / different-branches-using-the-same-instance
  std::vector<CACHE_STRUCTURE(std::vector<ngl::Mat4>)> m_transformCache;

  //the random number generator
  std::default_random_engine m_gen;

  TerrainGenerator m_terrainGen;

  //PUBLIC METHODS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fill m_treePositions
  //--------------------------------------------------------------------------------------------------------------------
  void scatterForest();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief create geometry of tree by taking instances from the instance cache
  //--------------------------------------------------------------------------------------------------------------------
  void createTree(size_t _treeType, ngl::Mat4 _transform, size_t _id, size_t _age);

  Instance * getInstance(LSystem &_treeType, size_t _id, size_t _age, size_t &_innerIndex);

  void createForest();

  void resizeTransformCache();

  void seedRandomEngine();

  void addTreeToForest(ngl::Vec3 &_point, size_t _treeType);

};

#endif //FOREST_H_
