//----------------------------------------------------------------------------------------------------------------------
/// @file main.cpp
/// @brief calls MainWindow to create a Qapplication
//----------------------------------------------------------------------------------------------------------------------


#include <QApplication>
#include "MainWindow.h"
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>

int main(int argc, char **argv)
{
  ngl::Mat4 m4(0,1,2,3,
               4,5,6,7,
               8,9,10,11,
               12,13,14,15);
  ngl::Mat3 m3 = m4;
  ngl::Vec3 v(1,2,3);
  ngl::Vec3 u;
  u = m3*v;

  std::cout << m4.m_00 << ',' << m4.m_01 << ',' << m4.m_02 << ',' << m4.m_03 << '\n';
  std::cout << m4.m_10 << ',' << m4.m_11 << ',' << m4.m_12 << ',' << m4.m_13 << '\n';
  std::cout << m4.m_20 << ',' << m4.m_21 << ',' << m4.m_22 << ',' << m4.m_23 << '\n';
  std::cout << m4.m_30 << ',' << m4.m_31 << ',' << m4.m_32 << ',' << m4.m_33 << '\n';

  std::cout<<'\n';

  std::cout << m3.m_00 << ',' << m3.m_01 << ',' << m3.m_02 << '\n';
  std::cout << m3.m_10 << ',' << m3.m_11 << ',' << m3.m_12 << '\n';
  std::cout << m3.m_20 << ',' << m3.m_21 << ',' << m3.m_22 << '\n';

  std::cout<<'\n';

  std::cout<< u[0] << ',' << u[1] << ',' << u[2] << '\n';

  // create an OpenGL format specifier
  QSurfaceFormat format;
  // set the number of samples for multisampling
  // will need to enable glEnable(GL_MULTISAMPLE); once we have a context
  format.setSamples(4);
  #if defined( __APPLE__)
    // at present mac osx Mountain Lion only supports GL3.2
    // the new mavericks will have GL 4.x so can change
    format.setMajorVersion(4);
    format.setMinorVersion(2);
  #else
    // with luck we have the latest GL version so set to this
    format.setMajorVersion(4);
    format.setMinorVersion(3);
  #endif
  // now we are going to set to CoreProfile OpenGL so we can't use and old Immediate mode GL
  format.setProfile(QSurfaceFormat::CoreProfile);
  // now set the depth buffer to 24 bits
  format.setDepthBufferSize(24);

  // this will set the format for all widgets

  QSurfaceFormat::setDefaultFormat(format);
  // make an instance of the QApplication
  QApplication a(argc, argv);
  // Create a new MainWindow
  MainWindow w;
  // show it
  w.show();
  // hand control over to Qt framework
  return a.exec();
}


