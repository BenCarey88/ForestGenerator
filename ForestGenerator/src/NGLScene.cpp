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

#include "InstanceCacheVAO.h"
#include "NGLScene.h"
#include "Camera.h"
#include "PrintFunctions.h"

//------------------------------------------------------------------------------------------------------------------------
///CONSTRUCTORS AND DESTRUCTORS
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
  m_cameras[1][1]=Camera({0,0,2600},{0,0,0});
  m_cameras[2][0]=m_cameras[1][0];

  m_mouseTransforms.resize(m_numSuperTabs);
  m_mouseTransforms[0].resize(m_numTreeTabs);
  m_mouseTransforms[1].resize(m_numTerrainTabs);
  m_mouseTransforms[2].resize(1);

  initializeLSystems();
  m_forest = Forest(m_LSystems,
                    m_width, m_terrainDimension,
                    m_numTrees, m_numHeroTrees);
  m_forestVAOs.resize(m_numTreeTabs);
  m_forest.m_terrainGen.generate();
  m_terrain = TerrainData(m_forest.m_terrainGen);

  m_currentCamera = &m_cameras[0][0];
  m_currentMouseTransform = &m_mouseTransforms[0][0];
  m_currentLSystem = &m_LSystems[0];

  m_currentLSystem->createGeometry();
  m_showGrid.resize(m_numTreeTabs, true);

  print("(",m_win.width,", ",m_win.height,")\n");
}

NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  m_gridVAO->removeVAO();
  m_terrainVAO->removeVAO();
  for(auto &LSystemVAO : m_treeVAOs)
  {
    LSystemVAO->removeVAO();
  }
  for(size_t t=0; t<m_numTreeTabs; t++)
  {
    FOR_EACH_ELEMENT(m_forestVAOs[t],
                     m_forestVAOs[t][ID][AGE][INDEX]->removeVAO())
  }
}


//------------------------------------------------------------------------------------------------------------------------
///OTHER PUBLIC METHODS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective(fieldOfView, static_cast<float>( _w ) / _h, nearFrame, farFrame );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );

  print("(",m_win.width,", ",m_win.height,")\n");
}

//------------------------------------------------------------------------------------------------------------------------

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
  shader->loadShader("TreeShader", "shaders/TreeVertex.glsl",
                     "shaders/TreeFragment.glsl");
  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");
  shader->loadShader("ForestShader", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl");
  shader->loadShader("TerrainShader", "shaders/TerrainVertex.glsl",
                     "shaders/TerrainFragment.glsl");

  //register the new VAO class created to take care of the instancing
  ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  ngl::VAOFactory::listCreators();

  //set up LSystem VAOs:
  for(size_t i=0; i<m_numTreeTabs; i++)
  {
    buildVAO(m_treeVAOs[i], m_LSystems[i].m_vertices, m_LSystems[i].m_indices, GL_LINES, GL_UNSIGNED_SHORT);
  }
  buildVAO(m_gridVAO, m_grid.m_vertices, m_grid.m_indices, GL_LINES, GL_UNSIGNED_SHORT);
}

//------------------------------------------------------------------------------------------------------------------------

