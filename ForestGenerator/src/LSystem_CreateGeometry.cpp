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
  ngl::Vec3 dir(0,1,0);
  ngl::Vec3 right(1,0,0);
  //I am using an ngl::Mat4 matrix for now because there is a problem with the euler
  //method for ngl::Mat3, so I am setting the rotation for r4 with r4.euler, then
  //using the copy constructor to transfer that rotation to r3
  ngl::Mat4 r4;
  ngl::Mat3 r3;
  ngl::Mat4 t4;
  std::string treeString = generateTreeString();

  auto start = std::chrono::high_resolution_clock::now();

  ngl::Vec3 lastVertex(0,0,0);
  GLshort lastIndex = 0;
  float stepSize = m_stepSize;
  float angle = m_angle;

  //Instance currentInstance;

  //paramVar will store the default value of each command, to be replaced by one
  //parsed from brackets by parseBrackets() if necessary
  float paramVar;
  size_t id, age;

  std::vector<ngl::Vec3> savedVert = {};
  std::vector<GLshort> savedInd = {};
  std::vector<ngl::Vec3> savedDir = {};
  std::vector<ngl::Vec3> savedRight = {};
  std::vector<float> savedStep = {};
  std::vector<float> savedAngle = {};

  Instance * currentInstance;
  std::vector<Instance> savedInstance = {};

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
        lastIndex = GLshort(m_vertices.size()-1);
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

        //BETTER VERSION: KEEP TRACK OF THIS FROM THE TOP WITH CONTANTLY UPDATING TRANSFORM MATRIX
        ngl::Vec3 axis = ngl::Vec3(0,1,0).cross(dir);
        float angle = float(acos(double(ngl::Vec3(0,1,0).dot(dir)))*180/M_PI);
        r4.euler(angle, axis.m_x, axis.m_y, axis.m_z);
        t4.translate(lastVertex.m_x, lastVertex.m_y, lastVertex.m_z);

        Instance instance(t4*r4);
        instance.m_instanceStart = &(indices->back());
        savedInstance.push_back(instance);
        currentInstance = &instance;
        m_instanceCache[id][age].push_back(instance);
        break;
      }

      //stopInstance
      case '}':
      {
        currentInstance->m_instanceEnd = &(*indices)[indices->size()-1];
        savedInstance.pop_back();
        currentInstance = &(savedInstance.back());
        break;
      }

      //getInstance
      case '@':
      {
        parseInstanceBrackets(treeString, i, id, age);

        ngl::Vec3 axis = ngl::Vec3(0,1,0).cross(dir);
        float angle = float(acos(double(ngl::Vec3(0,1,0).dot(dir)))*180/M_PI);
        r4.euler(angle, axis.m_x, axis.m_y, axis.m_z);
        t4.translate(lastVertex.m_x, lastVertex.m_y, lastVertex.m_z);

        currentInstance->m_exitPoints.push_back(Instance::ExitPoint(id,age,t4*r4));

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

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  //std::cout << "\ncreateGeometry() Elapsed time: " << elapsed.count() << " s\n";
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
  //don't need the outer if clause that parseBrackets() has because @, { and } are guaranteed to be followed by (
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