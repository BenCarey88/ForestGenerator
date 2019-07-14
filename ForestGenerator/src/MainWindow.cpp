//----------------------------------------------------------------------------------------------------------------------
/// @file MainWindow.cpp
/// @brief Implementation of MainWindow signals and slots
//----------------------------------------------------------------------------------------------------------------------

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);
  m_gl=new  NGLScene(this);
  m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);

  ///Signals and Slots
  //------------------------------------------------------------------------------------

  connect(m_ui->m_grid,SIGNAL(stateChanged(int)),m_gl,SLOT(toggleGrid(int)));
  connect(m_ui->m_resetCamera_layout,SIGNAL(clicked()),m_gl,SLOT(resetCamera()));
  connect(m_ui->m_resetCamera_render,SIGNAL(clicked()),m_gl,SLOT(resetCamera()));

  connect(m_ui->m_superTab,SIGNAL(currentChanged(int)),m_gl,SLOT(changeSuperTab(int)));
  connect(m_ui->m_tab,SIGNAL(currentChanged(int)),m_gl,SLOT(changeTab(int)));

  //TREE 1
  //------------------------------------------------------------------------------------

  connect(m_ui->m_generate_1,SIGNAL(clicked()),m_gl,SLOT(generate()));
  connect(m_ui->m_resetCamera_1,SIGNAL(clicked()),m_gl,SLOT(resetCamera()));
  connect(m_ui->m_stepSize_1,SIGNAL(valueChanged(double)),m_gl,SLOT(setStepSize(double)));
  connect(m_ui->m_stepScale_1,SIGNAL(valueChanged(double)),m_gl,SLOT(setStepScale(double)));
  connect(m_ui->m_angle_1,SIGNAL(valueChanged(double)),m_gl,SLOT(setAngle(double)));
  connect(m_ui->m_angleScale_1,SIGNAL(valueChanged(double)),m_gl,SLOT(setAngleScale(double)));
  connect(m_ui->m_generation_1,SIGNAL(valueChanged(int)),m_gl,SLOT(setGeneration(int)));
  connect(m_ui->m_seed_1,SIGNAL(valueChanged(int)),m_gl,SLOT(setSeed(int)));
  connect(m_ui->m_seedToggle_1,SIGNAL(stateChanged(int)),m_gl,SLOT(seedToggle(int)));

  connect(m_ui->m_axiom_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setAxiom(QString)));
  connect(m_ui->m_rule1_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule1(QString)));
  connect(m_ui->m_rule2_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule2(QString)));
  connect(m_ui->m_rule3_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule3(QString)));
  connect(m_ui->m_rule4_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule4(QString)));
  connect(m_ui->m_rule5_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule5(QString)));
  connect(m_ui->m_rule6_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule6(QString)));
  connect(m_ui->m_rule7_1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule7(QString)));

  //TREE 2
  //------------------------------------------------------------------------------------

  connect(m_ui->m_generate_2,SIGNAL(clicked()),m_gl,SLOT(generate()));
  connect(m_ui->m_resetCamera_2,SIGNAL(clicked()),m_gl,SLOT(resetCamera()));
  connect(m_ui->m_stepSize_2,SIGNAL(valueChanged(double)),m_gl,SLOT(setStepSize(double)));
  connect(m_ui->m_stepScale_2,SIGNAL(valueChanged(double)),m_gl,SLOT(setStepScale(double)));
  connect(m_ui->m_angle_2,SIGNAL(valueChanged(double)),m_gl,SLOT(setAngle(double)));
  connect(m_ui->m_angleScale_2,SIGNAL(valueChanged(double)),m_gl,SLOT(setAngleScale(double)));
  connect(m_ui->m_generation_2,SIGNAL(valueChanged(int)),m_gl,SLOT(setGeneration(int)));
  connect(m_ui->m_seed_2,SIGNAL(valueChanged(int)),m_gl,SLOT(setSeed(int)));
  connect(m_ui->m_seedToggle_2,SIGNAL(stateChanged(int)),m_gl,SLOT(seedToggle(int)));

  connect(m_ui->m_axiom_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setAxiom(QString)));
  connect(m_ui->m_rule1_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule1(QString)));
  connect(m_ui->m_rule2_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule2(QString)));
  connect(m_ui->m_rule3_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule3(QString)));
  connect(m_ui->m_rule4_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule4(QString)));
  connect(m_ui->m_rule5_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule5(QString)));
  connect(m_ui->m_rule6_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule6(QString)));
  connect(m_ui->m_rule7_2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule7(QString)));

}

MainWindow::~MainWindow()
{
  delete m_ui;
}

void MainWindow::on_m_seedToggle_1_stateChanged(int _clicked)
{
    m_ui->m_seed_1->setEnabled(_clicked);
}
void MainWindow::on_m_seedToggle_2_stateChanged(int _clicked)
{
    m_ui->m_seed_2->setEnabled(_clicked);
}
