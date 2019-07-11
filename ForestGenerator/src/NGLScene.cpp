//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.cpp
/// @brief implementation file for NGLScene class
//----------------------------------------------------------------------------------------------------------------------

#include <vector>

#include <QMouseEvent>
#include <QGuiApplication>

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>
#include <ngl/Random.h>

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

  m_LSystem.createGeometry(m_generation, ngl::Vec3(0,0,0));
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
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic camera from the graphics library
  m_view=ngl::lookAt(m_from, m_to, m_up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes
  m_project=ngl::perspective(fieldOfView ,720.0f/576.0f,nearFrame,farFrame);

  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // load frag and vert shaders
  constexpr auto ColourShader="ColourShader";
  constexpr auto ColourVertex="ColourVertex";
  constexpr auto ColourFragment="ColourFragment";
  shader->createShaderProgram(ColourShader);

  shader->attachShader(ColourVertex,ngl::ShaderType::VERTEX);
  shader->attachShader(ColourFragment,ngl::ShaderType::FRAGMENT);
  shader->loadShaderSource(ColourVertex,"shaders/ColourVertex.glsl");
  shader->loadShaderSource(ColourFragment,"shaders/ColourFragment.glsl");

  shader->compileShader(ColourVertex);
  shader->compileShader(ColourFragment);
  shader->attachShaderToProgram(ColourShader,ColourVertex);
  shader->attachShaderToProgram(ColourShader,ColourFragment);

  shader->linkProgramObject(ColourShader);
  (*shader)[ColourShader]->use();

  buildVAO();
  ngl::VAOFactory::listCreators();
}


void NGLScene::buildVAO()
{
  // create a vao using GL_LINES
  m_vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,GL_LINES);
  m_vao->bind();

  // set our data for the VAO
  m_vao->setData(ngl::SimpleIndexVAO::VertexData(
                                        sizeof(ngl::Vec3)*m_LSystem.m_vertices.size(),
                                        m_LSystem.m_vertices[0].m_x,
                                        uint(m_LSystem.m_indices.size()),
                                        &m_LSystem.m_indices[0],
                                        GL_UNSIGNED_SHORT));
  // data is 12 bytes apart (=sizeof(Vec3))
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  m_vao->setNumIndices(m_LSystem.m_indices.size());

  // now unbind
  m_vao->unbind();
}


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

  buildVAO();

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["ColourShader"]->use();

  m_view=ngl::lookAt(m_from, m_to, m_up);
  ngl::Mat4 MVP= m_project*m_view*m_mouseGlobalTX;
  shader->setUniform("MVP",MVP);

  m_vao->bind();
  m_vao->draw();
  m_vao->unbind();
}


//------------------------------------------------------------------------------------------------------------------------
///EVENTS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if ( m_win.rotate && _event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_win.origX;
    int diffy = _event->y() - m_win.origY;
    m_win.spinXFace = static_cast<int>( 0.4f * diffy );
    m_win.spinYFace = static_cast<int>( 0.4f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();

    ngl::Mat4 rotX;
    ngl::Mat4 rotY;
    rotX.rotateX(m_win.spinXFace);
    m_yRotAxis = rotX*m_yRotAxis;
    rotY.euler(m_win.spinYFace, m_yRotAxis.m_x, m_yRotAxis.m_y, m_yRotAxis.m_z);
    m_mouseGlobalTX = rotY*rotX*m_mouseGlobalTX;
    update();
  }

  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::MidButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_win.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();

    ngl::Mat4 trans;
    trans.translate(INCREMENT*diffX,-INCREMENT*diffY,0);
    m_mouseGlobalTX = trans*m_mouseGlobalTX;

    update();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.origX  = _event->x();
    m_win.origY  = _event->y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::MidButton )
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
    m_win.spinXFace = 0;
    m_win.spinYFace = 0;
  }
  // middle mouse translate mode
  if ( _event->button() == Qt::MidButton )
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{

  // check the diff of the wheel position (0 means no change)
  if ( _event->delta() > 0 )
  {
    m_from += ZOOM*(m_to-m_from);
  }
  else if ( _event->delta() < 0 )
  {
    m_from -= ZOOM*(m_to-m_from);
  }
  update();
}

//------------------------------------------------------------------------------------------------------------------------
///PUBLIC SLOTS
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::generate()
{
  m_rules = {};
  for(auto rule : m_ruleArray)
  {
    if(rule!="")
    {
      m_rules.push_back(rule);
    }
  }
  m_LSystem.m_axiom = m_axiom;
  m_LSystem.m_rules = m_rules;
  m_LSystem.update();
  m_LSystem.createGeometry(m_generation, ngl::Vec3(0,0,0));
  //buildVAO();
  update();
}

void NGLScene::setGeneration(int _generation)
{
  m_generation = _generation;
}
void NGLScene::setStepSize(double _stepSize)
{
  m_LSystem.m_stepSize = float(_stepSize);
}
void NGLScene::setStepScale(double _stepScale)
{
  m_LSystem.m_stepScale = float(_stepScale);
}
void NGLScene::setAngle(double _angle)
{
  m_LSystem.m_angle = float(_angle);
}
void NGLScene::setAngleScale(double _angleScale)
{
  m_LSystem.m_angleScale = float(_angleScale);
}

void NGLScene::setAxiom(QString _axiom)
{
  m_axiom = _axiom.toStdString();
}

void NGLScene::setRule1(QString _rule)
{
  m_ruleArray[0]=_rule.toStdString();
}
void NGLScene::setRule2(QString _rule)
{
  m_ruleArray[1]=_rule.toStdString();
}
void NGLScene::setRule3(QString _rule)
{
  m_ruleArray[2]=_rule.toStdString();
}
void NGLScene::setRule4(QString _rule)
{
  m_ruleArray[3]=_rule.toStdString();
}
void NGLScene::setRule5(QString _rule)
{
  m_ruleArray[4]=_rule.toStdString();
}
void NGLScene::setRule6(QString _rule)
{
  m_ruleArray[5]=_rule.toStdString();
}
void NGLScene::setRule7(QString _rule)
{
  m_ruleArray[6]=_rule.toStdString();
}
