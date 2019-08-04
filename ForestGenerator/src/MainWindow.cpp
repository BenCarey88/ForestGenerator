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

  //------------------------------------------------------------------------------------
  ///@brief Signals and Slots
  //------------------------------------------------------------------------------------

  //TAB CONTROLS
  //-----------------------------------------------------------------------------------
  connect(m_ui->m_superTab, SIGNAL(currentChanged(int)), m_gl, SLOT(changeSuperTab(int)));
  connect(m_ui->m_treeTab, SIGNAL(currentChanged(int)), m_gl, SLOT(changeTreeTab(int)));
  connect(m_ui->m_terraintTab, SIGNAL(currentChanged(int)), m_gl, SLOT(changeTerrainTab(int)));

  //TREE TAB
  //------------------------------------------------------------------------------------
  #define LSYSTEM_SIGNALS_AND_SLOTS(NUM)                                                                 \
    connect(m_ui->m_axiom_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setAxiom(QString)));           \
    connect(m_ui->m_rule1_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule1(QString)));           \
    connect(m_ui->m_rule2_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule2(QString)));           \
    connect(m_ui->m_rule3_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule3(QString)));           \
    connect(m_ui->m_rule4_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule4(QString)));           \
    connect(m_ui->m_rule5_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule5(QString)));           \
    connect(m_ui->m_rule6_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule6(QString)));           \
    connect(m_ui->m_rule7_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule7(QString)));           \
                                                                                                         \
    connect(m_ui->m_generate_##NUM, SIGNAL(clicked()), m_gl, SLOT(generate()));                          \
    connect(m_ui->m_resetCamera_##NUM, SIGNAL(clicked()), m_gl, SLOT(resetCamera()));                    \
    connect(m_ui->m_stepSize_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setStepSize(double)));      \
    connect(m_ui->m_stepScale_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setStepScale(double)));    \
    connect(m_ui->m_angle_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setAngle(double)));            \
    connect(m_ui->m_angleScale_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setAngleScale(double)));  \
    connect(m_ui->m_generation_##NUM, SIGNAL(valueChanged(int)), m_gl, SLOT(setGeneration(int)));        \
    connect(m_ui->m_seed_##NUM, SIGNAL(valueChanged(int)), m_gl, SLOT(setSeed(int)));                    \
    connect(m_ui->m_seedToggle_##NUM, SIGNAL(stateChanged(int)), m_gl, SLOT(seedToggle(int)));           \
    connect(m_ui->m_grid_##NUM, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleGrid(int)));

  LSYSTEM_SIGNALS_AND_SLOTS(1);
  LSYSTEM_SIGNALS_AND_SLOTS(2);
  #undef LSYSTEM_SIGNALS_AND_SLOTS;

  //TERRAIN
  //------------------------------------------------------------------------------------
  connect(m_ui->m_terrainSize, SIGNAL(valueChanged(double)), m_gl, SLOT(setTerrainSize(double)));
  connect(m_ui->m_LOD, SIGNAL(valueChanged(int)), m_gl, SLOT(setLOD(int)));
  connect(m_ui->m_octaves, SIGNAL(valueChanged(int)), m_gl, SLOT(setOctaves(int)));
  connect(m_ui->m_frequency, SIGNAL(valueChanged(double)), m_gl, SLOT(setFrequency(double)));
  connect(m_ui->m_lacunarity, SIGNAL(valueChanged(double)), m_gl, SLOT(setLacunarity(double)));
  connect(m_ui->m_persistence, SIGNAL(valueChanged(double)), m_gl, SLOT(setPersistence(double)));
  connect(m_ui->m_amplitude, SIGNAL(valueChanged(double)), m_gl, SLOT(setAmplitude(double)));
  connect(m_ui->m_terrainSeed, SIGNAL(valueChanged(double)), m_gl, SLOT(setTerrainSeed(double)));

  connect(m_ui->m_tolerance, SIGNAL(valueChanged(double)), m_gl, SLOT(setTolerance(double)));
  connect(m_ui->m_wireframe, SIGNAL(toggled(bool)), m_gl, SLOT(toggleWireframe(bool)));
  connect(m_ui->m_updateTerrain, SIGNAL(clicked()), m_gl, SLOT(updateTerrain()));
  connect(m_ui->m_resetCamera_terrain, SIGNAL(clicked()), m_gl, SLOT(resetCamera()));

  //FOREST
  //------------------------------------------------------------------------------------
  connect(m_ui->m_resetCamera_render, SIGNAL(clicked()), m_gl, SLOT(resetCamera()));

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
