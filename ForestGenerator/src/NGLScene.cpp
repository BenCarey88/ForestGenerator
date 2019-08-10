//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.cpp
/// @brief implementation file for NGLScene class
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>

#include <ngl/NGLInit.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>
#include <ngl/SimpleVAO.h>
#include <ngl/Texture.h>

#include "InstanceCacheVAO.h"
#include "NGLScene.h"
#include "Camera.h"
#include "PrintFunctions.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

//------------------------------------------------------------------------------------------------------------------------
///CONSTRUCTOR AND DESTRUCTOR
//------------------------------------------------------------------------------------------------------------------------

NGLScene::NGLScene(QWidget *_parent) : QOpenGLWidget( _parent )
{
  // set this widget to have the initial keyboard focus
  setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());

  m_cameras.resize(m_numSuperTabs);
  m_cameras[0].resize(m_numTreeTabs);
  m_cameras[1].resize(m_numTerrainTabs);
  m_cameras[2].resize(1);

  m_cameras[1][0]=Camera({0,200,1000},{0,0,0});
  m_cameras[1][1]=Camera({0,0,2500},{0,0,0});
  m_cameras[2][0]=m_cameras[1][0];

  m_mouseTransforms.resize(m_numSuperTabs);
  m_mouseTransforms[0].resize(m_numTreeTabs);
  m_mouseTransforms[1].resize(m_numTerrainTabs);
  m_mouseTransforms[2].resize(1);

  initializeLSystems();
  m_terrainGen = TerrainGenerator(m_terrainDimension, m_width);
  m_forest = Forest(m_LSystems, m_width,
                    m_numTrees, m_numHeroTrees, m_terrainGen);
  m_forestVAOs.resize(m_numTreeTabs);
  m_terrainGen.generate();
  m_terrain = TerrainData(m_terrainGen);

  m_currentCamera = &m_cameras[0][0];
  m_currentMouseTransform = &m_mouseTransforms[0][0];
  m_currentLSystem = &m_LSystems[0];

  m_currentLSystem->createGeometry();
  m_showGrid.resize(m_numTreeTabs, true);
}

NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  m_gridVAO->removeVAO();
  m_terrainVAO->removeVAO();
  for(size_t i=0; i<m_numTreeTabs; i++)
  {
    m_treeVAOs[i]->removeVAO();
    m_leafVAOs[i]->removeVAO();
    m_polygonVAOs[i]->removeVAO();
  }
  for(size_t t=0; t<m_numTreeTabs; t++)
  {
    FOR_EACH_ELEMENT(m_forestVAOs[t],
                     m_forestVAOs[t][ID][AGE][INDEX]->removeVAO();
                     m_forestLeafVAOs[t][ID][AGE][INDEX]->removeVAO();
                     m_forestPolygonVAOs[t][ID][AGE][INDEX]->removeVAO())
  }
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective(fieldOfView, static_cast<float>( _w ) / _h, nearFrame, farFrame );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}

void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background

  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic camera from the graphics library using the currently selected camera
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planesm_vao->bind();
  m_project=ngl::perspective(fieldOfView ,720.0f/576.0f,nearFrame,farFrame);

  // now to load the shaders
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("SkeletalTreeShader", "shaders/SkeletalTreeVertex.glsl",
                     "shaders/SkeletalTreeFragment.glsl");
  shader->loadShader("TreeShader", "shaders/TreeVertex.glsl",
                     "shaders/TreeFragment.glsl", "shaders/TreeGeometry.glsl");
  shader->loadShader("LeafShader", "shaders/LeafVertex.glsl",
                     "shaders/LeafFragment.glsl", "shaders/LeafGeometry.glsl");
  shader->loadShader("PolygonShader", "shaders/PolygonVertex.glsl",
                     "shaders/PolygonFragment.glsl");

  shader->loadShader("SkeletalForestShader", "shaders/SkeletalForestVertex.glsl",
                     "shaders/SkeletalForestFragment.glsl");
  shader->loadShader("ForestShader", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl", "shaders/ForestGeometry.glsl");
  shader->loadShader("ForestLeafShader", "shaders/ForestLeafVertex.glsl",
                     "shaders/ForestLeafFragment.glsl", "shaders/ForestLeafGeometry.glsl");
  shader->loadShader("ForestPolygonShader", "shaders/ForestPolygonVertex.glsl",
                     "shaders/ForestPolygonFragment.glsl");

  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");
  shader->loadShader("TerrainShader", "shaders/TerrainVertex.glsl",
                     "shaders/TerrainFragment.glsl");

  //register the new VAO class created to take care of the instancing
  ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  ngl::VAOFactory::listCreators();

  //set up initial VAOs:
  for(size_t i=0; i<m_numTreeTabs; i++)
  {
    buildTreeVAO(i);
  }
  buildGridVAO();
  //And load intial textures
  loadTextures(shader, "TreeShader",
               "textures/American_oak_pxr128.jpg",
               "textures/American_oak_pxr128_normal.jpg");
}


//------------------------------------------------------------------------------------------------------------------------

