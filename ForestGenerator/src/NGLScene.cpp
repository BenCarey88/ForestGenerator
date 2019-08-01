//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.cpp
/// @brief implementation file for NGLScene class
//----------------------------------------------------------------------------------------------------------------------

#include <QMouseEvent>
#include <QGuiApplication>

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>

#include "InstanceCacheVAO.h"
#include "NGLScene.h"
#include "Camera.h"

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
  m_cameras[1].resize(1);
  m_cameras[2].resize(1);

  m_mouseTransforms.resize(m_numSuperTabs);
  m_mouseTransforms[0].resize(m_numTreeTabs);
  m_mouseTransforms[1].resize(1);
  m_mouseTransforms[2].resize(1);

  initializeLSystems();
  m_instanceCacheVAOs.resize(m_numTreeTabs);

  m_currentCamera = &m_cameras[0][0];
  m_currentMouseTransform = &m_mouseTransforms[0][0];
  m_currentLSystem = &m_LSystems[0];

  m_currentLSystem->createGeometry();
}

NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  m_vao->removeVAO();
}


//------------------------------------------------------------------------------------------------------------------------
///OTHER PUBLIC METHODS
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
  shader->loadShader("ColourShader", "shaders/ColourVertex.glsl",
                     "shaders/ColourFragment.glsl");
  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");
  shader->loadShader("ForestShader", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl");

  ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  ngl::VAOFactory::listCreators();

  //set up LSystem VAOs:
  for(size_t i=0; i<m_numTreeTabs; i++)
  {
    buildLineVAO(m_LSystems[i].m_vertices, m_LSystems[i].m_indices, m_LSystemVAOs[i]);
  }
}


void NGLScene::buildLineVAO(std::vector<ngl::Vec3> &_vertices, std::vector<GLshort> &_indices,
                            std::unique_ptr<ngl::AbstractVAO> &_vao)
{
  // create a vao using GL_LINES
  //ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  _vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,GL_LINES);
  _vao->bind();

  // set our data for the VAO
  _vao->setData(ngl::SimpleIndexVAO::VertexData(
                                                  sizeof(ngl::Vec3)*_vertices.size(),
                                                  _vertices[0].m_x,
                                                  uint(_indices.size()),
                                                  &_indices[0],
                                                  GL_UNSIGNED_SHORT));
  // data is 12 bytes apart (=sizeof(Vec3))
  _vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  _vao->setNumIndices(_indices.size());
  _vao->unbind();
}

void NGLScene::buildInstanceCacheVAO(LSystem &_treeType, Instance &_instance, std::unique_ptr<ngl::AbstractVAO> &_vao)
{
  // create a vao using GL_LINES
  _vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,GL_LINES);
  _vao->bind();

  // set our data for the VAO
  _vao->setData(ngl::SimpleIndexVAO::VertexData(
                                                  sizeof(ngl::Vec3)*_treeType.m_heroVertices.size(),
                                                  _treeType.m_heroVertices[0].m_x,
                                                  uint(_instance.m_instanceEnd-_instance.m_instanceStart),
                                                  &_treeType.m_heroIndices[_instance.m_instanceStart],
                                                  GL_UNSIGNED_SHORT));
  // data is 12 bytes apart (=sizeof(Vec3))
  _vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  _vao->setNumIndices(_instance.m_instanceEnd-_instance.m_instanceStart);
  _vao->unbind();
}

