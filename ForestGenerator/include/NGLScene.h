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
#include "noiseutils.h"

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
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the current forest tab number for the qtGUI
  /// @param[in] treeTabNum, the int passed from m_forestTab in ui
  //----------------------------------------------------------------------------------------------------------------------
  void changeForestTab(int _forestTabNum);

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
  void setRule8(QString _rule);
  void setRule9(QString _rule);
  void setRule10(QString _rule);
  void setRule11(QString _rule);
  void setRule12(QString _rule);
  void setRule13(QString _rule);
  void setRule14(QString _rule);
  void setRule15(QString _rule);
  void setRule16(QString _rule);
  void setRule17(QString _rule);

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
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the default thickness in the L-system
  /// @param[in] angleScale, the double passed from m_thicknessScale in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setThickness(double _thickness);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the default scaling value of thickness in the L-system
  /// @param[in] thicknessScale, the double passed from m_thicknessScale in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setThicknessScale(double _thicknessScale);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the number of generations to generate for the current L-System
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
  /// @brief a slot to toggle whether or not we display the grid
  /// @param[in] _clicked, the int passed from m_grid in ui
  //----------------------------------------------------------------------------------------------------------------------
  void toggleGrid(bool _clicked);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to toggle whether or not we display the trees with skeletal or cylindrical branches
  /// @param[in] _clicked, the int passed from m_skeleton in ui
  //----------------------------------------------------------------------------------------------------------------------
  void toggleTreeSkeletonMode(bool _mode);

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
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the tolerance for the LOD algorithm
  /// @param[in] tolerance, the double passed from m_tolerance in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setTerrainSize(double _terrainSize);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to toggle wireframe mode
  /// @param[in] mode, the mode passed from the togglebutton
  //----------------------------------------------------------------------------------------------------------------------
  void toggleTerrainWireframe( bool _mode );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the base level of detail for the terrain, which
  /// determines the initial dimensions of the heightmap grid
  /// @param[in] LOD, the int passed from m_LOD
  //----------------------------------------------------------------------------------------------------------------------
  void setLOD(int _LOD);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to toggle whether or not we are currently painting trees onto the terrain
  /// @param[in] mode, the mode passed from m_paint
  //----------------------------------------------------------------------------------------------------------------------
  void toggleTreePaintMode( bool _mode );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to determine the minimum acceptable distance between trees painted on the terrain
  /// @param[in] _treeSpacing, the double passed from m_treeSpacing in the ui
  //----------------------------------------------------------------------------------------------------------------------
  void setTreeSpacing(double _treeSpacing);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set which tree we're currently painting with
  /// @param[in] _brushNum, the int passed from m_paintBrush in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setPaintBrush(int _brushNum);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to erase all painted trees and update brushes to use the latest versions of all L-systems
  /// note that we have to erase all previous trees before we can update brushes because updating the brushes involves
  /// refilling the hero buffers and instance caches of each tree which will disrupt any currently painted trees
  //----------------------------------------------------------------------------------------------------------------------
  void erasePaint();

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to switch how we're currently generating forests: painting trees or randomly scattering
  /// @param[in] _methodNum, the int passed from m_treeGenMethod in ui
  //----------------------------------------------------------------------------------------------------------------------
  void toggleTreeGenMethod(int _methodNum);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slots to determine how many trees of each type will be generated in the forest random scattering method
  /// @param[in] _numTrees, the int passed from m_numTrees_i in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setNumTrees1(int _numTrees);
  void setNumTrees2(int _numTrees);
  void setNumTrees3(int _numTrees);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slot to set the number of hero trees that each L-System will use to fill up its instance cache
  /// @param[in] numHeroTrees, the int passed from m_numHeroTrees in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setNumHeroTrees(int _numHeroTrees);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slot to set the instancing probability for all L-Systems
  /// @param[in] instancingProb, the double passed from m_instancingProb in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setInstancingProb(double _instancingProb);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slot to set the seed for both forest members
  /// @param[in] _seed, the int passed from m_seed in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setSeedForest(int _seed);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slot to toggle whether or not we'll use a seed for the forests (or seed by time)
  /// @param[in] _mode, the int passed from m_seedToggleForest in ui
  //----------------------------------------------------------------------------------------------------------------------
  void seedToggleForest(int _mode);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slot to perform random scattering and instancing again to reset m_forest
  //----------------------------------------------------------------------------------------------------------------------
  void remakeForest();

