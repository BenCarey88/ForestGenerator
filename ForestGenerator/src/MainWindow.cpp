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

  connect(m_ui->m_generate,SIGNAL(clicked()),m_gl,SLOT(generate()));
  connect(m_ui->m_generation,SIGNAL(valueChanged(int)),m_gl,SLOT(setGeneration(int)));
  connect(m_ui->m_axiom,SIGNAL(textChanged(QString)),m_gl,SLOT(setAxiom(QString)));
  connect(m_ui->m_rule1,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule1(QString)));
  connect(m_ui->m_rule2,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule2(QString)));
  connect(m_ui->m_rule3,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule3(QString)));
  connect(m_ui->m_rule4,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule4(QString)));
  connect(m_ui->m_rule5,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule5(QString)));
  connect(m_ui->m_rule6,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule6(QString)));
  connect(m_ui->m_rule7,SIGNAL(textChanged(QString)),m_gl,SLOT(setRule7(QString)));
}

MainWindow::~MainWindow()
{
  delete m_ui;
}
