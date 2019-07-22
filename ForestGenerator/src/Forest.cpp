//----------------------------------------------------------------------------------------------------------------------
/// @file Forest.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <math.h>
#include <chrono>
#include "Forest.h"

Forest::Forest(const std::vector<LSystem> &_treeTypes, float _width, float _length, size_t _numTrees) :
  m_treeTypes(_treeTypes), m_width(_width), m_length(_length), m_numTrees(_numTrees)
{
  scatterForest();

  int i=1;
  for(auto &treeType : m_treeTypes)
  {
    treeType.m_name = std::to_string(i);
    i++;
    treeType.fillInstanceCache(m_numHeroTrees);
    /*for(auto branch : treeType.m_branches)
    {
      std::cout<<branch<<'\n';
    }*/
  }

  createForest();
}

//----------------------------------------------------------------------------------------------------------------------

Forest::Tree::Tree(size_t _type, ngl::Mat4 _transform) :
  m_type(_type), m_transform(_transform) {}

Forest::OutputData::OutputData(ngl::Mat4 _transform, size_t _id, size_t _age, size_t _innerIndex) :
  m_transform(_transform), m_id(_id), m_age(_age), m_innerIndex(_innerIndex) {}

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
  std::uniform_real_distribution<float> distZ(-m_length*0.5f, m_length*0.5f);
  std::uniform_real_distribution<float> distRotate(0,360);
  std::uniform_real_distribution<float> distScale(0.2f,0.4f);
  std::uniform_int_distribution<size_t> distTreeType(0,m_treeTypes.size()-1);

  /*noise::module::Perlin perlinModule;
  std::vector<std::vector<float>> densityMap;
  size_t width = 1000;
  size_t length = 1000;
  densityMap.resize(width);
  for (size_t i=0; i<width; i++)
  {
    densityMap[i].resize(length);
    for(size_t j=0; j<length; j++)
    {
      densityMap[i][j] = 0.5f*(1+float(perlinModule.GetValue(double(i)/10.0,double(j)/10.0,seed)));
    }
  }
  for(size_t i=0; i<width; i++)
  {
    for(size_t j=0; j<length; j++)
    {
      float oldValue = densityMap[i][j]+1;
      float newValue = round(oldValue);
      float error = oldValue - newValue;
      densityMap[i][j] = newValue;

      if(i+1<width)
        densityMap[i+1][j] += error * 7/16;
      if(int(i)-1>=0 && j+1<length)
        densityMap[i-1][j+1] += error * 3/16;
      if(j+1<length)
        densityMap[i][j+1] += error * 5/16;
      if(i+1<width && j+1<length)
        densityMap[i+1][j+1] += error * 1/16;
    }
  }

  for(size_t i=0; i<width; i++)
  {
    for(size_t j=0; j<length; j++)
    {
      if(int(densityMap[i][j])==1)
      {
        ngl::Mat4 position;
        ngl::Mat4 orientation;
        ngl::Mat4 scale;
        std::cout << -m_width*0.5f+i*m_width/width << " " << -m_length*0.5f+j*m_length/length << "\n";
        position.translate(-m_width*0.5f+i*m_width/width,0,-m_length*0.5f+j*m_length/length);
        orientation.rotateY(distRotate(gen));
        scale = distScale(gen)*scale;
        m_treeData.push_back(Tree(m_treeTypes[distTreeType(gen)], position*orientation*scale));
      }
    }
  }*/


  for(size_t i=0; i<m_numTrees; i++)
  {
    ngl::Mat4 position;
    ngl::Mat4 orientation;
    ngl::Mat4 scale;
    position.translate(distX(m_gen),0,distZ(m_gen));
    orientation.rotateY(distRotate(m_gen));
    scale = distScale(m_gen)*scale;
    //m_treeData.push_back(Tree(distTreeType(m_gen), position*orientation*scale));
    m_treeData.push_back(Tree(0,position*orientation*scale));
  }
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::createTree(LSystem &_treeType, ngl::Mat4 _transform, size_t _id, size_t _age)
{
  size_t size = _treeType.m_instanceCache.at(_id).at(_age).size();
  if(size>0)
  {
    size_t innerIndex = 0;
    Instance * instance = getInstance(_treeType, _id, _age, innerIndex);
    ngl::Mat4 T = _transform * instance->m_transform.inverse();
    m_output.push_back(OutputData(T, _id, _age, innerIndex));
    for(size_t i=0; i<instance->m_exitPoints.size(); i++)
    {
      size_t newAge = instance->m_exitPoints[i].m_exitAge;
      size_t newId = instance->m_exitPoints[i].m_exitId;
      ngl::Mat4 exitTransform = instance->m_exitPoints[i].m_exitTransform;
      ngl::Mat4 newTransform = _transform * exitTransform;
      createTree(_treeType, newTransform, newId, newAge);
    }
  }
  else
  {
    //std::cout<<"Couldn't find instance of tree type "<<_treeType.m_name<<" with id "<<_id<<" at age "<<_age<<'\n';
  }
}

Instance * Forest::getInstance(LSystem &_treeType, size_t _id, size_t _age, size_t &_innerIndex)
{
  size_t size = _treeType.m_instanceCache.at(_id).at(_age).size();
  std::uniform_int_distribution<size_t> dist(0,size-1);
  _innerIndex = dist(m_gen);
  return &_treeType.m_instanceCache.at(_id).at(_age).at(_innerIndex);
}

//----------------------------------------------------------------------------------------------------------------------

void Forest::createForest()
{
  seedRandomEngine();
  m_output = {};
  for(auto &tree : m_treeData)
  {
    createTree(m_treeTypes[tree.m_type],tree.m_transform,0,0);
  }
}
