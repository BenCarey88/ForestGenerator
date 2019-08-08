//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainData.cpp
/// @brief implementation file for TerrainData class
//----------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <cmath>
#include <algorithm>
#include "TerrainData.h"

//----------------------------------------------------------------------------------------------------------------------
///CONSTRUCTOR FUNCTION
//----------------------------------------------------------------------------------------------------------------------
//On construction, we fill m_vertices from the heightmap values, then assign all relevant attributes to the vertices

TerrainData::TerrainData(TerrainGenerator &_terrainGen) :
    m_dimension(_terrainGen.m_dimension), m_heightMap(_terrainGen.m_heightMap), m_scale(_terrainGen.m_scale),
    m_normals(_terrainGen.m_normals), m_tangents(_terrainGen.m_tangents), m_bitangents(_terrainGen.m_bitangents)
{
  //Fill m_vertices
  size_t dimension = size_t(m_dimension);
  m_vertices.resize(9,Vertex(0,0,0,m_dimension, m_scale, {0,1,0}, {0,1,0}, {0,1,0}));
  m_vertices[0] = getVertex(dimension*dimension - dimension);          //SW corner
  m_vertices[1] = getVertex(dimension*dimension - 1);                  //SE corner
  m_vertices[2] = getVertex(dimension - 1);                            //NE corner
  m_vertices[3] = getVertex(0);                                        //NW corner
  m_vertices[4] = getVertex((dimension*dimension - 1)/2);              //centre
  m_vertices[5] = getVertex((dimension*dimension - dimension)/2);      //left middle
  m_vertices[6] = getVertex(dimension*dimension - (dimension+1)/2);    //bottom middle
  m_vertices[7] = getVertex((dimension*dimension + dimension - 2)/2);  //right middle
  m_vertices[8] = getVertex((dimension - 1)/2);                        //top middle

  createVerticesWQT((dimension*dimension - 1)/2,             4,3,(m_dimension-1)/4); //fill from root of white quadtree
  createVerticesBQT((dimension*dimension - dimension)/2,     5,4,(m_dimension-1)/4); //fill from root of first black quadtree
  createVerticesBQT(dimension*dimension - (dimension+1)/2,   6,4,(m_dimension-1)/4); //fill from root of second black quadtree
  createVerticesBQT((dimension*dimension + dimension - 2)/2, 7,4,(m_dimension-1)/4); //fill from root of third black quadtree
  createVerticesBQT((dimension - 1)/2,                       8,4,(m_dimension-1)/4); //fill from root of fourth black quadtree

  //Assign Children
  m_verticesArrangedByGraphLevel.resize(size_t(m_maxRefinementLevel),{});
  m_verticesArrangedByGraphLevel[size_t(m_maxRefinementLevel)-2]={5,6,7,8};
  m_verticesArrangedByGraphLevel[size_t(m_maxRefinementLevel)-1]={4};
  assignChildren(4,5,m_maxRefinementLevel-2);
  assignChildren(4,6,m_maxRefinementLevel-2);
  assignChildren(4,7,m_maxRefinementLevel-2);
  assignChildren(4,8,m_maxRefinementLevel-2);
  m_vertices[4].childList = {5,6,7,8};

  //Assign Bounding Sphere Radii
  assignRadius();

  //Assign Delta Values;
  assignDelta(0,4,5,2);
  assignDelta(1,4,6,2);
  assignDelta(2,4,7,2);
  assignDelta(3,4,8,2);

  //Assign Augmented Delta Values;
  assignAugmentedDelta();
}

//----------------------------------------------------------------------------------------------------------------------
///VERTEX METHODS
//----------------------------------------------------------------------------------------------------------------------

TerrainData::Vertex::Vertex(int _x, int _y, float _z, int _dimension, float _scale,
                            ngl::Vec3 _normal, ngl::Vec3 _tangent, ngl::Vec3 _bitangent)
{
  originalX = _x;
  originalY = _y;
  originalZ = _z;
  normal = _normal;
  tangent = _tangent;
  bitangent = _bitangent;
  //for scene coordinates, centre the grid at (0,0) then scale
  sceneX = (float(_x-_dimension/2))*_scale;
  sceneY = (float(_y-_dimension/2))*_scale;
  sceneZ = _z;//*_scale;
}
float TerrainData::Vertex::distanceTo(Vertex v) const
{
  return std::sqrt((v.sceneX-sceneX)*(v.sceneX-sceneX) +
                   (v.sceneY-sceneY)*(v.sceneY-sceneY) +
                   (v.sceneZ-sceneZ)*(v.sceneZ-sceneZ));
}

