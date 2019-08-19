//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.h
/// @author Ben Carey
/// @version 1.0
/// @date 06/07/19
//----------------------------------------------------------------------------------------------------------------------

#ifndef LSYSTEM_H_
#define LSYSTEM_H_

#include <vector>
#include <random>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include "Instance.h"
#include "InstanceCacheMacros.h"
#include "PrintFunctions.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class LSystem
/// @brief this class stores details of the L-system
//----------------------------------------------------------------------------------------------------------------------


class LSystem
{
public:

  //CONSTRUCTORS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  LSystem() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief user ctor for LSystem class: assigns variables, calls breakDownRules to fill the rule lists and
  /// calls createGeometry to fill vertex and index buffers
  //--------------------------------------------------------------------------------------------------------------------
  LSystem(std::string _axiom, std::vector<std::string> _rules,
          float _stepSize, float _stepScale,
          float _angle, float _angleScale,
          float _thickness, float _thicknessScale,
          int _generation);

  //RULE STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct Rule
  /// @brief struct to store the rules for the LSystem in a format easily accessible by generateTreeString()
  //--------------------------------------------------------------------------------------------------------------------
  struct Rule
  {
    /// @brief ctor for Rule Structure, takes in LHS, RHS and probability lists
    Rule(std::string _LHS, std::vector<std::string> _RHS, std::vector<float> _prob);

    /// @brief the LHS of the rule, ie. a non-terminal symbol, to be replaced
    std::string m_LHS;
    /// @brief list of possible RHSs of the rule, strings to replace the LHS
    std::vector<std::string> m_RHS;
    /// @brief corresponding list of probabilities for each RHS
    std::vector<float> m_prob;
    /// @brief corresponding list of number of valid branch occurences in each RHS,
    /// used to help add in the instancing commands later on
    std::vector<int> m_numBranches;

