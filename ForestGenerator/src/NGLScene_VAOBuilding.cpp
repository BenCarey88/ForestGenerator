//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_Slots.cpp
/// @brief implementation file for NGLScene VAO building methods
//----------------------------------------------------------------------------------------------------------------------

#include <ngl/NGLInit.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>
#include <ngl/SimpleVAO.h>
#include <ngl/Texture.h>
#include "InstanceCacheVAO.h"
#include "NGLScene.h"
#include "Camera.h"
#include "PrintFunctions.h"

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::addBufferToBoundVAO(size_t _bufferSize, const GLvoid * _bufferData)
{
  GLuint bufferID=0;
  glGenBuffers(1, &bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  glBufferData(GL_ARRAY_BUFFER,
               _bufferSize,
               _bufferData,
               GL_STATIC_DRAW);
}

//------------------------------------------------------------------------------------------------------------------------
/// Generic methods for the 2 different types of VAO: Simple Index and Instance Cache
//------------------------------------------------------------------------------------------------------------------------

template<class dataType>
void NGLScene::buildSimpleIndexVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                                   std::vector<dataType> &_indices, GLenum _mode, GLenum _indexType)
{
  // create a vao using _mode
  _vao=ngl::VAOFactory::createVAO(ngl::simpleIndexVAO,_mode);
  _vao->bind();

  // set our data for the VAO
  // set our data for the VAO:
  //    (1) vertexBufferSize, (2) vertexBufferStart,
  //    (3) indexBufferSize, (4) indexBufferStart,
  //    (5) type of indices
  _vao->setData(ngl::SimpleIndexVAO::VertexData(
                                                  sizeof(ngl::Vec3)*_vertices.size(),
                                                  _vertices[0].m_x,
                                                  uint(_indices.size()),
                                                  &_indices[0],
                                                  _indexType));
  // data is 12 bytes apart (=sizeof(Vec3))
  _vao->setVertexAttributePointer(0,3,GL_FLOAT,12,0);
  _vao->setNumIndices(_indices.size());
  _vao->unbind();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildInstanceCacheVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, LSystem &_treeType,
                                     Instance &_instance, std::vector<ngl::Mat4> &_transforms)
{
  // create a vao using GL_LINES
  _vao=ngl::VAOFactory::createVAO("instanceCacheVAO",GL_LINES);
  _vao->bind();
  // set our data for the VAO:
  //    (1) vertexBufferSize, (2) vertexBufferStart,
  //    (3) indexBufferSize, (4) indexBufferStart,
  //    (5) transformBufferSize, (6) transformBufferStart
  _vao->setData(ngl::InstanceCacheVAO::VertexData(
                       sizeof(ngl::Vec3)*_treeType.m_heroVertices.size(),
                       _treeType.m_heroVertices[0].m_x,
                       uint(_instance.m_instanceEnd-_instance.m_instanceStart),
                       &_treeType.m_heroIndices[_instance.m_instanceStart],
                       uint(_transforms.size()),
                       &_transforms[0]));
  // set number of indices to length of current instance
  _vao->setNumIndices(_instance.m_instanceEnd-_instance.m_instanceStart);

  addBufferToBoundVAO(sizeof(ngl::Vec3)*_treeType.m_heroRightVectors.size(),
                      &_treeType.m_heroRightVectors[0].m_x);
  _vao->setVertexAttributePointer(5,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*_treeType.m_heroThicknessValues.size(),
                      &_treeType.m_heroThicknessValues[0]);
  _vao->setVertexAttributePointer(6,1,GL_FLOAT,4,0);

  _vao->unbind();
}

//------------------------------------------------------------------------------------------------------------------------
/// Specific methods for each thing we need to draw: grid, tree, terrain and forest
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildGridVAO()
{
  buildSimpleIndexVAO(m_gridVAO, m_grid.m_vertices, m_grid.m_indices,
                  GL_LINES, GL_UNSIGNED_SHORT);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildTreeVAO(size_t _treeNum)
{
  buildSimpleIndexVAO(m_treeVAOs[_treeNum],
                  m_LSystems[_treeNum].m_vertices,
                  m_LSystems[_treeNum].m_indices,
                  GL_LINES, GL_UNSIGNED_SHORT);

  m_treeVAOs[_treeNum]->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_LSystems[_treeNum].m_rightVectors.size(),
                      &m_LSystems[_treeNum].m_rightVectors[0]);
  m_treeVAOs[_treeNum]->setVertexAttributePointer(1,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_LSystems[_treeNum].m_thicknessValues.size(),
                      &m_LSystems[_treeNum].m_thicknessValues[0]);
  m_treeVAOs[_treeNum]->setVertexAttributePointer(2,1,GL_FLOAT,4,0);
  m_treeVAOs[_treeNum]->unbind();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildTerrainVAO()
{
  //note that we need to use GLuints for the terrain because the data can be too large for GLushorts
  buildSimpleIndexVAO(m_terrainVAO,
                  m_terrain.m_vertsToBeRendered,
                  m_terrain.m_indicesToBeRendered,
                  GL_TRIANGLE_STRIP, GL_UNSIGNED_INT);
  m_terrainVAO->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_normalsToBeRendered.size(),
                      &m_terrain.m_normalsToBeRendered[0]);
  m_terrainVAO->setVertexAttributePointer(1,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_tangentsToBeRendered.size(),
                      &m_terrain.m_tangentsToBeRendered[0]);
  m_terrainVAO->setVertexAttributePointer(2,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_bitangentsToBeRendered.size(),
                      &m_terrain.m_bitangentsToBeRendered[0]);
  m_terrainVAO->setVertexAttributePointer(3,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec2)*m_terrain.m_UVsToBeRendered.size(),
                      &m_terrain.m_UVsToBeRendered[0]);
  m_terrainVAO->setVertexAttributePointer(4,2,GL_FLOAT,8,0);
  m_terrainVAO->unbind();

}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildForestVAO()
{
  for(size_t t=0; t<m_forest.m_treeTypes.size(); t++)
  {
    LSystem &treeType = m_forest.m_treeTypes[t];
    CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestVAOs[t], instanceCache)
    FOR_EACH_ELEMENT(m_forestVAOs[t],
                     buildInstanceCacheVAO(m_forestVAOs[t][ID][AGE][INDEX],
                                           treeType,
                                           instanceCache[ID][AGE][INDEX],
                                           m_forest.m_transformCache[t][ID][AGE][INDEX]))
  }
}
