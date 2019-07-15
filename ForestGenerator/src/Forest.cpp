//----------------------------------------------------------------------------------------------------------------------
/// @file Forest.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include "noise.h"
#include <math.h>
#include <random>
#include <chrono>
#include "Forest.h"

Forest::Forest(std::vector<LSystem> _treeTypes, float _width, float _length, size_t _numTrees) :
  m_treeTypes(_treeTypes), m_width(_width), m_length(_length), m_numTrees(_numTrees)
{
  scatterForest();
}

Forest::Tree::Tree(size_t _type, ngl::Mat4 _transform) :
  m_type(_type), m_transform(_transform) {}

void Forest::scatterForest()
{
  m_treeData = {};

  size_t seed;
  if(m_useSeed)
  {
    seed = m_seed;
  }
  else
  {
    seed = size_t(std::chrono::system_clock::now().time_since_epoch().count());
  }

  std::default_random_engine gen;
  gen.seed(seed);
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
    position.translate(distX(gen),0,distZ(gen));
    orientation.rotateY(distRotate(gen));
    scale = distScale(gen)*scale;
    m_treeData.push_back(Tree(distTreeType(gen), position*orientation*scale));
  }
}
