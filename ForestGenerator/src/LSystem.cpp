//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <regex>
#include "LSystem.h"

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules) :
  m_axiom(_axiom), m_rules(_rules){}

std::string LSystem::generateTreeString(int _generation)
{
  std::string tree = m_axiom;
  int numberOfRules = int(m_rules.size());
  for(int i=0; i<_generation; i++)
  {
    std::string currentRule = m_rules[size_t(i%numberOfRules)];

  }
  return tree;
}
