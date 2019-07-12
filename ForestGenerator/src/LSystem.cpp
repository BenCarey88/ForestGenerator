//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <random>
#include <stdexcept>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include "LSystem.h"

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules) :
  m_axiom(_axiom), m_rules(_rules)
{
  for(size_t i=0; i<_rules.size() ; i++)
  {
    m_ruleArray.at(i)=_rules[i];
  }
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

//I think it's gonna make sense to remove _startPos and _orientation from this
//generate all L-systems from (0,0,0), pointing towards (0,1,0) then perform transformations by adding
//them to the MVP matrix after
void LSystem::createGeometry(int _generation, ngl::Vec3 _startPos, ngl::Vec3 _orientation)
{
  ngl::Vec3 dir = _orientation;
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
  //paramVar will store the default value of each command, to be replaced by one
  //parsed from brackets by parseBrackets() if necessary
  float paramVar;

  std::vector<ngl::Vec3> savedVert = {};
  std::vector<GLshort> savedInd = {};
  std::vector<ngl::Vec3> savedDir = {};
  std::vector<ngl::Vec3> savedRight = {};
  std::vector<float> savedStep = {};
  std::vector<float> savedAngle = {};

  m_vertices = {_startPos};
  m_indices = {};

  for(size_t i=0; i<treeString.size(); i++)
  {
    char c = treeString[i];
    switch(c)
    {
      //move forward
      case 'F':
        m_indices.push_back(lastIndex);
        paramVar = stepSize;
        parseBrackets(treeString, i, paramVar);
        lastVertex += paramVar*dir;
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
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(paramVar, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
        break;

      //roll anticlockwise
      case '\\':
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(-paramVar, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
        break;

      //pitch up
      case '&':
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(paramVar, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
        break;

      //pitch down
      case '^':
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(-paramVar, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
        break;

      //scale step size
      case '\"':
        paramVar = m_stepScale;
        parseBrackets(treeString, i, paramVar);
        stepSize *= paramVar;
        break;

      //scale angle
      case ';':
        paramVar = m_angleScale;
        parseBrackets(treeString, i, paramVar);
        angle *= paramVar;
        break;

      default:
        break;
    }
  }
  std::cout<<treeString<<'\n';


  if(m_parameterError)
  {
    std::cerr<<"NOTE: unable to parse one or more parameters"<<'\n';
    m_parameterError = false;
  }
}

void LSystem::parseBrackets(std::string _treeString, size_t &_i, float &_output)
{
  if(_treeString[_i+1]=='(')
  {
    size_t j=_i+2;
    for( ; j<_treeString.size(); j++)
    {
      if(_treeString[j]==')')
      {
        break;
      }
    }
    if(j!=_treeString.size() && j>_i+2)
    {
      try
      {
        std::string parameter = _treeString.substr(_i+2, j-_i-2);
        _output = std::stof(parameter);
      }
      catch(std::invalid_argument)
      {
        m_parameterError = true;
      }
      catch(std::out_of_range)
      {
        m_parameterError = true;
      }
      _i=j;
    }
  }
}