void NGLScene::buildTestVAO(LSystem &_treeType, Instance &_instance, std::unique_ptr<ngl::AbstractVAO> &_vao, size_t _instanceCount)
{
  // create a vao using GL_LINES
  //ngl::VAOFactory::registerVAOCreator("instanceCacheVAO",ngl::InstanceCacheVAO::create);
  _vao=ngl::VAOFactory::createVAO("instanceCacheVAO",GL_LINES);
  _vao->bind();

  // set our data for the VAO
  _vao->setData(ngl::InstanceCacheVAO::VertexData(
                       sizeof(ngl::Vec3)*_treeType.m_heroVertices.size(),
                       _treeType.m_heroVertices[0].m_x,
                       uint(_instance.m_instanceEnd-_instance.m_instanceStart),
                       &_treeType.m_heroIndices[_instance.m_instanceStart],
                       GL_UNSIGNED_SHORT,
                       int(_instanceCount)));
  // data is 12 bytes apart (=sizeof(Vec3))
  _vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  _vao->setNumIndices(_instance.m_instanceEnd-_instance.m_instanceStart);
  _vao->unbind();
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  m_view=ngl::lookAt(m_currentCamera->m_from, m_currentCamera->m_to, m_currentCamera->m_up);
  ngl::Mat4 MVP= m_project*m_view*(*m_currentMouseTransform)*m_initialRotation;

  if(m_showGrid)
  {
    (*shader)["GridShader"]->use();
    shader->setUniform("MVP",MVP);
    buildLineVAO(m_grid.m_vertices, m_grid.m_indices, m_vao);
    m_vao->bind();
    m_vao->draw();
    m_vao->unbind();
  }

  if(m_buildTreeVAO)
  {
      buildLineVAO(m_currentLSystem->m_vertices, m_currentLSystem->m_indices, m_LSystemVAOs[m_treeTabNum]);
      m_buildTreeVAO = false;
  }

  if(m_buildInstanceVAO)
  {
    m_instanceCacheVAOs.resize(m_numTreeTabs);
    m_bufferIds.resize(m_numTreeTabs);
    for(size_t t=0; t<m_forest.m_treeTypes.size(); t++)
    {
      LSystem &treeType = m_forest.m_treeTypes[t];
      std::vector<std::vector<std::vector<Instance>>> &instanceCache = treeType.m_instanceCache;
      m_instanceCacheVAOs[t].resize(instanceCache.size());
      m_bufferIds[t].resize(instanceCache.size());

      for(size_t id=0; id<instanceCache.size(); id++)
      {
        //std::cout<<"id = "<<id<<'\n';
        m_instanceCacheVAOs[t][id].resize(instanceCache[id].size());
        m_bufferIds[t][id].resize(instanceCache[id].size());
        for(size_t age=0; age<instanceCache[id].size(); age++)
        {
          m_instanceCacheVAOs[t][id][age].resize(instanceCache[id][age].size());
          m_bufferIds[t][id][age].resize(instanceCache[id][age].size());
          for(size_t index=0; index<instanceCache[id][age].size(); index++)
          {
            Instance &instance = instanceCache[id][age][index];
            buildTestVAO(treeType,
                         instance,
                         m_instanceCacheVAOs[t][id][age][index],
                         m_forest.m_outputCache[t][id][age][index].size());
          }
        }
      }
    }
    m_buildInstanceVAO = false;
  }

  (*shader)["ColourShader"]->use();

  switch(m_superTabNum)
  {
    case 0:
      shader->setUniform("MVP",MVP);
      m_LSystemVAOs[m_treeTabNum]->bind();
      m_LSystemVAOs[m_treeTabNum]->draw();
      m_LSystemVAOs[m_treeTabNum]->unbind();
      break;

    case 1:
      break;

    case 2:
    {
    /*for(size_t i=0; i<m_forest.m_numTrees; i++)
      {
        size_t type = m_forest.m_treeData[i].m_type;
        ngl::Mat4 trans = m_forest.m_treeData[i].m_transform;
        shader->setUniform("MVP",MVP*trans);

        m_LSystemVAOs[type]->bind();
        m_LSystemVAOs[type]->draw();
        m_LSystemVAOs[type]->unbind();
      }*/

      /*for(auto const &o : m_forest.m_output)
      {
        shader->setUniform("MVP",MVP*o.m_transform);
        m_instanceCacheVAOs[o.m_treeType][o.m_id][o.m_age][o.m_innerIndex]->bind();
        m_instanceCacheVAOs[o.m_treeType][o.m_id][o.m_age][o.m_innerIndex]->draw();
        m_instanceCacheVAOs[o.m_treeType][o.m_id][o.m_age][o.m_innerIndex]->unbind();
      }*/

      /*buildTestVAO();
      m_testVao->bind();
      m_testVao->draw();
      m_testVao->unbind();*/

      (*shader)["ForestShader"]->use();
      shader->setUniform("MVP",MVP);

      std::vector<std::vector<std::vector<std::vector<std::vector<ngl::Mat4>>>>> &outputCache = m_forest.m_outputCache;

      for(size_t t=0; t<outputCache.size(); t++)
      {
        for(size_t id=0; id<outputCache[t].size(); id++)
        {
          for(size_t age=0; age<outputCache[t][id].size(); age++)
          {
            for(size_t innerIndex=0; innerIndex<outputCache[t][id][age].size(); innerIndex++)
            {

              m_instanceCacheVAOs[t][id][age][innerIndex]->bind();

              std::vector<ngl::Mat4> &transforms = outputCache[t][id][age][innerIndex];
              GLuint &transformBuffer = m_bufferIds[t][id][age][innerIndex];


              glGenBuffers(1, &transformBuffer);
              glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);

              //glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.m_size), &data.m_data, data.m_mode);

              glBufferData(GL_ARRAY_BUFFER,
                           sizeof(ngl::Mat4)*static_cast<GLsizeiptr>(transforms.size()),
                           &transforms[0],
                           GL_STATIC_DRAW);

              glEnableVertexAttribArray(1);
              glEnableVertexAttribArray(2);
              glEnableVertexAttribArray(3);
              glEnableVertexAttribArray(4);

              glVertexAttribPointer(1,4,GL_FLOAT,false,64,
                                    static_cast<ngl::Real *>(NULL));
              glVertexAttribPointer(2,4,GL_FLOAT,false,64,
                                    static_cast<ngl::Real *>(NULL)+4);
              glVertexAttribPointer(3,4,GL_FLOAT,false,64,
                                    static_cast<ngl::Real *>(NULL)+8);
              glVertexAttribPointer(4,4,GL_FLOAT,false,64,
                                    static_cast<ngl::Real *>(NULL)+12);
              glVertexAttribDivisor(1,1);
              glVertexAttribDivisor(2,1);
              glVertexAttribDivisor(3,1);
              glVertexAttribDivisor(4,1);

              m_instanceCacheVAOs[t][id][age][innerIndex]->draw();
              m_instanceCacheVAOs[t][id][age][innerIndex]->unbind();
            }
          }
        }
      }
      break;
    }

    default:
      break;
  }
}
