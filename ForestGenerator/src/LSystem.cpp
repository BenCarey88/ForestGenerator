//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <ngl/Mat3.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>
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
  std::string treeString = m_axiom;
  int numberOfRules = int(m_rules.size());

  for(int i=0; i<_generation; i++)
  {
    size_t ruleNumber = size_t(i % numberOfRules);
    std::string lhs = m_rulesBrokenDown[ruleNumber][0];
    std::string rhs = m_rulesBrokenDown[ruleNumber][1];
    boost::replace_all(treeString, lhs, rhs);
  }
  return treeString;
}

void LSystem::createGeometry(int _generation, ngl::Vec3 _startPos) //, ngl::Vec3 _orientation)
{
  const ngl::Vec3 up(0,1,0);
  ngl::Vec3 right(1,0,0);
  ngl::Vec3 direction(0,1,0);
  ngl::Mat3 rotate;
  std::string treeString = generateTreeString(_generation);
  m_vertices = {_startPos};
  m_indices = {};
  ngl::Vec3 lastVertex = _startPos;
  GLshort lastIndex = 0;
  for(auto c : treeString)
  {
    switch(c)
    {
      //Move forward
      case 'F':
        m_indices.push_back(lastIndex);
        lastVertex += m_stepSize*direction;
        lastIndex += 1;
        m_vertices.push_back(lastVertex);
        m_indices.push_back(lastIndex);
      break;

      //Roll clockwise
      case '/':
        rotate.euler(m_angle, up.m_x, up.m_y, up.m_z);
        direction = rotate*direction;
        right = rotate*right;
      break;

      //Roll anticlockwise
      case '\\':
        rotate.euler(-m_angle, up.m_x, up.m_y, up.m_z);
        direction = rotate*direction;
        right = rotate*right;
      break;

      //Pitch up
      case '&':
        rotate.euler(-m_angle, right.m_x, right.m_y, right.m_z);
        std::cout<<'\n'<<rotate.m_00<<' '<<rotate.m_01<<' '<<rotate.m_02;
        std::cout<<'\n'<<rotate.m_10<<' '<<rotate.m_11<<' '<<rotate.m_12;
        std::cout<<'\n'<<rotate.m_20<<' '<<rotate.m_21<<' '<<rotate.m_22<<'\n';
        direction = rotate*direction;
        std::cout<<'\n'<<direction.m_x<<' '<<direction.m_y<<' '<<direction.m_z<<'\n';
      break;

      default:
      break;
    }
  }
  std::cout<<"DONE \n\n";

}
