//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class methods that only occur during ForestMode
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>
#include <random>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <math.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <ngl/Mat3.h>
#include <ngl/Mat4.h>
#include "LSystem.h"

//----------------------------------------------------------------------------------------------------------------------

LSystem::Instance::Instance(ngl::Mat4 _transform) :
  m_transform(_transform) {}

LSystem::Instance::ExitPoint::ExitPoint(size_t _exitId, size_t _exitAge, ngl::Mat4 _exitTransform) :
  m_exitId(_exitId), m_exitAge(_exitAge), m_exitTransform(_exitTransform) {}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::resizeInstanceCache()
{
  m_instanceCache.resize(m_branches.size());
  for(auto &vector : m_instanceCache)
  {
    vector.resize(size_t(m_generation));
  }
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::addInstancingCommands()
{
  m_branches = {};
  for(auto &rule : m_rules)
  {
    std::vector<std::string> tmpRHS = {};
    std::vector<float> tmpProb = {};
    for(size_t i=0; i<rule.m_RHS.size(); i++)
    {
      int numRHSs = int(pow(2,rule.m_numBranches[i]));
      for(int j=0; j<numRHSs; j++)
      {
        std::string rhs = rule.m_RHS[i];
        float prob = rule.m_prob[i];
        addInstancingToRule(rhs, prob, j);
        tmpRHS.push_back(rhs);
        tmpProb.push_back(prob);
      }
    }
    rule.m_RHS = tmpRHS;
    rule.m_prob = tmpProb;
  }
}

void LSystem::addInstancingToRule(std::string &_rhs, float &_prob, int _index)
{
  int count = 1;
  int instanceCount = 0;
  int nonInstanceCount = 0;
  for(size_t i=0; i<_rhs.length(); i++)
  {
    if(_rhs[i]=='[')
    {
      size_t id;
      size_t j=i+1;
      for(; j<_rhs.length(); j++)
      {
        if(_rhs[j]==']')
          break;
      }
      if(j==_rhs.length())
      {
        break;
      }
      std::string branch(_rhs.begin()+int(i+1),_rhs.begin()+int(j));
      //check that the branch contains at least one non-terminal
      if(std::regex_search(branch, std::regex(m_nonTerminals)))
      {
        //if the branch hasn't been added to m_branches already, then add it
        auto it = std::find(m_branches.begin(), m_branches.end(), branch);
        if(it == m_branches.end())
        {
          id = m_branches.size();
          m_branches.push_back(branch);
        }
        //otherwise, the id is the index of the branch in m_branches
        else
        {
          id = size_t(std::distance(m_branches.begin(),it));
        }

        std::string replacement;
        //float prob;
        if(_index % int(pow(2,count)) < int(pow(2,count-1)))
        {
          replacement = "@("+std::to_string(id)+",#)";
          instanceCount++;
        }
        else
        {
          replacement = "{(" + std::to_string(id) + ",#)[" + branch + "]}(" + std::to_string(id) + ",#)";
          nonInstanceCount++;
        }

        _rhs.replace(i, j-i+1, replacement);
        i += replacement.size();
        count++;
      }
      else
      {
        i=j;
      }
    }
  }
  _prob *= pow(m_instancingProb, instanceCount) * pow(1-m_instancingProb, nonInstanceCount);
}
