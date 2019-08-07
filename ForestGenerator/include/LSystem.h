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
public://--------------------------------------------------------------------------------------------------------------------

  //CONSTRUCTOR
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  LSystem() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief user ctor for LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  LSystem(std::string _axiom, std::vector<std::string> _rules,
          float _stepSize, float _stepScale,
          float _angle, float _angleScale,
          float _thickness, float _thicknessScale,
          int _generation);

  //RULE STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct Rule
  /// @brief this structure stores rules for the LSystem in a format easily accessible by generateTreeString()
  //--------------------------------------------------------------------------------------------------------------------
  struct Rule
  {
    //------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for Rule Structure
    //------------------------------------------------------------------------------------------------------------------
    Rule(std::string _LHS, std::vector<std::string> _RHS, std::vector<float> _prob);

    //------------------------------------------------------------------------------------------------------------------
    /// @brief the LHS of the rule, ie. a non-terminal symbol, to be replaced
    //------------------------------------------------------------------------------------------------------------------
    std::string m_LHS;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief list of possible RHSs of the rule, strings to replace the LHS
    //------------------------------------------------------------------------------------------------------------------
    std::vector<std::string> m_RHS;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief corresponding list of probabilities for each RHS
    //------------------------------------------------------------------------------------------------------------------
    std::vector<float> m_prob;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief corresponding list of number of branch occurences in each RHS
    //------------------------------------------------------------------------------------------------------------------
    std::vector<int> m_numBranches;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief method to normalize all probabilities in m_prob
    //------------------------------------------------------------------------------------------------------------------
    void normalizeProbabilities();
  };

  std::string m_name;

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
  std::array<std::string,10> m_ruleArray;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief string representing all non-terminal elements in the rule system
  //--------------------------------------------------------------------------------------------------------------------
  std::string m_nonTerminals;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the branches introduced by rules in the L-system
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::string> m_branches;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the step-size
  //--------------------------------------------------------------------------------------------------------------------
  float m_stepSize;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the step-size scale
  //--------------------------------------------------------------------------------------------------------------------
  float m_stepScale;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the rotation angle
  //--------------------------------------------------------------------------------------------------------------------
  float m_angle;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the angle scale
  //--------------------------------------------------------------------------------------------------------------------
  float m_angleScale;

  float m_thickness;
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

  bool m_skeletonMode = false;

  //the random number generator
  std::default_random_engine m_gen;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief bool to tell if an error was thrown while parsing brackets when creating geometry
  //--------------------------------------------------------------------------------------------------------------------
  bool m_parameterError = false;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief probability of instancing a given branch
  //--------------------------------------------------------------------------------------------------------------------
  float m_instancingProb = 0.6f;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief vertex list to store the vertices of L-system geometry
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<ngl::Vec3> m_vertices;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief index list to tell ngl how to draw the order to draw the L-system vertices in
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<GLshort> m_indices;

  std::vector<ngl::Vec3> m_rightVectors;
  std::vector<float> m_thicknessValues;

  std::vector<ngl::Vec3> m_heroVertices = {};
  std::vector<GLshort> m_heroIndices= {};
  std::vector<ngl::Vec3> m_heroRightVectors = {};
  std::vector<float> m_heroThicknessValues= {};
  bool m_forestMode = false;

  size_t m_maxInstancePerLevel = 10;

  //instance cache is vectors of instances nested 3 deep
  //outer layer separates instances by id
  //middle layer separates instances of the same id by age
  //inner layer separates multiple possible instances of the same id and age
  //so accessing an istance is done by instanceCache[id][age][randomizer]
  CACHE_STRUCTURE(Instance) m_instanceCache;

  ///@brief makes hero trees to fill instance cache
  void fillInstanceCache(int _numHeroTrees);

  //PUBLIC MEMBER FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief counts branches in each rule and uses to fill m_rules[i].m_numBranches for each rule
  //--------------------------------------------------------------------------------------------------------------------
  void countBranches();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fills m_rules and m_nonTerminals
  //--------------------------------------------------------------------------------------------------------------------
  void breakDownRules(std::vector<std::string> _rules);

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recreates m_rules to add more RHSs to each rule corresponding to different instancing commands
  //--------------------------------------------------------------------------------------------------------------------
  void addInstancingCommands();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief replaces branches in the rhs with @ and {} commands, and alters the probability accordingly
  /// used by add instancingCommands
  //--------------------------------------------------------------------------------------------------------------------
  void addInstancingToRule(std::string &_rhs, float &_prob, int _index);

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns a string representation of the tree produced by the L-System
  //--------------------------------------------------------------------------------------------------------------------
  std::string generateTreeString();

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fills m_vertices and m_indices to represent the geometry of the L-System
  //--------------------------------------------------------------------------------------------------------------------
  void createGeometry();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief used by createGeometry to deal with parameters enclosed by brackets in the tree string
  /// @param [in] _treeString the string
  /// @param [in] _i the index of _treeString that createGeometry() has reached
  /// @param [in] _paramVar the variable that will be replaced by the parameter in the brackets if needed
  //--------------------------------------------------------------------------------------------------------------------
  void parseBrackets(const std::string &_treeString, size_t &_i, float &_paramVar);

  void parseInstanceBrackets(const std::string &_treeString, size_t &_i, size_t &_id, size_t &_age);
  void skipToNextChevron(const std::string &_treeString, size_t &_i);

  void seedRandomEngine();
};


#endif //LSYSTEM_H_
