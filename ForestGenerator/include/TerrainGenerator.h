//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainGenerator.h
/// @author Ben Carey
/// @version 1.0
/// @date 23/01/19 updated to NCCA coding standards
/// Revision History :
/// Initial Version 21/01/19, used for ASE TerrainGeneration project
/// 01/09/19 added to ForestGeneration masters project and slightly updated
//----------------------------------------------------------------------------------------------------------------------

#ifndef TERRAINGENERATOR_H_
#define TERRAINGENERATOR_H_

#include <iostream>
#include <vector>
#include "noiseutils.h"
#include "ngl/Vec3.h"
#include "ngl/Vec2.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class TerrainGenerator
/// @brief this class uses libnoise modules to generate heightmap values for the TerrainData class
/// @todo tidying up variable and method names
//----------------------------------------------------------------------------------------------------------------------

class TerrainGenerator
{
public:
  //CONSTRUCTOR FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for our Generator class
  //--------------------------------------------------------------------------------------------------------------------
  TerrainGenerator()=default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our Generator class
  /// @param [in] dimension, [in] dimension, the dimension of the terrain being generated
  /// (this must be 2^n+1 for some positive integer n, based on the way the algorithm is designed)
  //--------------------------------------------------------------------------------------------------------------------
  TerrainGenerator(int _dimension, float _width);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default dtor for our Generator class
  //--------------------------------------------------------------------------------------------------------------------
  ~TerrainGenerator()=default;

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief dimension of terrain being generated (must be 2^n+1 for some n)
  //--------------------------------------------------------------------------------------------------------------------
  int m_dimension;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief heightmap values to be passed into TerrainData class
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<float> m_heightMap;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief lists of normals, tangents, bitangents and UV coordinates for each vertex, calculated from their
  /// positions in the generated heightMap, to be passed to terrain data for rendering
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_normals;
  std::vector<ngl::Vec3> m_tangents;
  std::vector<ngl::Vec3> m_bitangents;
  std::vector<ngl::Vec2> m_UVs;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the scale of the grid, used to determine the world space coordinates of a vertex from its heightMap index
  //--------------------------------------------------------------------------------------------------------------------
  float m_scale;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief seed used to randomise terrain generation
  //--------------------------------------------------------------------------------------------------------------------
  double m_seed = 0;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief number of octaves in libnoise Perlin module used to generate heightmap values
  //--------------------------------------------------------------------------------------------------------------------
  int m_octaves = 3;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief frequency of libnoise Perlin module used to generate heightmap values
  //--------------------------------------------------------------------------------------------------------------------
  double m_frequency = 0.005;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief persistence of libnoise Perlin module used to generate heightmap values
  //--------------------------------------------------------------------------------------------------------------------
  double m_persistence = 0.8;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief lacunarity of libnoise Perlin module used to generate heightmap values
  //--------------------------------------------------------------------------------------------------------------------
  double m_lacunarity = 0.5;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief amplitude of heightmap values generated
  //--------------------------------------------------------------------------------------------------------------------
  float m_amplitude = 50;


  //PUBLIC MEMBER FUNCTION
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief assigns values to m_heightmap based on current noise attributes
  //--------------------------------------------------------------------------------------------------------------------
  void generate();

private:

  //PRIVATE MEMBER FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the x coordinate in worldSpace corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  double getSceneX(const int _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the z coordinate in worldSpace corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  double getSceneZ(const int _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the x coordinate in the integer-spaced grid corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  int getGridX(const int _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the z coordinate in the integer-spaced grid corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  int getGridZ(const int _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the heightmap index corresponding to the grid coordinates (_gridX, _gridZ)
  //--------------------------------------------------------------------------------------------------------------------
  size_t getIndex(const int _gridX, const int _gridZ) const;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief computes normals, tangents and bitangents of each vertex in the heightmap
  //--------------------------------------------------------------------------------------------------------------------
  void computeNormals();

};

#endif //TERRAINGENERATOR_H_
