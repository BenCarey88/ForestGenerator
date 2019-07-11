//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_Slots.cpp
/// @brief implementation file for NGLScene class slots
//----------------------------------------------------------------------------------------------------------------------

#include <QGuiApplication>

#include "NGLScene.h"

//------------------------------------------------------------------------------------------------------------------------
///PUBLIC SLOTS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::generate()
{
  std::vector<std::string> currentRules = {};
  for(auto rule : m_currentLSystem.m_ruleArray)
  {
    if(rule!="")
    {
      currentRules.push_back(rule);
    }
  }
  m_currentLSystem.m_rules = currentRules;
  m_currentLSystem.update();
  m_currentLSystem.createGeometry(m_currentLSystem.m_generation, ngl::Vec3(0,0,0));
  update();
}

void NGLScene::resetCamera()
{
  m_currentCamera.reset();
  m_currentMouseTransform.identity();
  update();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::setGeneration(int _generation)
{
  m_currentLSystem.m_generation = _generation;
}

void NGLScene::setStepSize(double _stepSize)
{
  m_currentLSystem.m_stepSize = float(_stepSize);
}

void NGLScene::setStepScale(double _stepScale)
{
  m_currentLSystem.m_stepScale = float(_stepScale);
}

void NGLScene::setAngle(double _angle)
{
  m_currentLSystem.m_angle = float(_angle);
}

void NGLScene::setAngleScale(double _angleScale)
{
  m_currentLSystem.m_angleScale = float(_angleScale);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::setAxiom(QString _axiom)
{
  m_currentLSystem.m_axiom = _axiom.toStdString();
}

void NGLScene::setRule1(QString _rule)
{
  m_currentLSystem.m_ruleArray[0]=_rule.toStdString();
}

void NGLScene::setRule2(QString _rule)
{
  m_currentLSystem.m_ruleArray[1]=_rule.toStdString();
}

void NGLScene::setRule3(QString _rule)
{
  m_currentLSystem.m_ruleArray[2]=_rule.toStdString();
}
void NGLScene::setRule4(QString _rule)
{
  m_currentLSystem.m_ruleArray[3]=_rule.toStdString();
}

void NGLScene::setRule5(QString _rule)
{
  m_currentLSystem.m_ruleArray[4]=_rule.toStdString();
}

void NGLScene::setRule6(QString _rule)
{
  m_currentLSystem.m_ruleArray[5]=_rule.toStdString();
}

void NGLScene::setRule7(QString _rule)
{
  m_currentLSystem.m_ruleArray[6]=_rule.toStdString();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::changeTab(int _tabNum)
{
  m_cameras[m_tabNum] = m_currentCamera;
  m_mouseTransforms[m_tabNum] = m_currentMouseTransform;
  m_LSystems[m_tabNum] = m_currentLSystem;

  m_tabNum = _tabNum;

  m_currentCamera = m_cameras[m_tabNum];
  m_currentMouseTransform = m_mouseTransforms[m_tabNum];
  m_currentLSystem = m_LSystems[m_tabNum];

  generate();
}
