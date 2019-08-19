//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_LSystems.cpp
/// @brief implementation file for NGLScene class initializeLSystems function and updateForest function
//----------------------------------------------------------------------------------------------------------------------

#include "NGLScene.h"

void NGLScene::initializeLSystems()
{
  m_LSystems.resize(m_numTreeTabs);
  m_treeVAOs.resize(m_numTreeTabs);
  m_leafVAOs.resize(m_numTreeTabs);
  m_polygonVAOs.resize(m_numTreeTabs);

  std::string axiom;
  std::vector<std::string> rules;
  float stepSize;
  float stepScale;
  float angle;
  float angleScale;
  float thickness;
  float thicknessScale;
  int generation;

  //LSystem 0:
  axiom = "///FFFFFFFA";
  rules = {"A=![&FL!A]//[&FL!A]/////[&FL!A]:0.5",
           "A=![&FL!A]//////[&FL!A]/////////[&FL!A]:0.5",
           "&F=&S/////F",
           "S=FL:0.6",
           "S=FFL:0.4",
           "L=[^\"\"\"\"\"\"{.-f.+f.+f.--(180)-f.+f.+f.}]"};
  stepSize = 1.2f;
  stepScale = 0.9f;
  angle = 30;
  angleScale = 0.9f;
  generation = 20;
  thickness = 1.2f;
  thicknessScale = 0.7f;
  m_LSystems[0] = LSystem(axiom,rules,
                          stepSize,stepScale,
                          angle,angleScale,
                          thickness,thicknessScale,
                          generation);

  //LSystem 1:
  axiom = "///A";
  rules = {"A=F&[![A]^!A]^F^[!^FA]&!A","F=FF"};
  stepSize = 1;
  stepScale = 0.9f;
  angle = 25;
  angleScale = 0.9f;
  generation = 6;
  thickness = 1;
  thicknessScale = 0.6f;
  m_LSystems[1] = LSystem(axiom,rules,
                          stepSize,stepScale,
                          angle,angleScale,
                          thickness,thicknessScale,
                          generation);

  //LSystem 2:
  axiom = "FFFA";
  rules = {"A=!\"[BJ]////[BJ]////BJ",
           "B=&FFFA:0.2",
           "B=&[!!C]FFFA:0.2",
           "B=&F[!!C]FFA:0.2",
           "B=&FF[!!C]FA:0.2",
           "B=&FFF[!!C]A:0.2",
           "C=/&F!FJ:0.3",
           "C=\\&FJ:0.3",
           "C=&FJ:0.3"};
  stepSize = 2;
  stepScale = 0.9f;
  angle = 30;
  angleScale = 0.9f;
  generation = 18;
  thickness = 1;
  thicknessScale = 0.7f;
  m_LSystems[2] = LSystem(axiom,rules,
                          stepSize,stepScale,
                          angle,angleScale,
                          thickness,thicknessScale,
                          generation);
}
