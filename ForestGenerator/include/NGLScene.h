//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @author Jonathan Macey, Ben Carey
/// @version 2.0
/// @date 05/07/19
/// Revision History :
/// 10/9/13: This is an initial version used for the new NGL6 / Qt 5 demos
/// 05/09/19: updated by Ben Carey for use in ForestGenerator project
//---------------------------------------------------------------------------------------------------------------------

#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <vector>

#include <ngl/AbstractVAO.h>
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include <ngl/ShaderLib.h>
#include "WindowParams.h"
#include <QOpenGLWindow>
#include <memory>
#include <math.h>
#include "Camera.h"
#include "Forest.h"
#include "Grid.h"
#include "TerrainData.h"

#include <QEvent>
#include <QResizeEvent>
#include <QOpenGLWidget>

//----------------------------------------------------------------------------------------------------------------------
/// @class NGLScene
/// @brief this class inherits from the Qt OpenGLWidget and allows us to use NGL to draw OpenGL
//----------------------------------------------------------------------------------------------------------------------


class NGLScene : public QOpenGLWidget
{

  Q_OBJECT

public:

  //CONSTRUCTORS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our NGL drawing class
  /// @param [in] parent the parent window to the class
  //----------------------------------------------------------------------------------------------------------------------
  NGLScene(QWidget *_parent);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief dtor must close down ngl and release OpenGL resources
  //----------------------------------------------------------------------------------------------------------------------
  ~NGLScene() override;

public slots:

  //SLOTS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the current supertab number for the qtGUI
  /// @param[in] superTabNum, the int passed from m_SuperTab in ui
  //----------------------------------------------------------------------------------------------------------------------
  void changeSuperTab(int _superTabNum);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the current L-System tab number for the qtGUI
  /// @param[in] treeTabNum, the int passed from m_treeTab in ui
  //----------------------------------------------------------------------------------------------------------------------
  void changeTreeTab(int _treeTabNum);

  void changeTerrainTab(int _terrainTabNum);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the axiom for the L-System
  /// @param[in] axiom, the QString passed from m_axiom in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setAxiom(QString _axiom);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slots to set the rules for the L-System
  /// @param[in] rule, the QString passed from m_rulei in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setRule1(QString _rule);
  void setRule2(QString _rule);
  void setRule3(QString _rule);
  void setRule4(QString _rule);
  void setRule5(QString _rule);
  void setRule6(QString _rule);
  void setRule7(QString _rule);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the size of steps in the L-system
  /// @param[in] stepSize, the double passed from m_stepSize in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setStepSize(double _stepSize);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the default scaling value of steps in the L-system
  /// @param[in] stepScale, the double passed from m_stepScale in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setStepScale(double _stepScale);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the default angle of rotations in the L-system
  /// @param[in] angle, the double passed from m_angle in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setAngle(double _angle);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the default scaling value of angles in the L-system
  /// @param[in] angleScale, the double passed from m_angleScale in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setAngleScale(double _angleScale);

  void setThickness(double _thickness);
  void setThicknessScale(double _thicknessScale);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the number of generations to generate for this L-System
  /// @param[in] generation, the int passed from m_generation in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setGeneration(int _generation);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to determine whether or not we'll use a seed for this L-System
  /// @param[in] clicked, the int passed from m_seedToggle in ui
  //----------------------------------------------------------------------------------------------------------------------
  void seedToggle(int _clicked);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the number of generations to generate for this L-System
  /// @param[in] seed, the int passed from m_seed in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setSeed(int _seed);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to tell QT to create a new tree from the L-System based on the user inputs
  //----------------------------------------------------------------------------------------------------------------------
  void generate();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to tell QT to reset the camera values
  //----------------------------------------------------------------------------------------------------------------------
  void resetCamera();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to toggle wether or not we display the grid
  /// @param[in] _clicked, the int passed from m_grid in ui
  //----------------------------------------------------------------------------------------------------------------------
  void toggleGrid(bool _clicked);

  void toggleTreeSkeletonMode(bool _mode);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to toggle wireframe mode
  /// @param[in] mode, the mode passed from the togglebutton
  //----------------------------------------------------------------------------------------------------------------------
  void toggleTerrainWireframe( bool _mode );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the number of octaves for the terrain generation
  /// @param[in] octaves, the int passed from m_octaves in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setOctaves(int _octaves);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the frequency for the terrain generation
  /// @param[in] frequency, the double passed from m_frequency in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setFrequency(double _frequency);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the lacunarity for the terrain generation
  /// @param[in] lacunarity, the double passed from m_lacunarity in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setLacunarity(double _lacunarity);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the persistence for the terrain generation
  /// @param[in] persistence, the double passed from m_persistence in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setPersistence(double _persistence);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the amplitude for the terrain generation
  /// @param[in] amplitude, the double passed from m_amplitude in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setAmplitude(double _amplitude);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the seed for the terrain generation
  /// @param[in] seed, the double passed from m_terrainSeed in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setTerrainSeed(double _seed);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to generate a new terrain based on current values
  //----------------------------------------------------------------------------------------------------------------------
  void updateTerrain();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the tolerance for the LOD algorithm
  /// @param[in] tolerance, the double passed from m_tolerance in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setTolerance(double _tolerance);

  void setTerrainSize(double _terrainSize);
  void setLOD(int _LOD);

protected:

  //PROTECTED MEMBER VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of superTabs and treeTabs
  //----------------------------------------------------------------------------------------------------------------------
  const size_t m_numSuperTabs = 3;
  const size_t m_numTreeTabs = 2;
  const size_t m_numTerrainTabs = 2;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the current superTab number
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_superTabNum = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the current treeTab number
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_treeTabNum = 0;

