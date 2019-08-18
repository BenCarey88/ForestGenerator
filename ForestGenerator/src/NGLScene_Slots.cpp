//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_Slots.cpp
/// @brief implementation file for NGLScene class slots
//----------------------------------------------------------------------------------------------------------------------

#include <QGuiApplication>
#include "NGLScene.h"

//------------------------------------------------------------------------------------------------------------------------
///PUBLIC SLOTS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::changeSuperTab(int _superTabNum)
{
  m_superTabNum = size_t(_superTabNum);
  size_t i=0;
  if(_superTabNum==0)
  {
    i=m_treeTabNum;
    m_treePaintMode=false;
  }
  else if(_superTabNum==1)
  {
    if(m_forestTabNum==0)
    {
       //buildPaintedForestVAOs();
    }
    else if(m_forestTabNum==1)
    {
      updateForest();
    }
    i=m_forestTabNum;
    m_treePaintMode = (m_forestTabNum==0) ? m_savedTreePaintMode : false;
  }
  else if(_superTabNum==2)
  {
    m_treePaintMode=false;
    updateForest();
  }
  m_currentCamera = &m_cameras[m_superTabNum][i];
  m_currentMouseTransform = &m_mouseTransforms[m_superTabNum][i];

  update();
}

void NGLScene::changeTreeTab(int _treeTabNum)
{
  m_treeTabNum = size_t(_treeTabNum);
  m_currentCamera = &m_cameras[0][m_treeTabNum];
  m_currentMouseTransform = &m_mouseTransforms[0][m_treeTabNum];
  m_currentLSystem = &m_LSystems[m_treeTabNum];

  update();
}

void NGLScene::changeForestTab(int _forestTabNum)
{
  m_forestTabNum = size_t(_forestTabNum);
  m_currentCamera = &m_cameras[1][m_forestTabNum];
  m_currentMouseTransform = &m_mouseTransforms[1][m_forestTabNum];
  m_treePaintMode = (m_forestTabNum==0) ? m_savedTreePaintMode : false;
  if(m_forestTabNum==0)
  {
     //buildPaintedForestVAOs();
  }
  else if(m_forestTabNum==1)
  {
    updateForest();
  }

  update();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::generate()
{
  std::vector<std::string> currentRules = {};
  for(auto rule : m_currentLSystem->m_ruleArray)
  {
    if(rule!="")
    {
      currentRules.push_back(rule);
    }
  }
  m_currentLSystem->breakDownRules(currentRules);
  m_currentLSystem->seedRandomEngine();
  m_currentLSystem->createGeometry();
  m_buildTreeVAO = true;
  update();
}

void NGLScene::resetCamera()
{
  m_currentCamera->reset();
  (*m_currentMouseTransform) = m_initialRotation;
  update();
}

void NGLScene::toggleGrid(bool _clicked)
{
  m_showGrid[m_treeTabNum] = _clicked;
  update();
}

void NGLScene::toggleTreeSkeletonMode(bool _mode)
{
  m_currentLSystem->m_skeletonMode = _mode;
  update();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::setStepSize(double _stepSize)
{
  m_currentLSystem->m_stepSize = float(_stepSize);
}

void NGLScene::setStepScale(double _stepScale)
{
  m_currentLSystem->m_stepScale = float(_stepScale);
}

void NGLScene::setAngle(double _angle)
{
  m_currentLSystem->m_angle = float(_angle);
}

void NGLScene::setAngleScale(double _angleScale)
{
  m_currentLSystem->m_angleScale = float(_angleScale);
}

void NGLScene::setThickness(double _thickness)
{
  m_currentLSystem->m_thickness = float(_thickness);
}

void NGLScene::setThicknessScale(double _thicknessScale)
{
  m_currentLSystem->m_thicknessScale = float(_thicknessScale);
}

void NGLScene::setGeneration(int _generation)
{
  m_currentLSystem->m_generation = _generation;
}

void NGLScene::seedToggle(int _clicked)
{
  m_currentLSystem->m_useSeed = bool(_clicked);
}

void NGLScene::setSeed(int _seed)
{
  m_currentLSystem->m_seed = size_t(_seed);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::setAxiom(QString _axiom)
{
  m_currentLSystem->m_axiom = _axiom.toStdString();
}

#define SET_RULE(NUM)                                         \
  void NGLScene::setRule##NUM(QString _rule)                  \
  {                                                           \
    m_currentLSystem->m_ruleArray[NUM-1]=_rule.toStdString(); \
  }

SET_RULE(1)
SET_RULE(2)
SET_RULE(3)
SET_RULE(4)
SET_RULE(5)
SET_RULE(6)
SET_RULE(7)
SET_RULE(8)
SET_RULE(9)
SET_RULE(10)
SET_RULE(11)
SET_RULE(12)
SET_RULE(13)
SET_RULE(14)
SET_RULE(15)
SET_RULE(16)
SET_RULE(17)

#undef SET_RULE


//------------------------------------------------------------------------------------------------------------------------

void NGLScene::setOctaves(int _octaves)
{
  m_terrainGen.m_octaves = _octaves;
}

void NGLScene::setFrequency(double _frequency)
{
  m_terrainGen.m_frequency = _frequency*0.05;
}

void NGLScene::setLacunarity(double _lacunarity)
{
  m_terrainGen.m_lacunarity = _lacunarity;
}

void NGLScene::setPersistence(double _persistence)
{
  m_terrainGen.m_persistence = _persistence;
}

void NGLScene::setAmplitude(double _amplitude)
{
  m_terrainGen.m_amplitude = float(_amplitude);
}

void NGLScene::setTerrainSeed(double _seed)
{
  m_terrainGen.m_seed = _seed*50;
}

void NGLScene::updateTerrain()
{
  m_terrainGen.generate();
  m_terrain = TerrainData(m_terrainGen);
  update();
}

void NGLScene::setTolerance(double _tolerance)
{
  m_tolerance = float(_tolerance);
  update();
}

void NGLScene::toggleTerrainWireframe(bool _mode)
{
  m_terrainWireframe=_mode;
  update();
}

void NGLScene::setTerrainSize(double _terrainSize)
{
  m_width = float(_terrainSize);
  m_terrainGen.m_scale = m_width/m_terrainDimension;
}

void NGLScene::setLOD(int _LOD)
{
  m_terrainDimension = int(std::pow(2,_LOD))+1;
  m_terrainGen.m_dimension = m_terrainDimension;
  m_terrainGen.m_scale = m_width/m_terrainDimension;
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::toggleTreePaintMode(bool _mode)
{
  m_savedTreePaintMode = _mode;
  m_treePaintMode = _mode;
}

void NGLScene::setTreeSpacing(double _treeSpacing)
{

}

void NGLScene::setPaintBrush(int _brushNum)
{

}

void NGLScene::erasePaint()
{

}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::toggleTreeGenMethod(int _methodNum)
{

}
void NGLScene::setNumTrees(int _numTrees)
{

}

void NGLScene::setNumHeroTrees(int _numHeroTrees)
{

}

void NGLScene::setCacheLimit(int _cacheLimit)
{

}

void NGLScene::toggleForestTreeDisplay(bool _checked)
{

}
