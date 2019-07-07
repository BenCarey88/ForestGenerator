//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.h
/// @author Ben Carey
/// @version 1.0
/// @date 06/07/19
//----------------------------------------------------------------------------------------------------------------------

#ifndef LSYSTEM_H_
#define LSYSTEM_H_

#include <iostream>
#include <vector>
#include <ngl/Vec3.h>

//----------------------------------------------------------------------------------------------------------------------
/// @class LSystem
/// @brief this class stores details of the L-system
//----------------------------------------------------------------------------------------------------------------------


class LSystem
{
public:

  //CONSTRUCTOR
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  LSystem() = default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief user ctor for LSystem class
  //--------------------------------------------------------------------------------------------------------------------
  LSystem(std::string _axiom, std::vector<std::string> _rules);

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief initial axiom for the LSystem
  //--------------------------------------------------------------------------------------------------------------------
  std::string m_axiom;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the rules for the LSystem
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::string> m_rules;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the rules for the LSystem, broken down into vectors of size 2 (assuming the rule is written correctly)
  /// 1st element of each is the string to be replaced (called a non-terminal), and the 2nd element is the replacement
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::vector<std::string>> m_rulesBrokenDown;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief string representing all non-terminal elements in the rule system
  //--------------------------------------------------------------------------------------------------------------------
  std::string m_nonTerminals;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the branches introduced by rules in the L-system
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::string> m_branches;

  //PUBLIC MEMBER FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief fills m_rulesBrokenDown and m_nonTerminals
  //--------------------------------------------------------------------------------------------------------------------
  void breakDownRules();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief detects branches introduced by rules and uses them to fill m_branches
  //--------------------------------------------------------------------------------------------------------------------
  void detectBranching();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns a string representation of the tree produced by the L-System after a given number of generations
  /// @param [in] _generation the number of iterations of the rules to implement
  //--------------------------------------------------------------------------------------------------------------------
  std::string generateTreeString(int _generation);

};


#endif //LSYSTEM_H_
