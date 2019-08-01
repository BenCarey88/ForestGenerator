//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem.cpp
/// @brief implementation file for LSystem class
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <regex>

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

LSystem::LSystem(std::string _axiom, std::vector<std::string> _rules,
                 float _stepSize, float _stepScale,
                 float _angle, float _angleScale, int _generation) :
  m_axiom(_axiom), m_stepSize(_stepSize), m_stepScale(_stepScale),
  m_angle(_angle), m_angleScale(_angleScale), m_generation(_generation)
{
  for(size_t i=0; i<_rules.size() ; i++)
  {
    m_ruleArray.at(i)=_rules[i];
  }
  breakDownRules(_rules);
  createGeometry();
}

//----------------------------------------------------------------------------------------------------------------------

LSystem::Rule::Rule(std::string _LHS, std::vector<std::string> _RHS, std::vector<float> _prob) :
  m_LHS(_LHS), m_RHS(_RHS), m_prob(_prob){}

void LSystem::Rule::normalizeProbabilities()
{
  float sumProb = 0;
  for(auto prob: m_prob)
  {
    sumProb += prob;
  }
  float sumProbInverse = 1/sumProb;
  for(auto &prob : m_prob)
  {
    prob *= sumProbInverse;
  }
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::seedRandomEngine()
{
  size_t seed;
  if(m_useSeed)
  {
    seed = m_seed;
  }
  else
  {
    seed = size_t(std::chrono::system_clock::now().time_since_epoch().count());
  }
  m_gen.seed(seed);
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::countBranches()
{
  m_branches = {m_axiom};
  for(auto &rule : m_rules)
  {
    rule.m_numBranches = {};
    for(auto rhs : rule.m_RHS)
    {
      int numBranches = 0;
      for(size_t i=0; i<rhs.length(); i++)
      {
        if(rhs[i]=='[')
        {
          int bracketCount=0;
          size_t j=i+1;
          for(; j<rhs.length(); j++)
          {
            if(rhs[j]=='[')
            {
              bracketCount++;
            }
            if(rhs[j]==']')
            {
              if(bracketCount==0)
              {
                break;
              }
              bracketCount--;
            }
          }
          if(j==rhs.length())
          {
            continue;
          }

          std::string branch(rhs.begin()+int(i+1),rhs.begin()+int(j));
          //check that the branch contains at least one non-terminal
          if(std::regex_search(branch, std::regex(m_nonTerminals)))
          {
            numBranches++;
            //if the branch hasn't been added to m_branches already, then add it
            if(std::find(m_branches.begin(), m_branches.end(), branch) == m_branches.end())
            {
              m_branches.push_back(branch);
            }
          }
        }
      }
      rule.m_numBranches.push_back(numBranches);
    }
  }
}

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
          std::cerr<<"WARNING: unable to convert probability to float \n";
        }
        catch(std::out_of_range)
        {
          std::cerr<<"WARNING: unable to convert probability to float \n";
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
    //if '=' doesn't appear in the rules, display error message
    else
    {
      std::cerr<<"WARNING: excluding rule because no replacement command was given \n";
    }
  }
  //normalize all probabilities in the rules
  for(auto &rule : m_rules)
  {
    rule.normalizeProbabilities();
  }
  m_nonTerminals += "]+";
  //note we need to conclude m_nonTerminals before calling countBranches
  countBranches();
}

//----------------------------------------------------------------------------------------------------------------------

std::string LSystem::generateTreeString()
{
  std::string treeString = m_axiom;
  int numRules = int(m_rules.size());

  std::uniform_real_distribution<float> dist(0.0,1.0);

  if(numRules>0)
  {
    for(int i=0; i<m_generation; i++)
    {
      size_t ruleNum = size_t(i % numRules);
      std::string lhs = m_rules[ruleNum].m_LHS;
      std::vector<std::string> RHS = m_rules[ruleNum].m_RHS;
      std::vector<float> probabilities = m_rules[ruleNum].m_prob;

      //use boost method if there is only one rhs - much faster
      if(RHS.size()==1)
      {
        std::string rhs = RHS[0];
        boost::replace_all(rhs, "#", std::to_string(i+1));
        boost::replace_all(treeString, lhs, rhs);
      }

      //otherwise we want to apply each replacement individually based on probabilities
      else
      {
        size_t pos = treeString.find(lhs);
        size_t len = lhs.size();
        while(pos != std::string::npos)
        {
          float randNum = dist(m_gen);
          float count = 0;
          size_t j = 0;
          for( ; j<probabilities.size(); j++)
          {
            count += probabilities[j];
            if(count>=randNum)
            {
              break;
            }
          }
          std::string rhs = RHS[j];
          boost::replace_all(rhs, "#", std::to_string(i+1));
          treeString.replace(pos, len, rhs);
          pos = treeString.find(lhs, pos+rhs.size());
        }
      }
    }
  }
  return treeString;
}
