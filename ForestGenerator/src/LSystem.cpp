//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include "LSystem.h"

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules) :
  m_axiom(_axiom), m_rules(_rules)
{
  breakDownRules();
  detectBranching();
}

void LSystem::update()
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

  if(numberOfRules>0)
  {
      for(int i=0; i<_generation; i++)
    {
      size_t ruleNumber = size_t(i % numberOfRules);
      std::string lhs = m_rulesBrokenDown[ruleNumber][0];
      std::string rhs = m_rulesBrokenDown[ruleNumber][1];
      boost::replace_all(treeString, lhs, rhs);
    }
  }
  return treeString;
}

void LSystem::createGeometry(int _generation, ngl::Vec3 _startPos) //, ngl::Vec3 _orientation)
{
  ngl::Vec3 dir(0,1,0); // direction = _orientation;
  ngl::Vec3 right(0,1,0);
  if(dir != ngl::Vec3(0,0,1))
  {
    right = dir.cross(ngl::Vec3(0,0,1));
  }
  right.normalize();
  //I am using an ngl::Mat4 matrix for now because there is a problem with the euler
  //method for ngl::Mat3, so I am setting the rotation for r4 with r4.euler, then
  //using the copy constructor to transfer that rotation to r3
  ngl::Mat4 r4;
  ngl::Mat3 r3;
  std::string treeString = generateTreeString(_generation);

  ngl::Vec3 lastVertex = _startPos;
  GLshort lastIndex = 0;
  float stepSize = m_stepSize;
  float angle = m_angle;

  std::vector<ngl::Vec3> savedVert = {};
  std::vector<GLshort> savedInd = {};
  std::vector<ngl::Vec3> savedDir = {};
  std::vector<ngl::Vec3> savedRight = {};
  std::vector<float> savedStep = {};
  std::vector<float> savedAngle = {};

  m_vertices = {_startPos};
  m_indices = {};

  for(auto c : treeString)
  {
    switch(c)
    {
      //move forward
      case 'F':
        m_indices.push_back(lastIndex);
        lastVertex += stepSize*dir;
        m_vertices.push_back(lastVertex);
        lastIndex = GLshort(m_vertices.size()-1);
        m_indices.push_back(lastIndex);
      break;

      //start branch
      case '[':
        savedInd.push_back(lastIndex);
        savedVert.push_back(lastVertex);
        savedDir.push_back(dir);
        savedRight.push_back(right);
        savedStep.push_back(stepSize);
        savedAngle.push_back(angle);
      break;

      //end branch
      case ']':
        lastIndex = savedInd.back();
        lastVertex = savedVert.back();
        dir = savedDir.back();
        right = savedRight.back();
        stepSize = savedStep.back();
        angle = savedAngle.back();

        savedInd.pop_back();
        savedVert.pop_back();
        savedDir.pop_back();
        savedRight.pop_back();
        savedStep.pop_back();
        savedAngle.pop_back();
      break;

      //roll clockwise
      case '/':
        r4.euler(angle, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
      break;

      //roll anticlockwise
      case '\\':
        r4.euler(-angle, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
      break;

      //pitch up
      case '&':
        r4.euler(angle, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
      break;

      //pitch down
      case '^':
        r4.euler(-angle, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
      break;

      //scale step size
      case '\"':
        stepSize *= m_stepScale;
      break;

      //scale angle
      case ';':
        angle *= m_angleScale;
      break;

      default:
      break;
    }
  }
}
