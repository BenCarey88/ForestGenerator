//----------------------------------------------------------------------------------------------------------------------
/// @file main.cpp
/// @brief calls MainWindow to create a Qapplication
//----------------------------------------------------------------------------------------------------------------------

#include <QApplication>
#include "MainWindow.h"
#include <random>
#include <chrono>

int main(int argc, char **argv)
{
  /*std::default_random_engine generator;
  float seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator.seed(7);
  std::uniform_real_distribution<double> distribution(0.0,1.0);
  for(int i=0; i<10; i++)
  {
    std::cout<<distribution(generator)<<'\n';
  }*/

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