void NGLScene::drawVAO(std::unique_ptr<ngl::AbstractVAO> &_VAO)
{
  _VAO->bind();
  _VAO->draw();
  _VAO->unbind();
}

void NGLScene::refineTerrain()
{
  //call meshRefine on terrain using the current eye coordinates
  ngl::Vec3 from = m_currentCamera->m_from;
  from = m_initialRotation.inverse() * m_currentMouseTransform->inverse() * from;
  m_terrain.meshRefine(from, m_tolerance, 100.0);
  buildTerrainVAO();
}

//------------------------------------------------------------------------------------------------------------------------
/// PAINT_GL
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);

  if(m_buildTreeVAO==true)
  {
    buildTreeVAO(m_treeTabNum);
    m_buildTreeVAO = false;
  }
  if(m_buildForestVAOs==true)
  {
    buildForestVAOs();
    m_buildForestVAOs = false;
  }

  switch(m_superTabNum)
  {
    case 0:
    {
      if(m_showGrid[m_treeTabNum]==true)
      {
        loadUniformsToShader(shader, "GridShader");
        drawVAO(m_gridVAO);
      }
      if(m_currentLSystem->m_skeletonMode==true)
      {
        loadUniformsToShader(shader, "SkeletalTreeShader");
      }
      else
      {
        loadUniformsToShader(shader, "TreeShader");
        loadTextures(shader, "TreeShader",
                     "textures/American_oak_pxr128.jpg",
                     "textures/American_oak_pxr128_normal.jpg");
      }
      drawVAO(m_treeVAOs[m_treeTabNum]);

      buildLeafVAO(m_treeTabNum);
      loadUniformsToShader(shader, "LeafShader");
      //NOTE: LOADING TEXTURES IS SLOW!
      //change this so we only load once at the beginning rather than everytime we draw
      loadTextures(shader, "LeafShader",
                   "textures/leaf2.jpg", "textures/leaf2.jpg");
      drawVAO(m_leafVAOs[m_treeTabNum]);

      buildPolygonVAO(m_treeTabNum);
      loadUniformsToShader(shader, "PolygonShader");
      drawVAO(m_polygonVAOs[m_treeTabNum]);
      break;
    }

    case 1:
    {
      if(m_terrainTabNum==0)
      {
        if(m_terrainWireframe == true)
        {
          glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }
        refineTerrain();
        loadUniformsToShader(shader, "TerrainShader");
        loadTextures(shader, "TerrainShader",
                     "textures/Lawn_grass_pxr128.jpg",
                     "textures/Lawn_grass_pxr128_normal.jpg");
        drawVAO(m_terrainVAO);
      }
      else if (m_terrainTabNum==1)
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        ngl::Mat4 currentTransform = (*m_currentMouseTransform)*m_initialRotation;
        ngl::Mat4 MVP= m_project*m_view*currentTransform*m_layoutRotation;
        Grid grid(25,80);
        buildSimpleIndexVAO(m_terrainVAO, grid.m_vertices,  grid.m_indices, GL_LINES, GL_UNSIGNED_SHORT);
        (*shader)["TerrainShader"]->use();
        shader->setUniform("MVP",MVP);
        drawVAO(m_terrainVAO);
      }

      for(GLshort i=0; size_t(i)<m_points.size(); i++)
      {
        m_pointIndices.push_back(i);
      }
      buildSimpleIndexVAO(m_terrainVAO, m_points, m_pointIndices, GL_POINTS, GL_UNSIGNED_SHORT);
      glPointSize(20);
      drawVAO(m_terrainVAO);

      break;
    }

    case 2:
    {
      if(m_terrainWireframe == true)
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      }
      refineTerrain();
      loadUniformsToShader(shader, "TerrainShader");
      loadTextures(shader, "TerrainShader",
                   "textures/Lawn_grass_pxr128.jpg",
                   "textures/Lawn_grass_pxr128_normal.jpg");
      drawVAO(m_terrainVAO);

      loadUniformsToShader(shader, "ForestShader");
      loadTextures(shader, "ForestShader",
                   "textures/American_oak_pxr128.jpg",
                   "textures/American_oak_pxr128_normal.jpg");
      for(size_t t=0; t<m_numTreeTabs; t++)
      {
        FOR_EACH_ELEMENT(m_forestVAOs[t],

                         loadUniformsToShader(shader, "ForestShader");
                         loadTextures(shader, "ForestShader",
                                      "textures/American_oak_pxr128.jpg",
                                      "textures/American_oak_pxr128_normal.jpg");
                         drawVAO(m_forestVAOs[t][ID][AGE][INDEX]);

                         loadUniformsToShader(shader, "ForestLeafShader");
                         loadTextures(shader, "LeafShader",
                                      "textures/leaf2.jpg", "textures/leaf2.jpg");
                         drawVAO(m_forestLeafVAOs[t][ID][AGE][INDEX]);

                         loadUniformsToShader(shader, "ForestPolygonShader");
                         drawVAO(m_forestPolygonVAOs[t][ID][AGE][INDEX]))
      }
      break;
    }

    default:
      break;
  }
}
