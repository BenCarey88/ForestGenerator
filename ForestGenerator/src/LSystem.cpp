//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <boost/algorithm/string.hpp>
#include "LSystem.h"

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules) :
  m_axiom(_axiom), m_rules(_rules)
{
  breakDownRules();
  detectBranching();
}

void LSystem::breakDownRules()
{
  m_rulesBrokenDown = {};
  m_nonTerminals = "[";
  for(auto rule : m_rules)
  {
    std::vector<std::string> ruleArray;
    boost::split(ruleArray, rule, [](char c){return c == '=';});
    m_rulesBrokenDown.push_back(ruleArray);
    m_nonTerminals += ruleArray[0];
  }
  m_nonTerminals += "]+";
}

void LSystem::detectBranching()
{
  m_branches = {};
  for(auto ruleArray : m_rulesBrokenDown)
  {
    std::string rhs = ruleArray[1];
    for(size_t i=0; i<rhs.length(); i++)
    {
      if(rhs[i]=='[')
      {
        size_t j=i+1;
        for(; j<rhs.length(); j++)
        {
          if(rhs[j]==']')
            break;
        }
        std::string branch(rhs.begin()+int(i+1),rhs.begin()+int(j));
        //the first part of this if clause checks that the branch hasn't been added to m_branches already
        //and the second checks that it contains at least one non-terminal
        if(std::find(m_branches.begin(), m_branches.end(), branch) == m_branches.end() &&
           std::regex_search(branch, std::regex(m_nonTerminals)))
        {
          m_branches.push_back(branch);
        }
        i=j;
      }
    }
  }
}

std::string LSystem::generateTreeString(int _generation)
{
  std::string tree = m_axiom;
  int numberOfRules = int(m_rules.size());

  for(int i=0; i<_generation; i++)
  {
    size_t ruleNumber = size_t(i % numberOfRules);
    std::string lhs = m_rulesBrokenDown[ruleNumber][0];
    std::string rhs = m_rulesBrokenDown[ruleNumber][1];
    boost::replace_all(tree, lhs, rhs);
  }
  return tree;
}
