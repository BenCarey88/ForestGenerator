//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainGenerator.cpp
/// @brief implementation file for TerrainGenerator class
//----------------------------------------------------------------------------------------------------------------------

#include <ngl/Vec2.h>
#include "TerrainGenerator.h"
#include "PrintFunctions.h"

TerrainGenerator::TerrainGenerator(int _dimension, float _width) :
  m_dimension(_dimension), m_scale(_width/_dimension) {}

void TerrainGenerator::generate()
{
  noise::module::Perlin perlinModule;

  perlinModule.SetOctaveCount(m_octaves);
  perlinModule.SetFrequency(m_frequency);
  perlinModule.SetPersistence(m_persistence);
  perlinModule.SetLacunarity(m_lacunarity);

  m_heightMap = {};

  for (int i=0; i<m_dimension*m_dimension; ++i)
  {
    float height = float(perlinModule.GetValue(getSceneX(i),getSceneZ(i),m_seed));
    m_heightMap.push_back(m_amplitude*height);
  }

  computeNormals();
}

void TerrainGenerator::computeNormals()
{
  m_normals = {};
  m_tangents = {};
  m_bitangents = {};
  m_normals.resize(m_heightMap.size(),{0,1,0});
  m_tangents.resize(m_heightMap.size(),{0,1,0});
  m_bitangents.resize(m_heightMap.size(),{0,1,0});
  for (size_t i=0; i<m_normals.size(); i++)
  {
    int x=getGridX(int(i));
    int z=getGridZ(int(i));

    ///////////////
    //     T     //
    //  L  V  R  //
    //     B     //
    ///////////////

    if(x!=0 && x!=m_dimension-1 && z!=0 && z!=m_dimension-1)
    {
      float T = m_heightMap[getIndex(x,z-1)];
      float L = m_heightMap[getIndex(x-1,z)];
      float R = m_heightMap[getIndex(x+1,z)];
      float B = m_heightMap[getIndex(x,z+1)];

      ngl::Vec3 normal(L-R, 2*m_scale, T-B);
      ngl::Vec3 tangent(2*m_scale,R-L,0);
      ngl::Vec3 bitangent = normal.cross(tangent);

      normal.normalize();
      tangent.normalize();
      bitangent.normalize();

      m_normals[i]=normal;
      m_tangents[i]=tangent;
      m_bitangents[i]=bitangent;
    }
  }
}


double TerrainGenerator::getSceneX(const int _index) const
{
  return ((_index%m_dimension)-m_dimension/2)*double(m_scale);
}

double TerrainGenerator::getSceneZ(const int _index) const
{
  return ((_index/m_dimension)-m_dimension/2)*double(m_scale);
}

int TerrainGenerator::getGridX(const int _index) const
{
  return _index%m_dimension;
}
int TerrainGenerator::getGridZ(const int _index) const
{
  return _index/m_dimension;
}
size_t TerrainGenerator::getIndex(const int _gridX, const int _gridZ) const
{
  return size_t(m_dimension*_gridZ + _gridX);
}
