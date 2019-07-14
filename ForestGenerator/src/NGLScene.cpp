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
  // The final two are near and far clipping planes
  m_project=ngl::perspective(fieldOfView ,720.0f/576.0f,nearFrame,farFrame);

  // now to load the shaders
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->loadShader("ColourShader", "shaders/ColourVertex.glsl",
                     "shaders/ColourFragment.glsl");
  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");

  ngl::VAOFactory::listCreators();
}


void NGLScene::buildLineVAO(std::vector<ngl::Vec3> &_vertices, std::vector<GLshort> &_indices)
{
  // create a vao using GL_LINES
  m_vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,GL_LINES);
  m_vao->bind();

  // set our data for the VAO
  m_vao->setData(ngl::SimpleIndexVAO::VertexData(
                                                  sizeof(ngl::Vec3)*_vertices.size(),
                                                  _vertices[0].m_x,
                                                  uint(_indices.size()),
                                                  &_indices[0],
                                                  GL_UNSIGNED_SHORT));
  // data is 12 bytes apart (=sizeof(Vec3))
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  m_vao->setNumIndices(_indices.size());
  m_vao->unbind();
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

  (*shader)["GridShader"]->use();
  shader->setUniform("MVP",MVP);
  buildLineVAO(m_grid.m_vertices, m_grid.m_indices);
  m_vao->bind();
  m_vao->draw();
  m_vao->unbind();

  (*shader)["ColourShader"]->use();

  switch(m_superTabNum)
  {
    case 0:
      shader->setUniform("MVP",MVP);
      buildLineVAO(m_currentLSystem->m_vertices, m_currentLSystem->m_indices);
      m_vao->bind();
      m_vao->draw();
      m_vao->unbind();
      break;

    case 1:
      break;

    case 2:
      for(size_t i=0; i<m_numTreeTabs; i++)
      {
        ngl::Mat4 trans(1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        16*int(i)-8,0,0,1);
        shader->setUniform("MVP",MVP*trans);
        buildLineVAO(m_LSystems[i].m_vertices, m_LSystems[i].m_indices);
        m_vao->bind();
        m_vao->draw();
        m_vao->unbind();
      }
      break;

    default:
      break;
  }
}
