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
  m_cameras[1].resize(m_numForestTabs);
  m_cameras[2].resize(1);

  m_cameras[1][0]=Camera({0,200,1000},{0,0,0});
  m_cameras[1][1]=m_cameras[1][0];
//  m_cameras[1][1]=Camera({0,0,2500},{0,0,0});
  m_cameras[2][0]=m_cameras[1][0];

  m_mouseTransforms.resize(m_numSuperTabs);
  m_mouseTransforms[0].resize(m_numTreeTabs, m_initialRotation);
  m_mouseTransforms[1].resize(m_numForestTabs, m_initialRotation);
  m_mouseTransforms[2].resize(1, m_initialRotation);

  initializeLSystems();
  m_terrainGen = TerrainGenerator(m_terrainDimension, m_width);
  m_forest = Forest(m_LSystems, m_width,
                    m_numTrees, m_numHeroTrees, m_terrainGen);
  m_paintedForest = Forest(m_LSystems, m_numHeroTrees);
  m_forestVAOs.resize(m_numTreeTabs);
  m_forestLeafVAOs.resize(m_numTreeTabs);
  m_forestPolygonVAOs.resize(m_numTreeTabs);
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
  m_paintLineVAO->removeVAO();
  m_pointVAO->removeVAO();
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

  //load shaders and textures
  compileShaders();
  loadShaderTextures();

  //register the new VAO class created to take care of the instancing
  ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  ngl::VAOFactory::listCreators();

  //set up initial VAOs:
  for(size_t i=0; i<m_numTreeTabs; i++)
  {
    buildTreeVAO(i);
    buildLeafVAO(i);
    buildPolygonVAO(i);
  }
  buildGridVAO();

  buildForestVAOs(m_paintedForest);
}


//------------------------------------------------------------------------------------------------------------------------

void NGLScene::drawVAO(std::unique_ptr<ngl::AbstractVAO> &_VAO)
{
  _VAO->bind();
//  print("bind ",glGetError(), "\n");
  _VAO->draw();
//  print("draw ",glGetError(), "\n");
  _VAO->unbind();
//  print("unbind ",glGetError(), "\n");
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

//  glPointSize(20);
//  if(m_buildPaintLineVAO)
//  {
//    buildSimpleIndexVAO(m_terrainVAO, m_paintLineVertices, m_paintLineIndices, GL_POINTS, GL_UNSIGNED_SHORT);
//    m_buildPaintLineVAO = false;
//  }
//  loadUniformsToShader(shader, "GridShader");
//  drawVAO(m_terrainVAO);

//  std::vector<ngl::Vec3> verts = {{-0.5,0.5,0},{-0.5,0.5,-0.5}};
//  std::vector<GLshort> inds = {0,1};
//  glPointSize(20);
//  buildSimpleIndexVAO(m_terrainVAO, verts, inds, GL_POINTS, GL_UNSIGNED_SHORT);
//  (*shader)["GridShader"]->use();
//  drawVAO(m_terrainVAO);

  if(m_buildTreeVAO==true)
  {
    buildTreeVAO(m_treeTabNum);
    buildLeafVAO(m_treeTabNum);
    buildPolygonVAO(m_treeTabNum);
    m_buildTreeVAO = false;
  }
  if(m_buildForestVAOs==true)
  {
    buildForestVAOs(m_forest);
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
      }
      drawVAO(m_treeVAOs[m_treeTabNum]);

      loadUniformsToShader(shader, "LeafShader");
      drawVAO(m_leafVAOs[m_treeTabNum]);

      loadUniformsToShader(shader, "PolygonShader");
      drawVAO(m_polygonVAOs[m_treeTabNum]);

      break;
    }

    case 1:
    {
      if (m_forestTabNum==0)
      {
//        print("GRID ERROR? ", glGetError(), "\n");
        loadUniformsToShader(shader, "GridShader");
//        print("GRID ERROR? ", glGetError(), "\n");
        //buildGridVAO();
//        print("GRID ERROR? ", glGetError(), "\n");
        drawVAO(m_gridVAO);
//        print("GRID ERROR? ", glGetError(), "\n");

        glPointSize(20);
        if(m_buildPaintLineVAO)
        {
          buildSimpleIndexVAO(m_paintLineVAO, m_paintLineVertices, m_paintLineIndices, GL_LINES, GL_UNSIGNED_SHORT);
          //buildSimpleIndexVAO(m_pointVAO, m_points, m_pointIndices, GL_POINTS, GL_UNSIGNED_SHORT);
          m_buildPaintLineVAO = false;
        }

        loadUniformsToShader(shader, "GridShader");
        drawVAO(m_paintLineVAO);
        //drawVAO(m_pointVAO);

        loadUniformsToShader(shader, "ForestShader");
        loadUniformsToShader(shader, "ForestLeafShader");
        loadUniformsToShader(shader, "ForestPolygonShader");

        for(auto &i : m_paintedForest.m_adjustedCacheIndexes)
        {
          buildForestVAO(m_paintedForest, i.m_treeNum, i.m_id, i.m_age, i.m_innerIndex);
          buildForestLeafVAO(m_paintedForest, i.m_treeNum, i.m_id, i.m_age, i.m_innerIndex);
          buildForestPolygonVAO(m_paintedForest, i.m_treeNum, i.m_id, i.m_age, i.m_innerIndex);
          m_paintedForest.m_adjustedCacheIndexes.clear();
        }
        //buildForestVAOs(m_paintedForest);

        FOR_EACH_ELEMENT(m_forestVAOs[0],
                         (*shader)["ForestShader"]->use();
//                         print("tree shader ",glGetError(), "\n");
                         drawVAO(m_forestVAOs[0][ID][AGE][INDEX]);
//                         print("draw trees ",glGetError(), "\n");
                         (*shader)["ForestLeafShader"]->use();
//                         print("leaf shader ",glGetError(), "\n");
                         drawVAO(m_forestLeafVAOs[0][ID][AGE][INDEX]);
//                         print("draw leaves ",glGetError(), "\n");
                         (*shader)["ForestPolygonShader"]->use();
//                         print("Polygon shader ",glGetError(), "\n");
                         drawVAO(m_forestPolygonVAOs[0][ID][AGE][INDEX]))//;
//                         print("draw polygons ",glGetError(), "\n");
//                         newLine())
      }

      else if(m_forestTabNum==1)
      {
        if(m_terrainWireframe == true)
        {
          glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }
        refineTerrain();
        loadUniformsToShader(shader, "TerrainShader");
        drawVAO(m_terrainVAO);
      }

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
      drawVAO(m_terrainVAO);

      loadUniformsToShader(shader, "ForestShader");
      loadUniformsToShader(shader, "ForestLeafShader");
      loadUniformsToShader(shader, "ForestPolygonShader");
      for(size_t t=0; t<m_numTreeTabs; t++)
      {
        FOR_EACH_ELEMENT(m_forestVAOs[t],
                         (*shader)["ForestShader"]->use();
                         drawVAO(m_forestVAOs[t][ID][AGE][INDEX]);
                         (*shader)["ForestLeafShader"]->use();
                         drawVAO(m_forestLeafVAOs[t][ID][AGE][INDEX]);
                         (*shader)["ForestPolygonShader"]->use();
                         print("FOREST ERROR? ", glGetError(), "\n");
                         drawVAO(m_forestPolygonVAOs[t][ID][AGE][INDEX]))
      }
      break;
    }

    default:
      break;
  }
}
