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
  connect(m_ui->m_forestTab, SIGNAL(currentChanged(int)), m_gl, SLOT(changeForestTab(int)));

  //TREE TAB
  //------------------------------------------------------------------------------------
  #define LSYSTEM_SIGNALS_AND_SLOTS(NUM)                                                                        \
    connect(m_ui->m_axiom_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setAxiom(QString)));                  \
    connect(m_ui->m_rule1_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule1(QString)));                  \
    connect(m_ui->m_rule2_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule2(QString)));                  \
    connect(m_ui->m_rule3_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule3(QString)));                  \
    connect(m_ui->m_rule4_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule4(QString)));                  \
    connect(m_ui->m_rule5_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule5(QString)));                  \
    connect(m_ui->m_rule6_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule6(QString)));                  \
    connect(m_ui->m_rule7_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule7(QString)));                  \
    connect(m_ui->m_rule8_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule8(QString)));                  \
    connect(m_ui->m_rule9_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule9(QString)));                  \
    connect(m_ui->m_rule10_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule10(QString)));                \
    connect(m_ui->m_rule11_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule11(QString)));                \
    connect(m_ui->m_rule12_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule12(QString)));                \
    connect(m_ui->m_rule13_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule13(QString)));                \
    connect(m_ui->m_rule14_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule14(QString)));                \
    connect(m_ui->m_rule15_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule15(QString)));                \
    connect(m_ui->m_rule16_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule16(QString)));                \
    connect(m_ui->m_rule17_##NUM, SIGNAL(textChanged(QString)), m_gl, SLOT(setRule17(QString)));                \
                                                                                                                \
    connect(m_ui->m_generate_##NUM, SIGNAL(clicked()), m_gl, SLOT(generate()));                                 \
    connect(m_ui->m_resetCamera_##NUM, SIGNAL(clicked()), m_gl, SLOT(resetCamera()));                           \
    connect(m_ui->m_stepSize_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setStepSize(double)));             \
    connect(m_ui->m_stepScale_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setStepScale(double)));           \
    connect(m_ui->m_angle_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setAngle(double)));                   \
    connect(m_ui->m_angleScale_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setAngleScale(double)));         \
    connect(m_ui->m_thickness_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setThickness(double)));           \
    connect(m_ui->m_thicknessScale_##NUM, SIGNAL(valueChanged(double)), m_gl, SLOT(setThicknessScale(double))); \
    connect(m_ui->m_generation_##NUM, SIGNAL(valueChanged(int)), m_gl, SLOT(setGeneration(int)));               \
    connect(m_ui->m_seed_##NUM, SIGNAL(valueChanged(int)), m_gl, SLOT(setSeed(int)));                           \
    connect(m_ui->m_seedToggle_##NUM, SIGNAL(stateChanged(int)), m_gl, SLOT(seedToggle(int)));                  \
    connect(m_ui->m_skeleton_##NUM, SIGNAL(toggled(bool)), m_gl, SLOT(toggleTreeSkeletonMode(bool)));           \
    connect(m_ui->m_grid_##NUM, SIGNAL(toggled(bool)), m_gl, SLOT(toggleGrid(bool)));

  LSYSTEM_SIGNALS_AND_SLOTS(1);
  LSYSTEM_SIGNALS_AND_SLOTS(2);
  LSYSTEM_SIGNALS_AND_SLOTS(3);
  #undef LSYSTEM_SIGNALS_AND_SLOTS

  //FOREST TAB
  //------------------------------------------------------------------------------------
  //Terrain
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
  connect(m_ui->m_updateTerrain, SIGNAL(clicked()), m_gl, SLOT(updateTerrain()));

  //------------------------------------------------------------------------------------
  //Trees
  //------------------------------------------------------------------------------------
  connect(m_ui->m_treeGenMethod, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(toggleTreeGenMethod(int)));
  connect(m_ui->m_paintBrush, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(setPaintBrush(int)));
  connect(m_ui->m_treeSpacing, SIGNAL(valueChanged(double)), m_gl, SLOT(setTreeSpacing(double)));
  connect(m_ui->m_erasePaint, SIGNAL(clicked()), m_gl, SLOT(erasePaint()));
  connect(m_ui->m_paint, SIGNAL(clicked(bool)), m_gl, SLOT(toggleTreePaintMode(bool)));
  connect(m_ui->m_numTrees_1, SIGNAL(valueChanged(int)), m_gl, SLOT(setNumTrees1(int)));
  connect(m_ui->m_numTrees_2, SIGNAL(valueChanged(int)), m_gl, SLOT(setNumTrees2(int)));
  connect(m_ui->m_numTrees_3, SIGNAL(valueChanged(int)), m_gl, SLOT(setNumTrees3(int)));
  connect(m_ui->m_updateForest, SIGNAL(clicked()), m_gl, SLOT(remakeForest()));

  connect(m_ui->m_numHeroTrees, SIGNAL(valueChanged(int)), m_gl, SLOT(setNumHeroTrees(int)));
  connect(m_ui->m_seed_forest, SIGNAL(valueChanged(int)), m_gl, SLOT(setSeedForest(int)));
  connect(m_ui->m_seedToggle_forest, SIGNAL(stateChanged(int)), m_gl, SLOT(seedToggleForest(int)));
  connect(m_ui->m_instancingProb, SIGNAL(valueChanged(double)), m_gl, SLOT(setInstancingProb(double)));
  connect(m_ui->m_resetCamera_terrain, SIGNAL(clicked()), m_gl, SLOT(resetCamera()));
  connect(m_ui->m_wireframe_terrain, SIGNAL(toggled(bool)), m_gl, SLOT(toggleTerrainWireframe(bool)));

}

MainWindow::~MainWindow()
{
  delete m_ui;
  delete m_gl;
}

void MainWindow::on_m_seedToggle_1_stateChanged(int _clicked)
{
    m_ui->m_seed_1->setEnabled(_clicked);
}
void MainWindow::on_m_seedToggle_2_stateChanged(int _clicked)
{
    m_ui->m_seed_2->setEnabled(_clicked);
}
void MainWindow::on_m_seedToggle_3_stateChanged(int _clicked)
{
    m_ui->m_seed_3->setEnabled(_clicked);
}
void MainWindow::on_m_seedToggle_forest_stateChanged(int _clicked)
{
    m_ui->m_seed_forest->setEnabled(_clicked);
}

void MainWindow::on_m_treeGenMethod_currentIndexChanged(int _index)
{
    m_ui->m_treeGenMethodStack->setCurrentIndex(_index);
    if(_index==1)
    {
      m_ui->m_paint->setChecked(0);
      m_gl->toggleTreePaintMode(false);
    }
}