template<class dataType>
void NGLScene::buildVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                        std::vector<dataType> &_indices, GLenum _mode, GLenum _indexType)
{
  // create a vao using _mode
  _vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,_mode);
  _vao->bind();

  // set our data for the VAO
  _vao->setData(ngl::SimpleIndexVAO::VertexData(
                                                  sizeof(ngl::Vec3)*_vertices.size(),
                                                  _vertices[0].m_x,
                                                  uint(_indices.size()),
                                                  &_indices[0],
                                                  _indexType));
  // data is 12 bytes apart (=sizeof(Vec3))
  _vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  _vao->setNumIndices(_indices.size());
  _vao->unbind();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildInstanceCacheVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, LSystem &_treeType,
                                     Instance &_instance, std::vector<ngl::Mat4> &_transforms)
{
  // create a vao using GL_LINES
  _vao=ngl::VAOFactory::createVAO("instanceCacheVAO",GL_LINES);
  _vao->bind();
  // set our data for the VAO:
  //    (1) vertexBufferSize, (2) vertexBufferStart,
  //    (3) indexBufferSize, (4) indexBufferStart,
  //    (5) transformBufferSize, (6) transformBufferStart
  _vao->setData(ngl::InstanceCacheVAO::VertexData(
                       sizeof(ngl::Vec3)*_treeType.m_heroVertices.size(),
                       _treeType.m_heroVertices[0].m_x,
                       uint(_instance.m_instanceEnd-_instance.m_instanceStart),
                       &_treeType.m_heroIndices[_instance.m_instanceStart],
                       uint(_transforms.size()),
                       &_transforms[0]));
  // set number of indices to length of current instance
  _vao->setNumIndices(_instance.m_instanceEnd-_instance.m_instanceStart);
  _vao->unbind();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::drawVAO(std::unique_ptr<ngl::AbstractVAO> &_VAO,
                       ngl::ShaderLib *_shader, const std::string &_shaderName, ngl::Mat4 &_MVP)
{
  (*_shader)[_shaderName]->use();
  _shader->setUniform("MVP",_MVP);
  _VAO->bind();
  _VAO->draw();
  _VAO->unbind();
}

void NGLScene::refineTerrain()
{
  ngl::Vec3 from = m_currentCamera->m_from;
  from = m_initialRotation.inverse()*m_currentMouseTransform->inverse()*from;
  m_terrain.meshRefine(from, m_tolerance, 100.0);
  buildVAO(m_terrainVAO,
           m_terrain.m_vertsToBeRendered,
           m_terrain.m_indicesToBeRendered,
           GL_TRIANGLE_STRIP, GL_UNSIGNED_INT);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  if (m_wireframe == true)
  {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  else
  {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  ngl::Mat4 MVP= m_project*m_view*(*m_currentMouseTransform)*m_initialRotation;

  if(m_buildTreeVAO)
  {
    buildVAO(m_treeVAOs[m_treeTabNum],
             m_currentLSystem->m_vertices,
             m_currentLSystem->m_indices,
             GL_LINES, GL_UNSIGNED_SHORT);
    m_buildTreeVAO = false;
  }

  if(m_buildForestVAOs)
  {
    for(size_t t=0; t<m_forest.m_treeTypes.size(); t++)
    {
      LSystem &treeType = m_forest.m_treeTypes[t];
      CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;
      RESIZE_CACHE_BY_OTHER_CACHE(m_forestVAOs[t], instanceCache)
      FOR_EACH_ELEMENT(m_forestVAOs[t],
                       buildInstanceCacheVAO(m_forestVAOs[t][ID][AGE][INDEX],
                                             treeType,
                                             instanceCache[ID][AGE][INDEX],
                                             m_forest.m_transformCache[t][ID][AGE][INDEX]))
    }
    m_buildForestVAOs = false;
  }

  switch(m_superTabNum)
  {
    case 0:
    {
      if(m_showGrid[m_treeTabNum]==true)
      {
        drawVAO(m_gridVAO, shader, "GridShader", MVP);
      }
      drawVAO(m_treeVAOs[m_treeTabNum], shader, "TreeShader", MVP);
      break;
    }

    case 1:
    {
      if(m_terrainTabNum==0)
      {
        refineTerrain();
        drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);
      }
      else
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        MVP = MVP*m_layoutRotation;
        //drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);
        Grid grid(25,80);
        buildVAO(m_terrainVAO, grid.m_vertices,  grid.m_indices, GL_LINES, GL_UNSIGNED_SHORT);
        drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);
      }

      for(GLshort i=0; i<m_points.size(); i++)
      {
        m_pointIndices.push_back(i);
      }
      buildVAO(m_terrainVAO, m_points, m_pointIndices, GL_POINTS, GL_UNSIGNED_SHORT);
      glPointSize(20);
      drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);

      break;
    }

    case 2:
    {
      refineTerrain();
      drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);

      (*shader)["ForestShader"]->use();
      shader->setUniform("MVP",MVP);
      for(size_t t=0; t<m_numTreeTabs; t++)
      {
        FOR_EACH_ELEMENT(m_forestVAOs[t],
                          m_forestVAOs[t][ID][AGE][INDEX]->bind();
                          m_forestVAOs[t][ID][AGE][INDEX]->draw();
                          m_forestVAOs[t][ID][AGE][INDEX]->unbind())
      }
      break;
    }

    default:
      break;
  }
}
