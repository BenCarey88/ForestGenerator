//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainGenerator.cpp
/// @brief implementation file for TerrainGenerator class
//----------------------------------------------------------------------------------------------------------------------

#include "TerrainGenerator.h"


TerrainGenerator::TerrainGenerator(int _dimension) : m_dimension(_dimension) {}


void TerrainGenerator::generate()
{
  noise::module::Perlin perlinModule;

  perlinModule.SetOctaveCount(m_octaves);
  perlinModule.SetFrequency(m_frequency);
  perlinModule.SetPersistence(m_persistence);
  perlinModule.SetLacunarity(m_lacunarity);

  m_heightMap = {};

  for (int i =0; i<m_dimension*m_dimension; ++i)
  {
    float height = float(perlinModule.GetValue(getX(i)/10,getY(i)/10,m_seed));
    m_heightMap.push_back(m_amplitude*height);
  }
}


double TerrainGenerator::getX(const int _index) const
{
  return _index%m_dimension;
}

double TerrainGenerator::getY(const int _index) const
{
  return _index/m_dimension;
}
