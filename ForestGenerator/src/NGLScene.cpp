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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
  shader->loadShader("TreeShader", "shaders/TreeVertex.glsl",
                     "shaders/TreeFragment.glsl");
  shader->loadShader("TreeShader_Geom", "shaders/TreeVertex.glsl",
                     "shaders/TreeFragment.glsl", "shaders/TreeGeometry.glsl");
  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");
  shader->loadShader("ForestShader", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl");
  shader->loadShader("ForestShader_Geom", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl", "shaders/ForestGeometry.glsl");
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
  loadTextures(shader, "TreeShader_Geom",
               "textures/American_oak_pxr128.jpg",
               "textures/American_oak_pxr128_normal.jpg");
}


//------------------------------------------------------------------------------------------------------------------------

void NGLScene::loadTextures(ngl::ShaderLib *_shader, const std::string &_shaderName,
                            const char *_textureMapFile, const char *_normalMapFile)
{
  ///@ref https://stackoverflow.com/questions/25252512/how-can-i-pass-multiple-textures-to-a-single-shader

  (*_shader)[_shaderName]->use();

  GLint textureMapLocation = glGetUniformLocation((*_shader)[_shaderName]->getID(), "textureMap");
  GLint normalMapLocation  = glGetUniformLocation((*_shader)[_shaderName]->getID(), "normalMap");
  glUniform1i(textureMapLocation, 0);
  glUniform1i(normalMapLocation,  1);

  // Then bind the uniform samplers to texture units:
  GLuint texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(_textureMapFile, &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load texture for "<<_shaderName<<"\n";
  }
  stbi_image_free(data);

  unsigned int normalMap;
  glGenTextures(1, &normalMap);
  glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
  glBindTexture(GL_TEXTURE_2D, normalMap);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  data = stbi_load(_normalMapFile, &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load normal map for "<<_shaderName<<"\n";
  }
  stbi_image_free(data);
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

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);

  ngl::Mat4 currentTransform = (*m_currentMouseTransform)*m_initialRotation;
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MV = m_view*currentTransform;
  ngl::Mat4 M = currentTransform;
  ngl::Mat3 normalMatrix= MV.inverse().transpose();
  ngl::Mat3 newNormalMatrix= m_view.inverse().transpose();
  ngl::Vec3 lightPos = (currentTransform * ngl::Vec4(0,100,100,1)).toVec3();

  if(m_buildTreeVAO)
  {
    buildTreeVAO(m_treeTabNum);
    m_buildTreeVAO = false;
  }

  if(m_buildForestVAOs)
  {
    buildForestVAO();
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

      if(m_currentLSystem->m_skeletonMode==true)
      {
        drawVAO(m_treeVAOs[m_treeTabNum], shader, "TreeShader", MVP);
      }
      else
      {
        loadTextures(shader, "TreeShader_Geom",
                     "textures/American_oak_pxr128.jpg",
                     "textures/American_oak_pxr128_normal.jpg");

        (*shader)["TreeShader_Geom"]->use();
        shader->setUniform("MVP",MVP);
        shader->setUniform("normalMatrix",normalMatrix);
        shader->setUniform("MV",MV);
        shader->setUniform("lightPos",lightPos);

        drawVAO(m_treeVAOs[m_treeTabNum], shader, "TreeShader_Geom", MVP);
      }

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

        loadTextures(shader, "TerrainShader",
                     "textures/groundTexture.jpg",
                     "textures/Dirt_and_gravel_pxr128_normal.jpg");

        (*shader)["TerrainShader"]->use();
        shader->setUniform("MVP",MVP);
        shader->setUniform("normalMatrix",normalMatrix);
        shader->setUniform("newNormalMatrix",newNormalMatrix);
        shader->setUniform("MV",MV);
        shader->setUniform("M",M);
        shader->setUniform("lightPosition",lightPos);
        shader->setUniform("maxHeight",m_forest.m_terrainGen.m_amplitude);

        m_terrainVAO->bind();
        m_terrainVAO->draw();
        m_terrainVAO->unbind();
      }
      else if (m_terrainTabNum==1)
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        MVP = MVP*m_layoutRotation;
        Grid grid(25,80);
        buildSimpleIndexVAO(m_terrainVAO, grid.m_vertices,  grid.m_indices, GL_LINES, GL_UNSIGNED_SHORT);
        drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);
      }

      for(GLshort i=0; size_t(i)<m_points.size(); i++)
      {
        m_pointIndices.push_back(i);
      }
      buildSimpleIndexVAO(m_terrainVAO, m_points, m_pointIndices, GL_POINTS, GL_UNSIGNED_SHORT);
      glPointSize(20);
      drawVAO(m_terrainVAO, shader, "TerrainShader", MVP);

      break;
    }

    case 2:
    {
      if(m_terrainWireframe == true)
      {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      }
      refineTerrain();

      loadTextures(shader, "TerrainShader",
                   "textures/groundTexture.jpg",
                   "textures/Dirt_and_gravel_pxr128_normal.jpg");

      (*shader)["TerrainShader"]->use();
      shader->setUniform("MVP",MVP);
      shader->setUniform("normalMatrix",normalMatrix);
      shader->setUniform("newNormalMatrix",newNormalMatrix);
      shader->setUniform("MV",MV);
      shader->setUniform("M",M);
      shader->setUniform("lightPosition",lightPos);
      shader->setUniform("maxHeight",m_forest.m_terrainGen.m_amplitude);

      m_terrainVAO->bind();
      m_terrainVAO->draw();
      m_terrainVAO->unbind();

      (*shader)["ForestShader_Geom"]->use();
      shader->setUniform("MVP",MVP);
      shader->setUniform("normalMatrix",normalMatrix);
      shader->setUniform("MV",MV);
      shader->setUniform("lightPos",lightPos);
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
