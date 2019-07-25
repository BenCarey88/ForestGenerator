//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class methods that are only used for forestMode
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

void LSystem::addInstancingCommands()
{
  //put m_axiom as the first branch, since we'll instance the base of each tree
  //in the same way we instance the branches
  m_axiom = "{(0,0)"+m_axiom+"}";
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
      int bracketCount = 0;
      size_t id;
      size_t j=i+1;
      for(; j<_rhs.length(); j++)
      {
        if(_rhs[j]=='[')
        {
          bracketCount++;
        }
        if(_rhs[j]==']')
        {
          if(bracketCount==0)
          {
            break;
          }
          bracketCount--;
        }
      }
      if(j==_rhs.length())
      {
        continue;
      }
      std::string branch(_rhs.begin()+int(i+1),_rhs.begin()+int(j));
      //check that the branch contains at least one non-terminal
      if(std::regex_search(branch, std::regex(m_nonTerminals)))
      {
        //if the branch hasn't been added to m_branches already, then add it
        //^BUT THIS BIT SHOULD BE UNNECESSARY - THE BRANCHES HAVE ALREADY ALL BEEN ADDED TO M_BRANCHES BY COUNT BRANCHES
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
        size_t skipAmount = 0;
        if(_index % int(pow(2,count)) < int(pow(2,count-1)))
        {
          replacement = "<(" + std::to_string(id) + ",#)[" + branch + "]>";
          instanceCount++;
          skipAmount = 5+std::to_string(id).size();
        }
        else
        {
          replacement = "{(" + std::to_string(id) + ",#)[" + branch + "]}";
          nonInstanceCount++;
          //add to skipAmount to make sure we don't get caught in an endless loop with the same [
          skipAmount = 5+std::to_string(id).size();
        }

        _rhs.replace(i, j-i+1, replacement);
        i += skipAmount;
        //i += replacement.size();
        count++;
      }
    }
  }
  _prob *= pow(m_instancingProb, instanceCount) * pow(1-m_instancingProb, nonInstanceCount);
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::resizeInstanceCache()
{

  m_instanceCache.resize(m_branches.size(),{});
  for(auto &vector : m_instanceCache)
  {
    vector.resize(size_t(m_generation+1),{});
  }
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::fillInstanceCache(int _numHeroTrees)
{
  seedRandomEngine();
  m_forestMode = true;
  addInstancingCommands();
  resizeInstanceCache();
  m_heroIndices = {};
  m_heroVertices = {};

  for(int i=0; i<_numHeroTrees; i++)
  {
    createGeometry();
  }

  m_forestMode = false;

  /*for(size_t id=0; id<m_instanceCache.size(); id++)
  {
    std::cout<<"id = "<<id<<'\n';

    for(size_t age=0; age<m_instanceCache[id].size(); age++)
    {
      std::cout<<"  age = "<<age<<'\n';

      std::cout<<"    size of this level of nesting is "<<m_instanceCache[id][age].size()<<'\n';
    }
    std::cout<<'\n';
  }
  std::cout<<"\n--------------------------------------------------------\n";*/
}