protected:

  //GENERAL MEMBER VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of tabs of each type
  //----------------------------------------------------------------------------------------------------------------------
  const size_t m_numSuperTabs = 2;
  const size_t m_numTreeTabs = 3;
  const size_t m_numForestTabs = 2;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the current tab numbers
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_superTabNum = 0;
  size_t m_treeTabNum = 0;
  size_t m_forestTabNum = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief matrices to set up projection and camera view
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
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the grid representing the ground plane for the L-Systems
  //----------------------------------------------------------------------------------------------------------------------
  Grid m_grid = Grid(7,5);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to decide whether we display the grid
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<bool> m_showGrid;

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


  //TERRAIN VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief TerrainGenerator object that uses perlin noise to generate a heightmap for the forest terrain
  //----------------------------------------------------------------------------------------------------------------------
  TerrainGenerator m_terrainGen;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief TerrainData object which applies LOD algorithms to the heightmap values from m_terrainGen and creates
  /// a vertex list to be passed into paintGL
  //----------------------------------------------------------------------------------------------------------------------
  TerrainData m_terrain;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief worldspace width of the terrain
  //----------------------------------------------------------------------------------------------------------------------
  float m_width = 2000;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief initial dimensions of the heightmap grid for m_terrain - note that this has to be 2^n+1 for
  /// some integer n, due to constraints from the LOD algorithm in m_terrain
  //----------------------------------------------------------------------------------------------------------------------
  int m_terrainDimension = 129;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief float to store the user-specified error tolerance passed into the LOD algorithm in m_terrain
  //----------------------------------------------------------------------------------------------------------------------
  float m_tolerance = 0.02f;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief bool determining whether or not to draw the terrain in wireframe
  //----------------------------------------------------------------------------------------------------------------------
  bool m_terrainWireframe = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief module storing the perlin noise used to generate the terrain
  //----------------------------------------------------------------------------------------------------------------------
  noise::module::Perlin m_perlinModule;


  //GENERAL FOREST VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the Forest object we use when generating forests with the tree scattering method
  //----------------------------------------------------------------------------------------------------------------------
  Forest m_scatteredForest;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the Forest object we use when generating forests with the tree painting method
  //----------------------------------------------------------------------------------------------------------------------
  Forest m_paintedForest;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine which forest type we're rendering with
  //----------------------------------------------------------------------------------------------------------------------
  bool m_usePaintedForest = true;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief number of hero trees to create for each LSystem when filling the instance cache
  //----------------------------------------------------------------------------------------------------------------------
  int m_numHeroTrees = 10;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief number of trees to use of each LSystem type in tree scattering
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<size_t> m_numTrees = {1000,1000,1000};
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief seed to be used for stochastic behaviour in choosing instances and scattering trees for the forests
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_forestSeed = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine whether the forests use m_forestSeed or are seeded by time
  //----------------------------------------------------------------------------------------------------------------------
  bool m_forestUseSeed = false;

  //FOREST PAINT VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the index of the current L-System we're using to paint trees
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_paintBrushNum = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine whether or not left mouse button events should currently correspond to painting trees
  //----------------------------------------------------------------------------------------------------------------------
  bool m_treePaintMode = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to save whether or not paint mode was on when we were last on the painting trees tab
  //----------------------------------------------------------------------------------------------------------------------
  bool m_savedTreePaintMode = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to save whether or not the mouse is currently in the middle of painting trees
  //----------------------------------------------------------------------------------------------------------------------
  bool m_drawingLine = false;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the spacing between painted trees
  //----------------------------------------------------------------------------------------------------------------------
  float m_minTreeDist = 20;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the error tolerance when picking a point on the terrain with the mouse
  //----------------------------------------------------------------------------------------------------------------------
  float m_rayPickTolerance = 0.1f;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief list of vertices corresponding to points where the user has painted trees onto the terrain
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_paintPoints = {};
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief corresponding index list to allow us to draw these points with GL_POINTS
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<GLshort> m_paintPointIndices = {};
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief list of vertices to describe the temporary lines painted onto the terrain as we paint trees
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_paintLineVertices = {};
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief corresponding index list to allow us to draw these lines with GL_LINES
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<GLshort> m_paintLineIndices = {};


  //VAOS AND RELATED VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief grid VAO
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_gridVAO;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief VAO for the painted line
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_paintLineVAO;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief terrain VAO
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_terrainVAO;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief VAOs for the LSystems
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<std::unique_ptr<ngl::AbstractVAO>> m_treeVAOs;
  std::vector<std::unique_ptr<ngl::AbstractVAO>> m_leafVAOs;
  std::vector<std::unique_ptr<ngl::AbstractVAO>> m_polygonVAOs;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief VAOs for the Forests, stored as nested vectors corresponding to instance caches:
  /// layers separate by: treetype / id / age / different instances of a given id and index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_forestVAOs;
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_forestLeafVAOs;
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_forestPolygonVAOs;
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_paintedForestVAOs;
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_paintedForestLeafVAOs;
  std::vector<CACHE_STRUCTURE(std::unique_ptr<ngl::AbstractVAO>)> m_paintedForestPolygonVAOs;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief bools to tell paintGL whether or not we need to rebuild a given VAO
  //----------------------------------------------------------------------------------------------------------------------
  bool m_buildTreeVAO = false;
  bool m_buildForestVAOs = false;
  bool m_buildPaintLineVAO = true;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief variables storing the buffer ids for the buffers used by each VAO
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_terrainNormalBuffer = 0;
  GLuint m_terrainTangentBuffer = 0;
  GLuint m_terrainBitangentBuffer = 0;
  GLuint m_terrainUVBuffer = 0;
  std::vector<GLuint> m_treeRightBuffers = {0,0,0};
  std::vector<GLuint> m_treeThicknessBuffers = {0,0,0};
  std::vector<GLuint> m_treeLeafDirectionBuffers = {0,0,0};
  std::vector<GLuint> m_treeLeafRightBuffers = {0,0,0};
  std::vector<CACHE_STRUCTURE(GLuint)> m_forestRightBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_forestThicknessBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_forestLeafDirectionBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_forestLeafRightBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_paintedForestRightBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_paintedForestThicknessBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_paintedForestLeafDirectionBuffers = {{},{},{}};
  std::vector<CACHE_STRUCTURE(GLuint)> m_paintedForestLeafRightBuffers = {{},{},{}};

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief variables storing the texture ids for each texture object
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_terrainTexId = 0;
  GLuint m_terrainNormalId = 0;
  GLuint m_treeTexId = 0;
  GLuint m_treeNormalId = 0;
  GLuint m_forestTreeTexId = 0;
  GLuint m_forestTreeNormalId = 0;
  GLuint m_leafTexId = 0;



  //GENERAL MEMBER FUNCTIONS
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
  /// @brief updates forest class when L-Systems change
  //----------------------------------------------------------------------------------------------------------------------
  void updateScatteredForest();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief apply LOD algorithm to terrain and rebuild m_terrainVAO accordingly
  //----------------------------------------------------------------------------------------------------------------------
  void refineTerrain();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief calls bind, then draw, thenunbind on the given VAO, just created to save repetition
  //----------------------------------------------------------------------------------------------------------------------
  void drawVAO(std::unique_ptr<ngl::AbstractVAO> &_VAO);


  //VAO BUILDING METHODS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build a VAO using the Jon's SimpleIndexVAO class by binding vertex and index data
  /// @param [in] vao, the vao to bind
  /// @param [in] vertices, list of vertices to be rendered
  /// @param [in] indices, list of indexes corresponding to the vertices
  /// @param [in] mode, the openGL drawing mode
  /// @param [in[ indexType, the data type of the indexes
  //----------------------------------------------------------------------------------------------------------------------
  template <class dataType>
  void buildSimpleIndexVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                           std::vector<dataType> &_indices, GLenum _mode, GLenum _indexType);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build a VAO using my InstanceCacheVAO class by binding vertex and index data
  /// @param [in] vao, the vao to bind
  /// @param [in] vertices, list of vertices to use for rendering
  /// @param [in] indices, list of indexes corresponding to the vertices
  /// @param [in] transforms, list of transforms for each instance
  /// @param [in] instanceStart and end, the start and end points of the supplied index list for this instance
  /// @param [in] mode, the openGL drawing mode
  //----------------------------------------------------------------------------------------------------------------------
  void buildInstanceCacheVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                             std::vector<GLshort> &_indices, std::vector<ngl::Mat4> &_transforms,
                             size_t _instanceStart, size_t _instanceEnd, GLenum _mode);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method used to bind more data to supplement the data sent to a VAO by the above two methods
  /// (note this method requires the vao to be bound before calling)
  /// @param [in] bufferSize, the size of the buffer to be used
  /// @param [in] bufferData, a pointer the data for the buffer
  /// @param [in] bufferId, the variable used to store the bufferId
  //----------------------------------------------------------------------------------------------------------------------
  void addBufferToBoundVAO(size_t _bufferSize, const GLvoid * _bufferData, GLuint &_bufferID);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build m_gridVAO to store data for rendering the grid
  //----------------------------------------------------------------------------------------------------------------------
  void buildGridVAO();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build m_terrainVAO to store data for rendering the terrain
  //----------------------------------------------------------------------------------------------------------------------
  void buildTerrainVAO();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build tree, leaf or polygon VAO to store data for rendering an LSystem
  /// @param [in] _treeNum, the index of the LSystem VAO to build
  //----------------------------------------------------------------------------------------------------------------------
  void buildTreeVAO(size_t _treeNum);
  void buildLeafVAO(size_t _treeNum);
  void buildPolygonVAO(size_t _treeNum);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build tree, leaf or polygon forest VAO to store data for rendering LSystem instances in forests
  /// @param [in] treeNum, id, age, index: the identifiers for the position of the VAO in the VAO cache structure
  /// @param [in] _usePaintedForest, a bool to determine whether we use VAOs corresponding to m_paintedForest
  /// or corresponding to m_scatteredForest
  //----------------------------------------------------------------------------------------------------------------------
  void buildForestVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest);
  void buildForestLeafVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest);
  void buildForestPolygonVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build all VAOs used for rendering m_scatteredForest
  //----------------------------------------------------------------------------------------------------------------------
  void buildScatteredForestVAOs();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief build all VAOs used for rendering m_paintedForest
  //----------------------------------------------------------------------------------------------------------------------
  void buildPaintedForestVAOs();


  //SHADER METHODS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief load and compile all shaders
  //----------------------------------------------------------------------------------------------------------------------
  void compileShaders();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief load all textures
  //----------------------------------------------------------------------------------------------------------------------
  void loadShaderTextures();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief bind texture to texture unit and load to a uniform in a shader
  /// @note my understanding of this part of the openGL pipeline is a bit off, consequently this function doesn't do
  /// quite what I expected it to and some textures have not been successfully loaded
  /// @param [in] shaderName, the name of the shader program
  /// @param [in] textureMapName, the name of the uniform sampler2D variable in the shader
  /// @param [in] textureMapFile, the path to the texture to be loaded
  /// @param [in] textureUnit, the number representing the texture unit to bind this texture to
  //----------------------------------------------------------------------------------------------------------------------
  void loadTextureToShader(const std::string &_shaderName,const char * _textureMapName,
                           const char *_textureMapFile, GLuint _textureUnit);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief load all unifrom variables (apart from textures) to a shader
  /// @param [in] shader, an instance of the ngl shader library
  /// @param [in] shaderName, the name of the shader program
  //----------------------------------------------------------------------------------------------------------------------
  void loadUniformsToShader(ngl::ShaderLib *_shader, const std::string &_shaderName);


  //L-SYSTEMS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set up the initial L-Systems for each treeTab screen
  //----------------------------------------------------------------------------------------------------------------------
  void initializeLSystems();


  //EVENTS
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
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

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create a projected ray from the mouse cursor to determine where the point on the terrain that the mouse
  /// is pointing to (if such a point exists)
  /// @param [in] screenX, screenY, the screen x and y coordinates of the mouse cursor
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 getProjectedPointOnTerrain(float _screenX, float _screenY);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief add a given point to m_paintedPoints and use to create a new tree in m_paintedForest if the point lies
  /// on the terrain
  /// @param [in] point, the world space coordinates of the point to be added
  //----------------------------------------------------------------------------------------------------------------------
  void addPointToPaintedForest(ngl::Vec3 &_point);

};



#endif //NGLSCENE_H_
