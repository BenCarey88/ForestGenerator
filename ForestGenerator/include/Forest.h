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
/// @brief this class stores a forest of trees made from LSystems, using the instancing algorithm by Kenwood et al (2014)
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
  /// @brief user ctor for Forest class: assigns variables, fills the instance caches of each LSystem, and performs
  /// tree scattering and instancing algorithm
  //--------------------------------------------------------------------------------------------------------------------
  Forest(const std::vector<LSystem> &_treeTypes, float _width,
         std::vector<size_t> &_numTrees, int _numHeroTrees, TerrainGenerator _terrainGen,
         size_t _seed, bool _useSeed);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief user ctor for painted forests, performs less actions on construction and assigns less variables
  /// because tree scattering is no longer needed when trees are painted onto terrain individually
  //--------------------------------------------------------------------------------------------------------------------
  Forest(const std::vector<LSystem> &_treeTypes, int _numHeroTrees, size_t _seed, bool _useSeed);

  //TREE STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct Tree
  /// @brief simple struct for grouping together an LSystem type and a transformation
  //--------------------------------------------------------------------------------------------------------------------
  struct Tree
  {
    /// @brief ctor for Tree struct, assigns both member variables
    Tree(size_t _type, ngl::Mat4 _transform);
    /// @brief tree type (ie. index in m_treeTypes of LSystem the tree uses)
    size_t m_type;
    /// @brief transform matrix for tree, representing position and orientation
    ngl::Mat4 m_transform;
  };

  //CACHE INDEX STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct CacheIndex
  /// @brief simple struct for grouping together the indexes of the four outer levels of m_transformCache
  //--------------------------------------------------------------------------------------------------------------------
  struct CacheIndex
  {
    /// @brief ctor for CacheIndex struct, assigns both member variables
    CacheIndex(size_t _treeNum, size_t _id, size_t _age, size_t _innerIndex);
    /// @brief the indexes of each level of the nested cache structure
    size_t m_treeNum;
    size_t m_id;
    size_t m_age;
    size_t m_innerIndex;
  };

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the LSystems used to describe the trees in the forest
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<LSystem> m_treeTypes;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief number of trees of each LSystem type in forest
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<size_t> m_numTrees;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief width and length of forest
  //--------------------------------------------------------------------------------------------------------------------
  float m_width;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief stores positions and types of trees in the forest when scatterForest() is used to determine tree positions
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<Tree> m_treeData;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the number of hero trees created for each tree type to fill the instance cache
  //--------------------------------------------------------------------------------------------------------------------
  int m_numHeroTrees;  
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief terrain generation object storing the perlin noise module that determines the heightmap of the terrain
  //--------------------------------------------------------------------------------------------------------------------
  TerrainGenerator m_terrainGen;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief random number generator used for randomness in tree scattering and choosing instances
  //--------------------------------------------------------------------------------------------------------------------
  std::default_random_engine m_gen;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief seed for random number generator
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_seed = 0;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine if we should use a seed - if not, we seed by time
  //--------------------------------------------------------------------------------------------------------------------
  bool m_useSeed = false;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the cache of transform data representing transformations to apply to each branch instance to form a forest
  // arranged to mimic the structures of the instanceCaches of each treeType, with indexes of each level representing:
  // treeType / id / age / innerIndex / different-branches-using-the-same-instance
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<CACHE_STRUCTURE(std::vector<ngl::Mat4>)> m_transformCache;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief vector of CacheIndex objects representing transformCache index levels that have been recently changed
  /// to inform which vaos need to be rebuilt in NGLScene
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<CacheIndex> m_adjustedCacheIndexes;


  //PUBLIC METHODS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief resizes m_transformCache based on the sizes of the instance caches in each LSystem in m_treeTypes
  //--------------------------------------------------------------------------------------------------------------------
  void resizeTransformCache();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief seeds the random engine using m_seed or the current time depending on the state of m_useSeed
  //--------------------------------------------------------------------------------------------------------------------
  void seedRandomEngine();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief scatter points randomly across the terrain to fill m_treePositions
  //--------------------------------------------------------------------------------------------------------------------
  void scatterForest();
  //--------------------------------------------------------------------------------------------------------------------
  /// @ref Kenwood et al, Efficient Procedural Generation of Forests, 2014
  /// @brief recursively adds to m_transformCache to represent the create geometry of a tree by picking instances from
  /// the instance cache of one of the LSystems
  /// @param [in] treeType, the index (in m_treeTypes) of the LSystem whose instance cache we're using
  /// @param [in] transform, matrix representing the transform of the current instance relative to the origin
  /// @param [in] id, age, the id and age of the current branch instance
  //--------------------------------------------------------------------------------------------------------------------
  void createTree(size_t _treeType, ngl::Mat4 _transform, size_t _id, size_t _age);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief chooses a random instance from the instance cache of the given tree type at the given id, age and index
  //--------------------------------------------------------------------------------------------------------------------
  Instance * getInstance(LSystem &_treeType, size_t _id, size_t _age, size_t &_innerIndex);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief creates a forest by calling createTree() for each point in m_treeData
  //--------------------------------------------------------------------------------------------------------------------
  void createForest();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief different way of filling m_transformCache, by creating transform from a given position vector and using
  /// it to call createTree() - used to add points that have been painted onto the terrain to the forest
  //--------------------------------------------------------------------------------------------------------------------
  void addTreeToForest(ngl::Vec3 &_point, size_t _treeType);

};

#endif //FOREST_H_
