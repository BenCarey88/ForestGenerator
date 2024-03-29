//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_VAOBuilding.cpp
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

void NGLScene::addBufferToBoundVAO(size_t _bufferSize, const GLvoid * _bufferData, GLuint &_bufferID)
{
  glDeleteBuffers(1, &_bufferID);

  glGenBuffers(1, &_bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
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

void NGLScene::buildInstanceCacheVAO(std::unique_ptr<ngl::AbstractVAO> &_vao, std::vector<ngl::Vec3> &_vertices,
                                     std::vector<GLshort> &_indices, std::vector<ngl::Mat4> &_transforms,
                                     size_t _instanceStart, size_t _instanceEnd, GLenum _mode)
{
  // create a vao using _mode
  _vao=ngl::VAOFactory::createVAO("instanceCacheVAO",_mode);
  _vao->bind();

  // set our data for the VAO:
  //    (1) vertexBufferSize, (2) vertexBufferStart,
  //    (3) indexBufferSize, (4) indexBufferStart,
  //    (5) transformBufferSize, (6) transformBufferStart
  _vao->setData(ngl::InstanceCacheVAO::VertexData(
                       sizeof(ngl::Vec3)*_vertices.size(),
                       _vertices[0].m_x,
                       uint(_instanceEnd - _instanceStart),
                       &_indices[_instanceStart],
                       uint(_transforms.size()),
                       &_transforms[0].m_00));

  // set number of indices to length of current instance
  _vao->setNumIndices(_instanceEnd - _instanceStart);
  _vao->unbind();
}

//------------------------------------------------------------------------------------------------------------------------
/// Specific methods for each thing we need to draw: grid, tree, leaves, polygons, terrain and forest
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
                      &m_LSystems[_treeNum].m_rightVectors[0].m_x,
                      m_treeRightBuffers[_treeNum]);
  m_treeVAOs[_treeNum]->setVertexAttributePointer(1,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(float)*m_LSystems[_treeNum].m_thicknessValues.size(),
                      &m_LSystems[_treeNum].m_thicknessValues[0],
                      m_treeThicknessBuffers[_treeNum]);
  m_treeVAOs[_treeNum]->setVertexAttributePointer(2,1,GL_FLOAT,4,0);
  m_treeVAOs[_treeNum]->unbind();
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildLeafVAO(size_t _treeNum)
{
  buildSimpleIndexVAO(m_leafVAOs[_treeNum],
                  m_LSystems[_treeNum].m_leafVertices,
                  m_LSystems[_treeNum].m_leafIndices,
                  GL_POINTS, GL_UNSIGNED_SHORT);

  m_leafVAOs[_treeNum]->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_LSystems[_treeNum].m_leafDirections.size(),
                      &m_LSystems[_treeNum].m_leafDirections[0].m_x,
                      m_treeLeafDirectionBuffers[_treeNum]);
  m_leafVAOs[_treeNum]->setVertexAttributePointer(1,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_LSystems[_treeNum].m_leafRightVectors.size(),
                      &m_LSystems[_treeNum].m_leafRightVectors[0].m_x,
                      m_treeLeafRightBuffers[_treeNum]);
  m_leafVAOs[_treeNum]->setVertexAttributePointer(2,3,GL_FLOAT,12,0);
  m_leafVAOs[_treeNum]->unbind();
}
//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildPolygonVAO(size_t _treeNum)
{
  buildSimpleIndexVAO(m_polygonVAOs[_treeNum],
                  m_LSystems[_treeNum].m_polygonVertices,
                  m_LSystems[_treeNum].m_polygonIndices,
                  GL_TRIANGLES, GL_UNSIGNED_SHORT);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildTerrainVAO()
{
  //note that we need to use GLuints for the terrain because the data can get too large for GLushorts
  buildSimpleIndexVAO(m_terrainVAO,
                  m_terrain.m_vertsToBeRendered,
                  m_terrain.m_indicesToBeRendered,
                  GL_TRIANGLE_STRIP, GL_UNSIGNED_INT);
  m_terrainVAO->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_normalsToBeRendered.size(),
                      &m_terrain.m_normalsToBeRendered[0].m_x,
                      m_terrainNormalBuffer);
  m_terrainVAO->setVertexAttributePointer(1,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_tangentsToBeRendered.size(),
                      &m_terrain.m_tangentsToBeRendered[0].m_x,
                      m_terrainTangentBuffer);
  m_terrainVAO->setVertexAttributePointer(2,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*m_terrain.m_bitangentsToBeRendered.size(),
                      &m_terrain.m_bitangentsToBeRendered[0].m_x,
                      m_terrainBitangentBuffer);
  m_terrainVAO->setVertexAttributePointer(3,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec2)*m_terrain.m_UVsToBeRendered.size(),
                      &m_terrain.m_UVsToBeRendered[0].m_x,
                      m_terrainUVBuffer);
  m_terrainVAO->setVertexAttributePointer(4,2,GL_FLOAT,8,0);
  m_terrainVAO->unbind();

}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildForestVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest)
{
  Forest *forest;
  std::unique_ptr<ngl::AbstractVAO> *vao;
  GLuint *rightBuffer;
  GLuint *thicknessBuffer;
  if(_usePaintedForest)
  {
    forest = &m_paintedForest;
    vao = &m_paintedForestVAOs[_treeNum][_id][_age][_index];
    rightBuffer = &m_paintedForestRightBuffers[_treeNum][_id][_age][_index];
    thicknessBuffer = &m_paintedForestThicknessBuffers[_treeNum][_id][_age][_index];
  }
  else
  {
    forest = &m_scatteredForest;
    vao = &m_forestVAOs[_treeNum][_id][_age][_index];
    rightBuffer = &m_forestRightBuffers[_treeNum][_id][_age][_index];
    thicknessBuffer = &m_forestThicknessBuffers[_treeNum][_id][_age][_index];
  }

  LSystem &treeType = forest->m_treeTypes[_treeNum];
  CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;
  //std::unique_ptr<ngl::AbstractVAO> &vao = m_forestVAOs[_treeNum][_id][_age][_index];

  buildInstanceCacheVAO(*vao,
                        treeType.m_heroVertices,
                        treeType.m_heroIndices,
                        forest->m_transformCache[_treeNum][_id][_age][_index],
                        instanceCache[_id][_age][_index].m_instanceStart,
                        instanceCache[_id][_age][_index].m_instanceEnd,
                        GL_LINES);

  (*vao)->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*treeType.m_heroRightVectors.size(),
                      &treeType.m_heroRightVectors[0].m_x, *rightBuffer);
  (*vao)->setVertexAttributePointer(5,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(float)*treeType.m_heroThicknessValues.size(),
                      &treeType.m_heroThicknessValues[0], *thicknessBuffer);
  (*vao)->setVertexAttributePointer(6,1,GL_FLOAT,4,0);
  (*vao)->unbind();

}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildForestLeafVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest)
{
  Forest *forest;
  std::unique_ptr<ngl::AbstractVAO> *vao;
  GLuint *dirBuffer;
  GLuint *rightBuffer;
  if(_usePaintedForest)
  {
    forest = &m_paintedForest;
    vao = &m_paintedForestLeafVAOs[_treeNum][_id][_age][_index];
    dirBuffer = &m_paintedForestLeafDirectionBuffers[_treeNum][_id][_age][_index];
    rightBuffer = &m_paintedForestLeafRightBuffers[_treeNum][_id][_age][_index];
  }
  else
  {
    forest = &m_scatteredForest;
    vao = &m_forestLeafVAOs[_treeNum][_id][_age][_index];
    dirBuffer = &m_forestLeafDirectionBuffers[_treeNum][_id][_age][_index];
    rightBuffer = &m_forestLeafRightBuffers[_treeNum][_id][_age][_index];
  }

  LSystem &treeType = forest->m_treeTypes[_treeNum];
  CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;
  //std::unique_ptr<ngl::AbstractVAO> &vao = m_forestLeafVAOs[_treeNum][_id][_age][_index];

  buildInstanceCacheVAO(*vao,
                        treeType.m_heroLeafVertices,
                        treeType.m_heroLeafIndices,
                        forest->m_transformCache[_treeNum][_id][_age][_index],
                        instanceCache[_id][_age][_index].m_instanceLeafStart,
                        instanceCache[_id][_age][_index].m_instanceLeafEnd,
                        GL_POINTS);

  (*vao)->bind();
  addBufferToBoundVAO(sizeof(ngl::Vec3)*treeType.m_heroLeafDirections.size(),
                      &treeType.m_heroLeafDirections[0].m_x, *dirBuffer);
  (*vao)->setVertexAttributePointer(5,3,GL_FLOAT,12,0);
  addBufferToBoundVAO(sizeof(ngl::Vec3)*treeType.m_heroLeafRightVectors.size(),
                      &treeType.m_heroLeafRightVectors[0].m_x, *rightBuffer);
  (*vao)->setVertexAttributePointer(6,3,GL_FLOAT,12,0);
  (*vao)->unbind();

}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildForestPolygonVAO(size_t _treeNum, size_t _id, size_t _age, size_t _index, bool _usePaintedForest)
{
  Forest *forest;
  std::unique_ptr<ngl::AbstractVAO> *vao;
  if(_usePaintedForest)
  {
    forest = &m_paintedForest;
    vao = &m_paintedForestPolygonVAOs[_treeNum][_id][_age][_index];
  }
  else
  {
    forest = &m_scatteredForest;
    vao = &m_forestPolygonVAOs[_treeNum][_id][_age][_index];
  }

  LSystem &treeType = forest->m_treeTypes[_treeNum];
  CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;

  buildInstanceCacheVAO(*vao,
                        treeType.m_heroPolygonVertices,
                        treeType.m_heroPolygonIndices,
                        forest->m_transformCache[_treeNum][_id][_age][_index],
                        instanceCache[_id][_age][_index].m_instancePolygonStart,
                        instanceCache[_id][_age][_index].m_instancePolygonEnd,
                        GL_TRIANGLES);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildScatteredForestVAOs()
{  
  for(size_t t=0; t<m_scatteredForest.m_treeTypes.size(); t++)
  {
    LSystem &treeType = m_scatteredForest.m_treeTypes[t];
    CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;
    std::cout<<instanceCache.size();
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestVAOs[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestLeafVAOs[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestPolygonVAOs[t], instanceCache)

    RESIZE_CACHE_BY_OTHER_CACHE(m_forestRightBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestThicknessBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestLeafDirectionBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_forestLeafRightBuffers[t], instanceCache)

    FOR_EACH_ELEMENT(m_forestVAOs[t],
                     buildForestVAO(t,ID,AGE,INDEX,false);
                     buildForestLeafVAO(t,ID,AGE,INDEX,false);
                     buildForestPolygonVAO(t,ID,AGE,INDEX,false))

  }
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::buildPaintedForestVAOs()
{
  for(size_t t=0; t<m_paintedForest.m_treeTypes.size(); t++)
  {
    LSystem &treeType = m_paintedForest.m_treeTypes[t];
    CACHE_STRUCTURE(Instance) &instanceCache = treeType.m_instanceCache;

    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestVAOs[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestLeafVAOs[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestPolygonVAOs[t], instanceCache)

    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestRightBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestThicknessBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestLeafDirectionBuffers[t], instanceCache)
    RESIZE_CACHE_BY_OTHER_CACHE(m_paintedForestLeafRightBuffers[t], instanceCache)

    FOR_EACH_ELEMENT(m_paintedForestVAOs[t],
                     buildForestVAO(t,ID,AGE,INDEX,true);
                     buildForestLeafVAO(t,ID,AGE,INDEX,true);
                     buildForestPolygonVAO(t,ID,AGE,INDEX,true))
  }
}
