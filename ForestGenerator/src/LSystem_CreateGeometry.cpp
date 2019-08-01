//----------------------------------------------------------------------------------------------------------------------
/// @file LSystem_CreateGeometry.cpp
/// @brief implementation file for LSystem class createGeometry method
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

void LSystem::createGeometry()
{
  std::string treeString = generateTreeString();

  //std::cout<<treeString<<"\n\n";

  ngl::Vec3 dir(0,1,0);
  ngl::Vec3 right(1,0,0);
  //I am using an ngl::Mat4 matrix for now because there is a problem with the euler
  //method for ngl::Mat3, so I am setting the rotation for r4 with r4.euler, then
  //using the copy constructor to transfer that rotation to r3
  ngl::Mat4 r4;
  ngl::Mat3 r3;
  ngl::Mat4 t4;

  ngl::Vec3 lastVertex(0,0,0);
  GLshort lastIndex = 0;
  float stepSize = m_stepSize;
  float angle = m_angle;

  //paramVar will store the default value of each command, to be replaced by one
  //parsed from brackets by parseBrackets() if necessary
  float paramVar;
  size_t id, age;

  std::vector<GLshort> savedInd = {};
  std::vector<ngl::Vec3> savedVert = {};
  std::vector<ngl::Vec3> savedDir = {};
  std::vector<ngl::Vec3> savedRight = {};
  std::vector<float> savedStep = {};
  std::vector<float> savedAngle = {};

  Instance instance;
  Instance * currentInstance;
  std::vector<Instance *> savedInstance = {};

  std::vector<ngl::Vec3> * vertices;
  std::vector<GLshort> * indices;
  if(m_forestMode == false)
  {
    m_vertices = {lastVertex};
    m_indices = {};
    vertices = &m_vertices;
    indices = &m_indices;
  }
  else
  {
    lastIndex = GLshort(m_heroVertices.size());
    m_heroVertices.push_back(lastVertex);
    vertices = &m_heroVertices;
    indices = &m_heroIndices;
  }

  for(size_t i=0; i<treeString.size(); i++)
  {
    char c = treeString[i];
    switch(c)
    {
      //move forward
      case 'F':
      {
        indices->push_back(lastIndex);
        paramVar = stepSize;
        parseBrackets(treeString, i, paramVar);
        lastVertex += paramVar*dir;
        vertices->push_back(lastVertex);
        lastIndex = GLshort(vertices->size()-1);
        indices->push_back(lastIndex);
        break;
      }

      //start branch
      case '[':
      {
        savedInd.push_back(lastIndex);
        savedVert.push_back(lastVertex);
        savedDir.push_back(dir);
        savedRight.push_back(right);
        savedStep.push_back(stepSize);
        savedAngle.push_back(angle);
        break;
      }

      //end branch
      case ']':
      {
        if(savedInd.size()>0)
        {
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
        }
        break;
      }

      //roll clockwise
      case '/':
      {
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(paramVar, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
        break;
      }

      //roll anticlockwise
      case '\\':
      {
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(-paramVar, dir.m_x, dir.m_y, dir.m_z);
        r3 = r4;
        right = r3*right;
        break;
      }

      //pitch up
      case '&':
      {
        paramVar = angle;
        parseBrackets(treeString, i, paramVar);
        r4.euler(paramVar, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
        break;
      }

      //pitch down
      case '^':
      {
        paramVar = angle;

        parseBrackets(treeString, i, paramVar);
        r4.euler(-paramVar, right.m_x, right.m_y, right.m_z);
        r3 = r4;
        dir = r3*dir;
        break;
      }

      //scale step size
      case '\"':
      {
        paramVar = m_stepScale;
        parseBrackets(treeString, i, paramVar);
        stepSize *= paramVar;
        break;
      }

      //scale angle
      case ';':
      {
        paramVar = m_angleScale;
        parseBrackets(treeString, i, paramVar);
        angle *= paramVar;
        break;
      }

      //startInstance
      case '{':
      {
        parseInstanceBrackets(treeString, i, id, age);

        ngl::Vec3 k = right.cross(dir);
        ngl::Mat4 transform(right.m_x,      right.m_y,      right.m_z,      0,
                            dir.m_x,        dir.m_y,        dir.m_z,        0,
                            k.m_x,          k.m_y,          k.m_z,          0,
                            lastVertex.m_x, lastVertex.m_y, lastVertex.m_z, 1);

        instance = Instance(transform);
        instance.m_instanceStart = indices->size();//&(indices->back()); //except maybe should be &(indices->back())+1?
        if(m_instanceCache[id][age].size()<=size_t(m_maxInstancePerLevel/(age+1)))
        {
          m_instanceCache[id][age].push_back(instance);
          currentInstance = &m_instanceCache[id][age].back();
        }
        else
        {
          currentInstance = &instance;
        }

        savedInstance.push_back(currentInstance);
        break;
      }

      //stopInstance
      case '}':
      {
        currentInstance->m_instanceEnd = indices->size();
        savedInstance.pop_back();
        if(savedInstance.size()>0)
        {
          currentInstance = savedInstance.back();
        }
        break;
      }

      //getInstance
      case '<':
      {
        parseInstanceBrackets(treeString, i, id, age);

        t4.translate(lastVertex.m_x, lastVertex.m_y, lastVertex.m_z);

        ngl::Vec3 k = right.cross(dir);
        ngl::Mat4 transform(right.m_x,      right.m_y,      right.m_z,      0,
                            dir.m_x,        dir.m_y,        dir.m_z,        0,
                            k.m_x,          k.m_y,          k.m_z,          0,
                            lastVertex.m_x, lastVertex.m_y, lastVertex.m_z, 1);

        for(auto instance : savedInstance)
        {
          instance->m_exitPoints.push_back(Instance::ExitPoint(id, age, instance->m_transform.inverse()*transform));
        }

        if(m_instanceCache[id][age].size()==0)
        {
          instance = Instance(transform);
          instance.m_instanceStart = indices->size();//&(indices->back());
          m_instanceCache[id][age].push_back(instance);
          currentInstance = &m_instanceCache[id][age].back();
          savedInstance.push_back(currentInstance);
        }
        else
        {
          skipToNextChevron(treeString,i);
        }

        break;
      }

      case '>':
      {
        //note that assuming > doesn't appear in any rules, we will only reach this case if we are using the corresponding < to make an instance
        currentInstance->m_instanceEnd = indices->size();
        savedInstance.pop_back();
        if(savedInstance.size()>0)
        {
          currentInstance = savedInstance.back();
        }
        break;
      }

      default:
      {
        break;
      }
    }
  }
  if(m_parameterError)
  {
    std::cerr<<"WARNING: unable to parse one or more parameters \n";
    m_parameterError = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::parseBrackets(const std::string &_treeString, size_t &_i, float &_paramVar)
{
  if(_i+1<_treeString.length() && _treeString.at(_i+1)=='(')
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
        _paramVar = std::stof(parameter);
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

//----------------------------------------------------------------------------------------------------------------------

void LSystem::parseInstanceBrackets(const std::string &_treeString, size_t &_i, size_t &_id, size_t &_age)
{
  //don't need the outer if clause that parseBrackets() has because <, { are guaranteed to be followed by (
  size_t j=_i+2;
  for( ; j<_treeString.size(); j++)
  {
    if(_treeString[j]==',')
    {
      break;
    }
  }
  std::string id = _treeString.substr(_i+2, j-_i-2);
  _id = size_t(std::stoi(id));
  _i=j;

  for( ; j<_treeString.size(); j++)
  {
    if(_treeString[j]==')')
    {
      break;
    }
  }
  std::string age = _treeString.substr(_i+1, j-_i-1);
  _age = size_t(std::stoi(age));
  _i=j;
}

//----------------------------------------------------------------------------------------------------------------------

void LSystem::skipToNextChevron(const std::string &_treeString, size_t &_i)
{
  int chevronCount = 0;
  size_t j=_i+1;
  for(; j<_treeString.length(); j++)
  {
    if(_treeString[j]=='<')
    {
      chevronCount++;
    }
    if(_treeString[j]=='>')
    {
      if(chevronCount==0)
      {
        break;
      }
      chevronCount--;
    }
  }
  _i=j;
}