    /// @brief method to 'normalize' all probabilities in m_prob so their sum is 1
    void normalizeProbabilities();
  };

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief initial axiom for the LSystem
  //--------------------------------------------------------------------------------------------------------------------
  std::string m_axiom;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the rules for the LSystem
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<Rule> m_rules;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief statically sized rule array, used by NGLScene class to fill rules from user inputs
  //--------------------------------------------------------------------------------------------------------------------
  std::array<std::string,20> m_ruleArray;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief string representing all non-terminal elements in the rule system
  //--------------------------------------------------------------------------------------------------------------------
  std::string m_nonTerminals;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the branches introduced by rules in the L-system
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::string> m_branches;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief initial step-size
  //--------------------------------------------------------------------------------------------------------------------
  float m_stepSize;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief step-size scale
  //--------------------------------------------------------------------------------------------------------------------
  float m_stepScale;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief initial rotation angle
  //--------------------------------------------------------------------------------------------------------------------
  float m_angle;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief angle scale
  //--------------------------------------------------------------------------------------------------------------------
  float m_angleScale;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief initial thickness
  //--------------------------------------------------------------------------------------------------------------------
  float m_thickness;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief thickness scale
  //--------------------------------------------------------------------------------------------------------------------
  float m_thicknessScale;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the number of generations of the LSystem to implement
  //--------------------------------------------------------------------------------------------------------------------
  int m_generation;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the seed to use to generate the stochastic behaviour of the tree
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_seed = 0;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine if we should use a seed - if not, we seed by time
  //--------------------------------------------------------------------------------------------------------------------
  bool m_useSeed = false;
  //--------------------------------------------------------------------------------------------------------------------
  //the random number generator to generate randomness for stochastic behaviour of tree
  //--------------------------------------------------------------------------------------------------------------------
  std::default_random_engine m_gen;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief toggle to determine if we should draww the L-System as a stick or a tube
  //--------------------------------------------------------------------------------------------------------------------
  bool m_skeletonMode = false;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief bool to tell if an error was thrown while parsing brackets when creating geometry
  //--------------------------------------------------------------------------------------------------------------------
  bool m_parameterError = false;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief instance cache holding instance data for branches at each id and age to be accessed by the instancing
  /// algorithm createTree() in the Forest class. Cache_Structure is a triple nesting of vectors:
  ///   outer layer separates instances by id
  ///   middle layer separates instances of the same id by age
  ///   inner layer separates multiple possible instances of the same id and age
  /// so accessing an istance is done by instanceCache[id][age][randomizer]
  //--------------------------------------------------------------------------------------------------------------------
  CACHE_STRUCTURE(Instance) m_instanceCache;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers to be sent to the renderer for drawing the main L-system geometry
  /// there are buffers for: vertices, indices, vector to the right of the current vertex and thickness value
  /// These will be drawn with GL_LINES and then optionally given geometry in a geometry shader if skeletonMode is false
  /// using m_rightVectors and m_thicknessValues to determine the geometry
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_vertices;
  std::vector<GLshort> m_indices;
  std::vector<ngl::Vec3> m_rightVectors;
  std::vector<float> m_thicknessValues;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers to be sent to the renderer for drawing the L-System default leaves
  /// there are buffers for: vertices, indices, current forwards direction and vector to the right of the current vertex
  /// These will be drawn with GL_POINTS, then turned into textured planes with geometry and fragment shaders, using
  /// m_leafDirections and m_leafRightVectors to determine the geometry of the plane
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_leafVertices = {};
  std::vector<GLshort> m_leafIndices = {};
  std::vector<ngl::Vec3> m_leafDirections = {};
  std::vector<ngl::Vec3> m_leafRightVectors = {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers to be sent to the renderer for drawing the L-System polygons
  /// there are buffers for: vertices and indices
  /// These will be drawn using GL_TRIANGLES, allowing for multiple disconnected polygon primitives
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_polygonVertices = {};
  std::vector<GLshort> m_polygonIndices = {};

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers representing the geometry of the hero trees, used for drawing branch instances for Forests
  /// there are buffers for: vertices, indices, vector to the right of the current vertex and thickness value
  /// These will be drawn with GL_LINES and then optionally given geometry in a geometry shader using m_heroRightVectors
  /// and m_heroThicknessValues to determine the geometry
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_heroVertices = {};
  std::vector<GLshort> m_heroIndices= {};
  std::vector<ngl::Vec3> m_heroRightVectors = {};
  std::vector<float> m_heroThicknessValues= {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers representing the hero tree leaves, used for drawing branch instances for Forests
  /// there are buffers for: vertices, indices, current forwards direction and vector to the right of the current vertex
  /// These will be drawn with GL_POINTS, then turned into textured planes with geometry and fragment shaders, using
  /// m_heroLeafDirections and m_heroLeafRightVectors to determine the geometry of the plane
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_heroLeafVertices = {};
  std::vector<GLshort> m_heroLeafIndices = {};
  std::vector<ngl::Vec3> m_heroLeafDirections = {};
  std::vector<ngl::Vec3> m_heroLeafRightVectors = {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief buffers representing the hero tree polygons, used for drawing branch instances for Forests
  /// there are buffers for: vertices, indices, current forwards direction and vector to the right of the current vertex
  /// These will be drawn using GL_TRIANGLES, allowing for multiple disconnected polygon primitives
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_heroPolygonVertices = {};
  std::vector<GLshort> m_heroPolygonIndices = {};

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief probability of instancing any given branch
  //--------------------------------------------------------------------------------------------------------------------
  float m_instancingProb = 0.6f;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief int used to limit size of instance cache to decrease program memory requirements
  //--------------------------------------------------------------------------------------------------------------------
  size_t m_maxInstancePerLevel = 10;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief boolean toggle to determine whether we should be filling hero buffers or regular buffers
  //--------------------------------------------------------------------------------------------------------------------
  bool m_forestMode = false;




  //GENERAL MEMBER FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief seeds the random engine using m_seed or the current time depending on the state of m_useSeed
  //--------------------------------------------------------------------------------------------------------------------
  void seedRandomEngine();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief counts branches in each rule and uses to fill m_rules[i].m_numBranches for each rule
  //--------------------------------------------------------------------------------------------------------------------
  void countBranches();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fills m_rules and m_nonTerminals from the user-set rule arrays
  //--------------------------------------------------------------------------------------------------------------------
  void breakDownRules(std::vector<std::string> _rules);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns a string representation of the tree produced by the L-System
  //--------------------------------------------------------------------------------------------------------------------
  std::string generateTreeString();

  //GEOMETRY CREATION METHODS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fills m_vertices and m_indices to represent the geometry of the L-System
  //--------------------------------------------------------------------------------------------------------------------
  void createGeometry();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief used by createGeometry to deal with a parameter enclosed by brackets in the tree string
  /// @param [in] _treeString the string
  /// @param [in] _i the index of _treeString that createGeometry() has reached
  /// @param [in] _paramVar the variable that will be replaced by the parameter in the brackets if needed
  //--------------------------------------------------------------------------------------------------------------------
  void parseBrackets(const std::string &_treeString, size_t &_i, float &_paramVar);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief used by createGeometry to deal with the two parameters enclosed by brackets following an instance command
  /// @param [in] _treeString the string
  /// @param [in] _i the index of _treeString that createGeometry() has reached
  /// @param [in] _id,_age, variables that will be assigned the values of the parameters in the brackets
  //--------------------------------------------------------------------------------------------------------------------
  void parseInstanceBrackets(const std::string &_treeString, size_t &_i, size_t &_id, size_t &_age);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief
  //--------------------------------------------------------------------------------------------------------------------
  void skipToNextChevron(const std::string &_treeString, size_t &_i);

  //INSTANCE METHODS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recreates m_rules to add more RHSs to each rule corresponding to different instancing commands
  //--------------------------------------------------------------------------------------------------------------------
  void addInstancingCommands();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief replaces branches in the rhs with @ and {} commands, and alters the probability accordingly
  /// used by addInstancingCommands
  //--------------------------------------------------------------------------------------------------------------------
  void addInstancingToRule(std::string &_rhs, float &_prob, int _index);
  //--------------------------------------------------------------------------------------------------------------------
  ///@brief calls createGeometry() in m_forestMode to makes hero trees to fill instance cache
  //--------------------------------------------------------------------------------------------------------------------
  void fillInstanceCache(int _numHeroTrees);
};


#endif //LSYSTEM_H_
