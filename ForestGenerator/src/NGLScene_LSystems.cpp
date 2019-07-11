//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_LSystems.cpp
/// @brief implementation file for NGLScene class initializeLSystems function
//----------------------------------------------------------------------------------------------------------------------

#include "NGLScene.h"

void NGLScene::initializeLSystems()
{
  m_LSystems.resize(m_numTabs);

  //LSystem 0:
  std::string axiom0 = "FFFA";
  std::vector<std::string> rules0 = {"A=\"[B]////[B]////B","B=&FFFA"};
  m_LSystems[0] = LSystem(axiom0,rules0);

  //LSystem 1:
  std::string axiom1 = "///A";
  std::vector<std::string> rules1 = {"A=F&[[A]^A]^F^[^FA]&A","F=FF"};
  m_LSystems[1] = LSystem(axiom1,rules1);
  m_LSystems[1].m_generation = 6;
  m_LSystems[1].m_stepSize = 1;
  m_LSystems[1].m_angle = 25;
}
