//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_LSystems.cpp
/// @brief implementation file for NGLScene class initializeLSystems function
//----------------------------------------------------------------------------------------------------------------------

#include "NGLScene.h"

void NGLScene::initializeLSystems()
{
  m_LSystems.resize(m_numTreeTabs);
  m_LSystemVAOs.resize(m_numTreeTabs);

  std::string axiom;
  std::vector<std::string> rules;
  float stepSize;
  float stepScale;
  float angle;
  float angleScale;
  int generation;

  //LSystem 0:
  axiom = "FFFA";
  rules = {"A=\"[B]////[B]////B","B=&FFFA"};
  stepSize = 2;
  stepScale = 0.9f;
  angle = 30;
  angleScale = 0.9f;
  generation = 4;
  m_LSystems[0] = LSystem(axiom,rules,stepSize,stepScale,angle,angleScale,generation);

  //LSystem 1:
  axiom = "///A";
  rules = {"A=F&[[A]^A]^F^[^FA]&A","F=FF"};
  stepSize = 1;
  stepScale = 0.9f;
  angle = 25;
  angleScale = 0.9f;
  generation = 6;
  m_LSystems[1] = LSystem(axiom,rules,stepSize,stepScale,angle,angleScale,generation);
}

void NGLScene::updateForest()
{
  m_forest = Forest(m_LSystems, m_width, m_length, m_numTrees);

  LSystem &treeType = m_forest.m_treeTypes[0];
  std::vector<std::vector<std::vector<Instance>>> &instanceCache = treeType.m_instanceCache;
  m_instanceCacheVAOs.resize(instanceCache.size());

  for(size_t id=0; id<instanceCache.size(); id++)
  {
    //std::cout<<"id = "<<id<<'\n';

    m_instanceCacheVAOs[id].resize(instanceCache[id].size());
    for(size_t age=0; age<instanceCache[id].size(); age++)
    {
      //std::cout<<"  age = "<<age<<'\n';
      //std::cout<<"    size of this level of nesting is "<<instanceCache[id][age].size()<<'\n';
      m_instanceCacheVAOs[id][age].resize(instanceCache[id][age].size());
      for(size_t index=0; index<instanceCache[id][age].size(); index++)
      {
        Instance &instance = instanceCache[id][age][index];
        buildInstanceCacheVAO(treeType, instance, m_instanceCacheVAOs[id][age][index]);
      }
    }
    //std::cout<<'\n';
  }
  //std::cout<<"\n---------------------------------------\n";
}
