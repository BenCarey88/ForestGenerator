//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_LSystems.cpp
/// @brief implementation file for NGLScene class initializeLSystems function
//----------------------------------------------------------------------------------------------------------------------

#include "NGLScene.h"

void NGLScene::initializeLSystems()
{
  m_LSystems.resize(m_numTreeTabs);

  std::string axiom;
  std::vector<std::string> rules;
  float stepSize;
  float stepScale;
  float angle;
  float angleScale;
  float generation;

  //LSystem 0:
  axiom = "FFFA";
  rules = {"A=\"[B]////[B]////B","B=&FFFA"};
  stepSize = 2;
  stepScale = 0.9;
  angle = 30;
  angleScale = 0.9;
  generation = 4;
  m_LSystems[0] = LSystem(axiom,rules,stepSize,stepScale,angle,angleScale,generation);

  //LSystem 1:
  axiom = "///A";
  rules = {"A=F&[[A]^A]^F^[^FA]&A","F=FF"};
  stepSize = 1;
  stepScale = 0.9;
  angle = 25;
  angleScale = 0.9;
  generation = 6;
  m_LSystems[1] = LSystem(axiom,rules,stepSize,stepScale,angle,angleScale,generation);
}