//----------------------------------------------------------------------------------------------------------------------
///PUBLIC MEMBER FUNCTION: MESH REFINE
//----------------------------------------------------------------------------------------------------------------------
/// @ref pseudocode from 'Visualisation of Large Terrains Made Easy', Lindstrom and Pascucci, 2001
/// basically this method starts at the root of the DAG and then moves through the DAG, checking at each stage
/// whether the current vertex should be active based on the camera view and tolerance

void TerrainData::meshRefine(ngl::Vec3 _cameraPos, float _tolerance, float _lambda)
{
  m_indices = {};
  m_parity = 0;
  m_indices.push_back(0);
  m_indices.push_back(0);
  submeshRefine(4,6,1, _cameraPos, _tolerance, _lambda);
  tstripAppend(1,0);
  submeshRefine(4,7,1, _cameraPos, _tolerance, _lambda);
  tstripAppend(2,0);
  submeshRefine(4,8,1, _cameraPos, _tolerance, _lambda);
  tstripAppend(3,0);
  submeshRefine(4,5,1, _cameraPos, _tolerance, _lambda);
  m_indices.push_back(0);

  fillVerticesAndIndicesForRendering();
}

void TerrainData::fillVerticesAndIndicesForRendering()
{
  m_vertsToBeRendered = {};
  m_indicesToBeRendered = {};
  m_normalsToBeRendered = {};
  m_tangentsToBeRendered = {};
  m_bitangentsToBeRendered = {};
  m_UVsToBeRendered = {};
  for(auto &vert : m_vertices)
  {
    //Note that I used a different convention in my ASE project and had z as the vertical axis,
    //but for this project the z-coordinate is depth, in keeping with openGL conventions
    //hence I have swapped z and y below
    m_vertsToBeRendered.push_back(ngl::Vec3(vert.sceneX, vert.sceneZ, vert.sceneY));
    m_normalsToBeRendered.push_back(vert.normal);
    m_tangentsToBeRendered.push_back(vert.tangent);
    m_bitangentsToBeRendered.push_back(vert.bitangent);

    int size = m_dimension/m_UVRepeat;
    float U = float(vert.originalX % size) / size;
    float V = float(vert.originalY % size) / size;
    m_UVsToBeRendered.push_back(ngl::Vec2(U,V));
  }
  for(auto ind : m_indices)
  {
    m_indicesToBeRendered.push_back(GLuint(ind));
  }
}

//----------------------------------------------------------------------------------------------------------------------
///PUBLIC MEMBER FUNCTIONS: BASIC
//----------------------------------------------------------------------------------------------------------------------


int TerrainData::getX(const size_t _index) const
{
  return int(_index)%m_dimension;
}

int TerrainData::getY(const size_t _index) const
{
  return int(_index)/m_dimension;
}

size_t TerrainData::getHeightMapIndex(const int _x, const int _y) const
{
  return size_t(_y*m_dimension+_x);
}

TerrainData::Vertex TerrainData::getVertex(const size_t _index) const
{
  return Vertex(getX(_index), getY(_index), m_heightMap[_index], m_dimension, m_scale,
                m_normals[_index], m_tangents[_index], m_bitangents[_index]);
}


//----------------------------------------------------------------------------------------------------------------------
///PUBLIC MEMBER FUNCTIONS: CREATING VERTICES
//----------------------------------------------------------------------------------------------------------------------