  size_t m_terrainTabNum = 0;

  std::vector<ngl::Vec3> m_points = {};
  std::vector<GLshort> m_pointIndices = {};

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief matrices to set up camera view
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_view;
  ngl::Mat4 m_project;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief initial rotation matrix to make grid alignment look nicer
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_initialRotation = ngl::Mat4(std::sqrt(2.0f)*0.5f, 0,  -std::sqrt(2.0f)*0.5f,0,
                                          0,                    1,  0,                    0,
                                          std::sqrt(2.0f)*0.5f, 0,  std::sqrt(2.0f)*0.5f, 0,
                                          0,                    0,  0,                    1);

  ngl::Mat4 m_layoutRotation = m_initialRotation.inverse() * ngl::Mat4(1,0,0,0,
                                                                       0,0,1,0,
                                                                       0,1,0,0,
                                                                       0,0,0,1);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the grid representing the ground plane
  //----------------------------------------------------------------------------------------------------------------------
  Grid m_grid = Grid(7,5);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to decide whether we display the grid
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<bool> m_showGrid;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief grid VAO
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_gridVAO;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief terrain VAO
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_terrainVAO;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief VAOs for the LSystems
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<std::unique_ptr<ngl::AbstractVAO>> m_treeVAOs;

  std::vector<std::unique_ptr<ngl::AbstractVAO>> m_leafVAOs;

  //nested std::vector of VAOs corresponding to instance caches for the forest rendering
  //outer layer separates the instance caches of the differing tree types
  //second layer separates within a cache by id
  //third layer separates by age
  //inner index corresponds to different instances of a given age and id
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_forestVAOs;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief bool to tell paintGL whether or not we need to rebuild the current LSystem VAO
  //----------------------------------------------------------------------------------------------------------------------
  bool m_buildTreeVAO = false;

  bool m_buildForestVAOs = false;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the forest object to be sent to the renderer
  //----------------------------------------------------------------------------------------------------------------------
  Forest m_forest;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief width and number of trees for m_forest
  //----------------------------------------------------------------------------------------------------------------------
  float m_width = 2000;
  size_t m_numTrees = 1000;
  int m_numHeroTrees = 10;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief list of all L-Systems stored by the scene
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<LSystem> m_LSystems;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief list of all cameras for the screens - stored as nested vectors to reflect the nested tabs
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<std::vector<Camera>> m_cameras;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief list of all mouseTransforms for the screens - stored as nested vectors to reflect the nested tabs
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<std::vector<ngl::Mat4>> m_mouseTransforms;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief pointer to the currently active (or most recently active) L-System
  //----------------------------------------------------------------------------------------------------------------------
  LSystem * m_currentLSystem;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief pointer to the camera for the currently active screen
  //----------------------------------------------------------------------------------------------------------------------
  Camera * m_currentCamera;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief pointer to the mouseTransform for the currently active screen
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 * m_currentMouseTransform;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief TerrainData object which applies LOD algorithms to the heightmap values from m_terrainValues and creates
  /// a vertex list to be passed into paintGL
  //----------------------------------------------------------------------------------------------------------------------
  TerrainData m_terrain;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief float to store the user-specified error tolerance passed into the LOD algorithm in m_terrain
  //----------------------------------------------------------------------------------------------------------------------
  float m_tolerance = 0.02f;

  TerrainGenerator m_terrainGen;

  int m_terrainDimension = 129;

  bool m_terrainWireframe = false;

  bool m_rotate = true;


  //PROTECTED MEMBER FUNCTIONS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the initialize class is called once when the window is created and we have a valid GL context
  /// use this to setup any default GL stuff
  //----------------------------------------------------------------------------------------------------------------------
  void initializeGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we want to draw the scene
  //----------------------------------------------------------------------------------------------------------------------
  void paintGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we resize
  //----------------------------------------------------------------------------------------------------------------------
  void resizeGL(int _w, int _h) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build an openGL line VAO from lists of vertices and indices (used by paintGL)
  //----------------------------------------------------------------------------------------------------------------------
  template <class dataType>
  void buildSimpleIndexVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                           std::vector<dataType> &_indices, GLenum _mode, GLenum _indexType);

  void buildInstanceCacheVAO(std::unique_ptr<ngl::AbstractVAO> &_vao,
                             LSystem &_treeType, Instance &_instance,
                             std::vector<ngl::Mat4> &_transforms);

  void addBufferToBoundVAO(size_t _bufferSize, const GLvoid * _bufferData);
  void buildGridVAO();
  void buildTerrainVAO();
  void buildTreeVAO(size_t _treeNum);
  void buildLeafVAO(size_t _treeNum);

  void buildForestVAOs();

  void loadTextures(ngl::ShaderLib *_shader, const std::string &_shaderName,
                    const char *_textureMapFile, const char *_normalMapFile);
  void loadUniformsToShader(ngl::ShaderLib *_shader, const std::string &_shaderName);

  void drawVAO(std::unique_ptr<ngl::AbstractVAO> &_VAO);
  void refineTerrain();

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set up the initial L-Systems for each treeTab screen, and sends them to the Forest class
  //----------------------------------------------------------------------------------------------------------------------
  void initializeLSystems();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief updates forest class when L-Systems change
  //----------------------------------------------------------------------------------------------------------------------
  void updateForest();

  //EVENTS
  //----------------------------------------------------------------------------------------------------------------------
  void mouseMoveEvent (QMouseEvent * _event ) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mousePressEvent ( QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseReleaseEvent ( QMouseEvent *_event ) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse wheel is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void wheelEvent( QWheelEvent *_event) override;

};



#endif //NGLSCENE_H_
