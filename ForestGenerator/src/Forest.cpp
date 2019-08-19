//----------------------------------------------------------------------------------------------------------------------
/// @file Forest.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <math.h>
#include <chrono>
#include "noiseutils.h"
#include "Forest.h"


//FOREST CONSTRUCTORS
//----------------------------------------------------------------------------------------------------------------------

Forest::Forest(const std::vector<LSystem> &_treeTypes, float _width,
               std::vector<size_t> &_numTrees, int _numHeroTrees,
               TerrainGenerator _terrainGen,
               size_t _seed, bool _useSeed) :
  m_treeTypes(_treeTypes), m_numTrees(_numTrees),
  m_width(_width), m_numHeroTrees(_numHeroTrees),
  m_terrainGen(_terrainGen),
  m_seed(_seed), m_useSeed(_useSeed)
{
  scatterForest();

  for(auto &treeType : m_treeTypes)
  {
    treeType.fillInstanceCache(m_numHeroTrees);
  }
  createForest();
}

Forest::Forest(const std::vector<LSystem> &_treeTypes, int _numHeroTrees,
               size_t _seed, bool _useSeed) :
  m_treeTypes(_treeTypes), m_numHeroTrees(_numHeroTrees),
  m_seed(_seed), m_useSeed(_useSeed)
{
  for(auto &treeType : m_treeTypes)
  {
    treeType.fillInstanceCache(m_numHeroTrees);
  }
  resizeTransformCache();
}


//STRUCT CONSTRUCTORS
//----------------------------------------------------------------------------------------------------------------------

Forest::Tree::Tree(size_t _type, ngl::Mat4 _transform) :
  m_type(_type), m_transform(_transform) {}

Forest::CacheIndex::CacheIndex(size_t _treeNum, size_t _id, size_t _age, size_t _innerIndex) :
  m_treeNum(_treeNum), m_id(_id), m_age(_age), m_innerIndex(_innerIndex) {}


//----------------------------------------------------------------------------------------------------------------------

void Forest::resizeTransformCache()
{
  m_transformCache={};
  m_transformCache.resize(m_treeTypes.size());
  for(size_t t=0; t<m_treeTypes.size(); t++)
  {
    RESIZE_CACHE_BY_OTHER_CACHE(m_transformCache[t], m_treeTypes[t].m_instanceCache)
  }
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::seedRandomEngine()
{
  size_t seed;
  if(m_useSeed)
  {
    seed = m_seed;
  }
  else
  {
    seed = size_t(std::chrono::system_clock::now().time_since_epoch().count());
  }
  m_gen.seed(seed);
}


//----------------------------------------------------------------------------------------------------------------------

void Forest::scatterForest()
{
  seedRandomEngine();
  m_treeData = {};

  std::uniform_real_distribution<float> distX(-m_width*0.5f, m_width*0.5f);
  std::uniform_real_distribution<float> distZ(-m_width*0.5f, m_width*0.5f);
  std::uniform_real_distribution<float> distRotate(0,360);
  std::uniform_real_distribution<float> distScale(2,3);

  //set perlinModule identically to m_terrainGenerator to ensure forest positions
  //correspond to the terrain
  noise::module::Perlin perlinModule;
  perlinModule.SetOctaveCount(m_terrainGen.m_octaves);
  perlinModule.SetFrequency(m_terrainGen.m_frequency);
  perlinModule.SetPersistence(m_terrainGen.m_persistence);
  perlinModule.SetLacunarity(m_terrainGen.m_lacunarity);

  for(size_t t=0; t<m_treeTypes.size(); t++)
  {
    for(size_t i=0; i<m_numTrees[t]; i++)
    {
      ngl::Mat4 position;
      ngl::Mat4 orientation;
      float s = distScale(m_gen);
      float xPos = distX(m_gen);
      float zPos = distZ(m_gen);
      float yPos = float(perlinModule.GetValue(double(xPos),
                                               double(zPos),
                                               m_terrainGen.m_seed));
      yPos *= m_terrainGen.m_amplitude;
      position.translate(xPos,yPos,zPos);
      orientation.rotateY(distRotate(m_gen));
      ngl::Mat4 scale(s, 0, 0, 0,
                      0, s, 0, 0,
                      0, 0, s, 0,
                      0, 0, 0, 1);
      m_treeData.push_back(Tree(t, position*orientation));//*scale));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::createTree(size_t _treeType, ngl::Mat4 _transform, size_t _id, size_t _age)
{
  ///@ref Kenwood et al, Efficient Procedural Generation of Forests, 2014

  LSystem &treeType = m_treeTypes[_treeType];
  size_t size = treeType.m_instanceCache[_id][_age].size();
  //first check there is an instance at the given id and age of the cache
  if(size>0)
  {
    size_t innerIndex = 0;
    //pick a random instance of the given id and age
    Instance * instance = getInstance(treeType, _id, _age, innerIndex);
    //find the worldspace transform of this new instance from the current transform and the relative instance transform
    ngl::Mat4 T = _transform * instance->m_transform.inverse();
    //and add it to the transform cache
    m_transformCache[_treeType][_id][_age][innerIndex].push_back(T);
    //add current indexes to m_cacheIndexes to tell NGLScene which VAOs need rebuilding
    //(only necessary when used for painting on forests)
    m_adjustedCacheIndexes.push_back(CacheIndex(_treeType,_id,_age,innerIndex));

    //now iterate through exit points to recursively call the function again
    for(size_t i=0; i<instance->m_exitPoints.size(); i++)
    {
      size_t newAge = instance->m_exitPoints[i].m_exitAge;
      size_t newId = instance->m_exitPoints[i].m_exitId;
      ngl::Mat4 exitTransform = instance->m_exitPoints[i].m_exitTransform;
      //use the worldspace transform of the exit point, found from the current transform and the relative exit transform
      ngl::Mat4 newTransform = _transform * exitTransform;
      createTree(_treeType, newTransform, newId, newAge);
    }
  }
  else
  {
    //note that this shouldn't actually ever occur because createGeometry() ensures that no empty instance is called
    std::cout<<"Couldn't find instance of tree type "<<_treeType<<" with id "<<_id<<" at age "<<_age<<'\n';
  }
}

Instance * Forest::getInstance(LSystem &_treeType, size_t _id, size_t _age, size_t &_innerIndex)
{
  size_t size = _treeType.m_instanceCache.at(_id).at(_age).size();
  std::uniform_int_distribution<size_t> dist(0,size-1);
  _innerIndex = dist(m_gen);
  return &_treeType.m_instanceCache[_id][_age][_innerIndex];
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::createForest()
{
  seedRandomEngine();
  resizeTransformCache();
  for(auto &tree : m_treeData)
  {
    createTree(tree.m_type,tree.m_transform,0,0);
  }
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::addTreeToForest(ngl::Vec3 &_point, size_t _treeType)
{
  std::uniform_real_distribution<float> distRotate(0,360);
  ngl::Mat4 orientation;
  orientation.rotateY(distRotate(m_gen));
  ngl::Mat4 position;
  position.translate(_point.m_x, _point.m_y, _point.m_z);
  createTree(_treeType,position*orientation,0,0);
}