void TerrainData::createVerticesWQT(size_t _QTParentHeightMapIndex, size_t _QTParentVerticesIndex,
                                    int _refinementLevel, int _distance)
{
  if(_refinementLevel<m_maxRefinementLevel)
  {
    int x = getX(_QTParentHeightMapIndex);
    int y = getY(_QTParentHeightMapIndex);

    size_t verticesIndexChild1 = 4 * _QTParentVerticesIndex - 7;
    size_t verticesIndexChild2 = verticesIndexChild1 + 1;
    size_t verticesIndexChild3 = verticesIndexChild2 + 1;
    size_t verticesIndexChild4 = verticesIndexChild3 + 1;
    if(m_vertices.size()<verticesIndexChild4+1)
    {
      m_vertices.resize(verticesIndexChild4+1,Vertex(0,0,0,m_dimension,m_scale,{0,1,0},{0,1,0},{0,1,0}));
    }

    size_t heightMapIndexChild1 = getHeightMapIndex(x - _distance, y + _distance);
    size_t heightMapIndexChild2 = getHeightMapIndex(x + _distance, y + _distance);
    size_t heightMapIndexChild3 = getHeightMapIndex(x + _distance, y - _distance);
    size_t heightMapIndexChild4 = getHeightMapIndex(x - _distance, y - _distance);

    m_vertices[verticesIndexChild1] = getVertex(heightMapIndexChild1);
    m_vertices[verticesIndexChild2] = getVertex(heightMapIndexChild2);
    m_vertices[verticesIndexChild3] = getVertex(heightMapIndexChild3);
    m_vertices[verticesIndexChild4] = getVertex(heightMapIndexChild4);

    createVerticesWQT(heightMapIndexChild1, verticesIndexChild1,
                                   _refinementLevel + 2, _distance/2);
    createVerticesWQT(heightMapIndexChild2, verticesIndexChild2,
                                   _refinementLevel + 2, _distance/2);
    createVerticesWQT(heightMapIndexChild3, verticesIndexChild3,
                                   _refinementLevel + 2, _distance/2);
    createVerticesWQT(heightMapIndexChild4, verticesIndexChild4,
                                   _refinementLevel + 2, _distance/2);
  }
}

