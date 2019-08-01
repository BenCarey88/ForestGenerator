//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainGenerator.h
/// @author Ben Carey
/// @version 1.0
/// @date 23/01/19 updated to NCCA coding standards
/// Revision History :
/// Initial Version 21/01/19, used for ASE TerrainGeneration project
/// 01/09/19 added to ForestGeneration masters project
//----------------------------------------------------------------------------------------------------------------------

#ifndef TERRAINGENERATOR_H_
#define TERRAINGENERATOR_H_

#include <iostream>
#include <vector>
#include "noiseutils.h"

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
  TerrainGenerator(int _dimension);
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
  double m_frequency = 1;
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
  float m_amplitude = 10;

  //PUBLIC MEMBER FUNCTION
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief assigns values to m_heightmap based on current noise attributes
  //--------------------------------------------------------------------------------------------------------------------
  void generate();

private:

  //PRIVATE MEMBER FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the x coordinate corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  double getX(const int _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the y coordinate corresponding to a particular heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  double getY(const int _index) const;

};

#endif //TERRAINGENERATOR_H_
