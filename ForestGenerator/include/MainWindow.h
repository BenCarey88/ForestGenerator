//----------------------------------------------------------------------------------------------------------------------
/// @file MainWindow.h
/// @author QT, Ben Carey
/// @version 2.0
/// @date 05/07/19
/// Revision History :
/// 05/07/19: updated by Ben Carey for use in ForestGenerator project
//----------------------------------------------------------------------------------------------------------------------

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "NGLScene.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class MainWindow
/// @brief this class creates our QT GUI and includes our NGLScene
//----------------------------------------------------------------------------------------------------------------------

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  /// @brief slots to enable the seed spin box when the seedToggle is selected
  void on_m_seedToggle_1_stateChanged(int _clicked);
  void on_m_seedToggle_2_stateChanged(int _clicked);

private:
  /// @brief our user interface
  Ui::MainWindow *m_ui;
  /// @brief our openGL widget
  NGLScene *m_gl;
};

#endif // MAINWINDOW_H
