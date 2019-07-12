//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @author Jonathan Macey
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
#include "WindowParams.h"
#include <QOpenGLWindow>
#include <memory>
#include "Camera.h"
#include "Forest.h"

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
  /// @param[in] treeTabNum, the int passed from m_tab in ui
  //----------------------------------------------------------------------------------------------------------------------
  void changeTab(int _treeTabNum);

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to tell QT to create a new tree from the L-System based on the user inputs
  //----------------------------------------------------------------------------------------------------------------------
  void generate();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to tell QT to reset the camera values
  //----------------------------------------------------------------------------------------------------------------------
  void resetCamera();

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a slot to set the number of generations to generate for this L-System
  /// @param[in] generation, the int passed from m_generation in ui
  //----------------------------------------------------------------------------------------------------------------------
  void setGeneration(int _generation);
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

protected:

  //PROTECTED MEMBER VARIABLES
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the current tab number
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_superTabNum = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the current tab number
  //----------------------------------------------------------------------------------------------------------------------
  size_t m_treeTabNum = 0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief matrices to set up camera view
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_view;
  ngl::Mat4 m_project;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the model position for mouse movement
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_modelPos;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the global mouse transforms
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_mouseGlobalTX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the VAO
  //----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<ngl::AbstractVAO> m_vao;

  size_t m_numSuperTabs = 3;
  size_t m_numTreeTabs = 2;
  std::vector<LSystem> m_LSystems;
  std::vector<std::vector<Camera>> m_cameras;
  std::vector<std::vector<ngl::Mat4>> m_mouseTransforms;

  LSystem * m_currentLSystem;
  Camera * m_currentCamera;
  ngl::Mat4 * m_currentMouseTransform;

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
  void buildLineVAO(std::vector<ngl::Vec3> &_vertices, std::vector<GLshort> &_indices);

  void initializeLSystems();

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