void TerrainData::createVerticesBQT(size_t _QTParentHeightMapIndex, size_t _QTParentVerticesIndex,
                                    int _refinementLevel, int _distance)
{
  if(_refinementLevel<=m_maxRefinementLevel)
  {
    int x = getX(_QTParentHeightMapIndex);
    int y = getY(_QTParentHeightMapIndex);

    size_t verticesIndexChild1 = 4 * _QTParentVerticesIndex - 7;
    size_t verticesIndexChild2 = verticesIndexChild1 + 1;
    size_t verticesIndexChild3 = verticesIndexChild2 + 1;
    size_t verticesIndexChild4 = verticesIndexChild3 + 1;
    if(m_vertices.size()<verticesIndexChild4+1)
    {
      m_vertices.resize(verticesIndexChild4+1,Vertex(0,0,0,m_dimension, m_scale, {0,1,0},{0,1,0},{0,1,0}));
    }

    if(y>0)
    {
      size_t heightMapIndexChild1 = getHeightMapIndex(x, y - _distance);
      m_vertices[verticesIndexChild1] = getVertex(heightMapIndexChild1);
      createVerticesBQT(heightMapIndexChild1, verticesIndexChild1,
                                     _refinementLevel + 2, _distance/2);
    }
    if(x<m_dimension-1)
    {
      size_t heightMapIndexChild2 = getHeightMapIndex(x + _distance, y);
      m_vertices[verticesIndexChild2] = getVertex(heightMapIndexChild2);
      createVerticesBQT(heightMapIndexChild2, verticesIndexChild2,
                                     _refinementLevel + 2, _distance/2);
    }
    if(y<m_dimension-1)
    {
      size_t heightMapIndexChild3 = getHeightMapIndex(x, y + _distance);
      m_vertices[verticesIndexChild3] = getVertex(heightMapIndexChild3);
      createVerticesBQT(heightMapIndexChild3, verticesIndexChild3,
                                     _refinementLevel + 2, _distance/2);
    }
    if(x>0)
    {
      size_t heightMapIndexChild4 = getHeightMapIndex(x - _distance, y);
      m_vertices[verticesIndexChild4] = getVertex(heightMapIndexChild4);
      createVerticesBQT(heightMapIndexChild4, verticesIndexChild4,
                                     _refinementLevel + 2, _distance/2);
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
///PUBLIC MEMBER FUNCTIONS: ASSIGNING CHILDREN
//----------------------------------------------------------------------------------------------------------------------

size_t TerrainData::getChild1(size_t _QTParent, size_t _DAGParent) const
{
  size_t child1 = 4 * _QTParent + (2 * _QTParent + _DAGParent - 6) % 4 - 7;
  return child1;
}

size_t TerrainData::getChild2(size_t _QTParent, size_t _DAGParent) const
{
  size_t child2 = 4 * _QTParent + (2 * _QTParent + _DAGParent - 5) % 4 - 7;
  return child2;
}

void TerrainData::assignChildren(size_t _DAGParent, size_t _currentVertex, int _refinementLevel)
{
  if(_refinementLevel != 0)
  {
    size_t child1 = getChild1(_DAGParent,_currentVertex);
    size_t child2 = getChild2(_DAGParent,_currentVertex);
    m_vertices[_currentVertex].childList.push_back(child1);
    m_vertices[_currentVertex].childList.push_back(child2);
    assignChildren(_currentVertex, child1, _refinementLevel-1);
    assignChildren(_currentVertex, child2, _refinementLevel-1);

    m_verticesArrangedByGraphLevel[size_t(_refinementLevel)-1].push_back(child1);
    m_verticesArrangedByGraphLevel[size_t(_refinementLevel)-1].push_back(child2);
  }
}

//----------------------------------------------------------------------------------------------------------------------
///PUBLIC METHODS: ASSIGN OTHER VERTEX MEMBER VARIABLES
//----------------------------------------------------------------------------------------------------------------------

void TerrainData::assignRadius()
{
  for (auto list : m_verticesArrangedByGraphLevel)
  {
    for (auto i : list)
    {
      Vertex v = m_vertices[i];
      for (auto j : v.childList)
      {
        Vertex w = m_vertices[j];
        if(m_vertices[i].radius < v.distanceTo(w) + w.radius)
        {
          m_vertices[i].radius = v.distanceTo(w) + w.radius;
        }
      }
    }
  }
}

void TerrainData::assignDelta(size_t _DAGGrandParent, size_t _DAGParent, size_t _currentVertex, int _refinementLevel)
{
  if(_refinementLevel <= m_maxRefinementLevel)
  {
    Vertex B = m_vertices[_currentVertex];
    Vertex A = m_vertices[_DAGGrandParent];
    Vertex C = getVertex(getHeightMapIndex(2*B.originalX-A.originalX, 2*B.originalY - A.originalY));
    m_vertices[_currentVertex].delta = std::abs(2*B.sceneZ - C.sceneZ - A.sceneZ);
    assignDelta(_DAGParent,_currentVertex,getChild1(_DAGParent,_currentVertex),_refinementLevel+1);
    assignDelta(_DAGParent,_currentVertex,getChild2(_DAGParent,_currentVertex),_refinementLevel+1);
  }
}

void TerrainData::assignAugmentedDelta()
{
  for (auto list : m_verticesArrangedByGraphLevel)
  {
    for (auto i : list)
    {
      m_vertices[i].augmentedDelta = m_vertices[i].delta;
      for (auto j : m_vertices[i].childList)
      {
        Vertex w = m_vertices[j];
        if(m_vertices[i].augmentedDelta < w.augmentedDelta)
        {
          m_vertices[i].augmentedDelta = w.augmentedDelta;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
///PUBLIC METHODS CALLED BY MESH REFINE
//----------------------------------------------------------------------------------------------------------------------
///@ref based on pseudocode from Lindstrom and Pascucci, 2001

void TerrainData::submeshRefine(size_t _currentVertex, size_t _DAGChildVertex, int _refinementLevel,
                                ngl::Vec3 _cameraPos, float _tolerance, float _lambda)
{
  if(_refinementLevel <= m_maxRefinementLevel &&
     isActive(_currentVertex, _cameraPos, _tolerance, _lambda))
  {
    submeshRefine(_DAGChildVertex,
                  4 * _currentVertex + (2 * _currentVertex + _DAGChildVertex - 6) % 4 - 7,
                  _refinementLevel + 1, _cameraPos, _tolerance, _lambda);
    tstripAppend(_currentVertex, _refinementLevel % 2);
    submeshRefine(_DAGChildVertex,
                  4 * _currentVertex + (2 * _currentVertex + _DAGChildVertex - 5) % 4 - 7,
                  _refinementLevel + 1, _cameraPos, _tolerance, _lambda);
    }
}

void TerrainData::tstripAppend(size_t _vertex, int _parity)
{
  size_t l = m_indices.size();
  if (m_indices[l-1] != _vertex && m_indices[l-2] != _vertex)
  {
    if(_parity != m_parity)
    {
      m_parity = _parity;
    }
    else
    {
      m_indices.push_back(m_indices[l-2]);
    }
    m_indices.push_back(_vertex);
    }
}

bool TerrainData::isActive(size_t _vertex, ngl::Vec3 _cameraPos, float _tolerance, float _lambda) const
{
  float mu = _lambda/_tolerance;
  Vertex v = m_vertices[_vertex];
  return (mu * v.augmentedDelta + v.radius)*(mu * v.augmentedDelta + v.radius) >
                                        (v.sceneX-_cameraPos.m_x)*(v.sceneX-_cameraPos.m_x)
                                      + (v.sceneY-_cameraPos.m_y)*(v.sceneY-_cameraPos.m_y)
                                      + (v.sceneZ-_cameraPos.m_z)*(v.sceneZ-_cameraPos.m_z);
}
