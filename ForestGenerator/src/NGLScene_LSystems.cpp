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
  m_forest = Forest(m_LSystems, m_width, m_length, m_numTrees, m_numHeroTrees);

  m_buildInstanceVAO = true;
}
