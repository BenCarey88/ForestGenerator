//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <random>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include "LSystem.h"

//----------------------------------------------------------------------------------------------------------------------

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules) :
  m_axiom(_axiom)
{
  for(size_t i=0; i<_rules.size() ; i++)
  {
    m_ruleArray.at(i)=_rules[i];
  }
  breakDownRules(_rules);
  detectBranching();
  createGeometry();
}

//----------------------------------------------------------------------------------------------------------------------

LSystem::Rule::Rule(std::string _LHS, std::vector<std::string> _RHS, std::vector<float> _prob) :
  m_LHS(_LHS), m_RHS(_RHS), m_prob(_prob){}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::breakDownRules(std::vector<std::string> _rules)
{
  m_rules = {};
  m_nonTerminals = "[";
  for(auto ruleString : _rules)
  {
    //LRP aims to store rules in the form {LHS, RHS, Probability}
    std::vector<std::string> LRP;
    //use boost::split to get LRP={"A","B","C"} when rule is "A=B:C"
    //note that if =,: symbols are used incorrectly this will give an incorrect result
    boost::split(LRP, ruleString, boost::is_any_of("=,:"));

    //only carry on if '=' appeared in the rule - otherwise, the rule is invalid
    //this stops the program crashing if a rule doesn't have an =, and just skips this rule instead
    //but technically this setup means "A=B:P" parses the same as "A:B=P" which is maybe a problem?
    if(std::regex_search(ruleString, std::regex("=+")))
    {
      //define probability as 1 unless given otherwise
      float probability = 1;
      if(LRP.size()>2)
      {
        try
        {
          probability = std::stof(LRP[2]);
        }
        catch(std::invalid_argument)
        {
          std::cerr<<"WARNING: unable to convert probability to string \n";
        }
        catch(std::out_of_range)
        {
          std::cerr<<"WARNING: unable to convert probability to string \n";
        }
      }

      //now if the LHS is already a LHS of some rule in m_rules, add the RHS and probabilities to that rule
      size_t i=0;
      for(; i<m_rules.size(); i++)
      {
        Rule &r = m_rules[i];
        if(LRP[0]==r.m_LHS)
        {
          r.m_RHS.push_back(LRP[1]);
          r.m_prob.push_back(probability);
          break;
        }
      }
      //otherwise if the LHS hasn't been seen before, create a new rule and add it to m_rules
      //and also add this new LHS to m_nonTerminals
      if(i==m_rules.size())
      {
        Rule r(LRP[0],{LRP[1]},{probability});
        m_rules.push_back(r);
        m_nonTerminals += LRP[0];
      }
    }
    else
    {
      std::cerr<<"WARNING: excluding rule because no replacement command was given \n";
    }
  }

  m_nonTerminals += "]+";
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::detectBranching()
{
  m_branches = {};
  for(auto rule : m_rules)
  {
    for(auto rhs : rule.m_RHS)
    {
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
}

//----------------------------------------------------------------------------------------------------------------------

std::string LSystem::generateTreeString()
{
  std::string treeString = m_axiom;
  int numRules = int(m_rules.size());

  std::default_random_engine gen;
  int seed = std::chrono::system_clock::now().time_since_epoch().count();
  gen.seed(seed);
  std::uniform_real_distribution<double> dist(0.0,1.0);

  if(numRules>0)
  {
    for(int i=0; i<m_generation; i++)
    {
      size_t ruleNum = size_t(i % numRules);
      std::string lhs = m_rules[ruleNum].m_LHS;
      std::vector<std::string> rhs = m_rules[ruleNum].m_RHS;
      std::vector<float> probabilities = m_rules[ruleNum].m_prob;

      //work out sum of all probabilities to normalise them
      float sumProb = 0;
      for(auto prob: probabilities)
      {
        sumProb += prob;
      }
      float sumProbInverse = 1/sumProb;

      size_t pos = treeString.find(lhs);
      size_t len = lhs.size();
      while(pos != std::string::npos)
      {
        float randNum = dist(gen);
        float count = 0;
        size_t j = 0;
        for( ; j<probabilities.size(); j++)
        {
          count += probabilities[j]*sumProbInverse;
          std::cout<<count<<"\n";
          if(count>=randNum)
          {
            break;
          }
        }
        treeString.replace(pos, len, rhs[j]);
        pos = treeString.find(lhs, pos+rhs[j].size());
      }
    }
  }
  return treeString;
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::createGeometry()
{
  ngl::Vec3 dir(0,1,0);
  ngl::Vec3 right(1,0,0);
  //I am using an ngl::Mat4 matrix for now because there is a problem with the euler
  //method for ngl::Mat3, so I am setting the rotation for r4 with r4.euler, then
  //using the copy constructor to transfer that rotation to r3
  ngl::Mat4 r4;
  ngl::Mat3 r3;
  std::string treeString = generateTreeString();

  ngl::Vec3 lastVertex(0,0,0);
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

  m_vertices = {lastVertex};
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
  if(m_parameterError)
  {
    std::cerr<<"WARNING: unable to parse one or more parameters \n";
    m_parameterError = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------

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
