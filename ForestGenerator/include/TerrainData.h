//----------------------------------------------------------------------------------------------------------------------
/// @file TerrainData.h
/// @author Ben Carey
/// @version 1.0
/// @date 23/01/19 updated to NCCA coding standards
/// Revision History :
/// Initial Version 14/12/18, used for ASE TerrainGeneration project
/// 01/09/19 added to ForestGeneration masters project
//----------------------------------------------------------------------------------------------------------------------

#ifndef TERRAINDATA_H_
#define TERRAINDATA_H_

#include <cmath>
#include <iostream>
#include <vector>
#include <ngl/Vec3.h>

//----------------------------------------------------------------------------------------------------------------------
/// @class TerrainData
/// @brief this class takes heightMap values from an object of the TerrainGenerator class and applies a LOD reduction
/// algorithm to it.
/// The algorithm is taken from 'Visualization of Large Terrains Made Easy', Lindstrom & Pascucci, 2001, and involves
/// arranging the vertices into a directed acyclic graph (DAG) and also arranging them into 2 quadtrees (QT), which are
/// visualised in the paper as a black quadtree (BQT) and a white quadtree (WQT). Consequently, several of the variable
/// and method names reference these graph structures and use the abbreviations DAG, QT, BQT and WQT.
//----------------------------------------------------------------------------------------------------------------------

class TerrainData
{
public:

  //CONSTRUCTOR FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default ctor for our TerrainData class
  /// needed for m_terrain in NGLScene which starts with heightmap values uninitialised
  //--------------------------------------------------------------------------------------------------------------------
  TerrainData()=default;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our TerrainData class
  /// @param [in] dimension, the dimension of the grid being drawn
  /// (this must be 2^n+1 for positive integer n, based on the way the algorithm is designed)
  /// @param [in] heightmap, the std::vector of z-values for the terrain vertices
  //--------------------------------------------------------------------------------------------------------------------
  TerrainData(int _dimension, std::vector<float> _heightMap);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief default dtor for our TerrainData class
  //--------------------------------------------------------------------------------------------------------------------
  ~TerrainData()=default;

  //PUBLIC STRUCT
  //--------------------------------------------------------------------------------------------------------------------
  /// @struct Vertex
  /// @brief this structure stores (x,y,z) coordinates of the vertices in the terrain, as well as attributes required
  /// for the LOD algorithm. The x and y coordinates are passed in as equally spaced positive integers, which are easier
  /// to use for some aspects of the algorithm, but the coordinates will be transformed for their final positions in the
  /// NGLscene scene; consequently this struct stores two values for each coordinate, its original value and its final
  /// scene value
  //--------------------------------------------------------------------------------------------------------------------
  struct Vertex
  {
    //------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for Vertex Structure
    /// @param [in] x, the x coordinate (before transformation)
    /// @param [in] y, the y coordinate (before transformation)
    /// @param [in] z, the z (height) coordinate taken from m_heightmap (before transformation)
    //------------------------------------------------------------------------------------------------------------------
    Vertex(int _x, int _y, float _z, int _dimension, float _scale);
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the x coordinate of the vertex as it apears in NGL scene
    //------------------------------------------------------------------------------------------------------------------
    float sceneX;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the y coordinate of the vertex as it apears in NGL scene
    //------------------------------------------------------------------------------------------------------------------
    float sceneY;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the z (height) coordinate of the vertex as it apears in NGL scene
    //------------------------------------------------------------------------------------------------------------------
    float sceneZ;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the x coordinate before transformation
    //------------------------------------------------------------------------------------------------------------------
    int originalX;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the y coordinate before transformation
    //------------------------------------------------------------------------------------------------------------------
    int originalY;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the z coordinate before transformation
    //------------------------------------------------------------------------------------------------------------------
    float originalZ;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief list of indices of the current vertex's children in the DAG
    //------------------------------------------------------------------------------------------------------------------
    std::vector<size_t> childList = {};
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the radius of the bounding sphere assigned to the vertex for the LOD algorithm
    //------------------------------------------------------------------------------------------------------------------
    float radius = 0;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the delta value assigned to the vertex for the LOD algorithm, representing the world space error
    /// introduced to the scene by removing this vertex
    //------------------------------------------------------------------------------------------------------------------
    float delta = 0;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief the augmented delta value assigned to the vertex for the LOD algorithm
    //------------------------------------------------------------------------------------------------------------------
    float augmentedDelta = 0;
    //------------------------------------------------------------------------------------------------------------------
    /// @brief method returning the distance between this vertex and another one
    //------------------------------------------------------------------------------------------------------------------
    float distanceTo(Vertex v) const;
  };

  //PUBLIC MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief dimension of terrain grid (must be 2^n+1 for some n)
  //--------------------------------------------------------------------------------------------------------------------
  int m_dimension;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the heightMap values for the terrain (stored as a std::vector of size m_dimension^2)
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<float>  m_heightMap = {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief list of vertices of the terrain (stored in an order that allows easy access of child vertices)
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<Vertex> m_vertices = {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief list of indices of m_vertices to be sent to NGLScene to allow it to draw the terrain with reduced LOD
  /// using GL_TRIANGLE_STRIP
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<size_t> m_indices = {};
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief list of list of indices of m_vertices arranged by their level in DAG, to make it easier to assign radius
  /// and delta values to the vertices
  //--------------------------------------------------------------------------------------------------------------------
  std::vector<std::vector<size_t>> m_verticesArrangedByGraphLevel;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief scale used to transform vertices to scene position
  //--------------------------------------------------------------------------------------------------------------------
  float m_scale = 50.0f/m_dimension;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief the maximum refinement level for the LOD algorithm (equal to the height of the DAG)
  //--------------------------------------------------------------------------------------------------------------------
  int m_maxRefinementLevel = int(2*std::log2(m_dimension-1));

  std::vector<ngl::Vec3> m_vertsToBeRendered = {};
  std::vector<GLshort> m_indicesToBeRendered = {};

  //call this after meshRefine
  void fillVerticsAndIndicesForRendering();

  //PUBLIC MEMBER FUNCTION: MESHREFINE
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief runs the LOD reduction algorithm on the terrain based on the current camera position and the user-specified
  /// tolerance, then fills m_indices according to this data
  /// @ref this method, and the methods it calls, is based on pseudocode from Lindstrom and Pascucci (2001)
  /// @param [in] cameraPos, the position of the camera
  /// @param [in] tolerance, the user-specified error tolerance
  /// @param [in] lambda, this should be set equal to the field of view divided by the number of pixels along the
  /// field of view - or can be changed to increase or decrease effect of the tolerance
  //--------------------------------------------------------------------------------------------------------------------
  void meshRefine(ngl::Vec3 _cameraPos, float _tolerance, float _lambda);


  //OTHER PUBLIC MEMBER FUNCTIONS - all these functions could be private as they do not need to be accessed outside the
  //class; however I have left them public since I needed to test them in the googletest files as part of the TDD
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the x value associated with a certain index of the heightmap
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  int getX(const size_t _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the y value associated with a certain index of the heightmap
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  int getY(const size_t _index) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the heightmap index for a given set of (x,y) coordinates
  /// @param [in] x, the x coordinate
  /// @param [in] y, the y coordinate
  //--------------------------------------------------------------------------------------------------------------------
  size_t getHeightMapIndex(const int _x, const int _y) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns a vertex corresponding to a given heightmap index
  /// @param [in] index, the heightmap index
  //--------------------------------------------------------------------------------------------------------------------
  Vertex getVertex(const size_t _index) const;

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recursively fills m_vertices with vertices in an order determined by their positions in the white quadtree
  /// @ref the ordering is based on Lindstrom and Pascucci, 2001
  /// @param [in] QTParentHeightMapIndex, the m_heightmap index of the current vertices' parent in the quadtree
  /// @param [in] QTParentVerticesIndex, the m_vertices index of the current vertices' parent in the quadtree
  /// @param [in] refinementLevel, the refinement level (height in the DAG) of the current vertex
  /// @param [in] distance, the distance from the parent vertex to its children
  //--------------------------------------------------------------------------------------------------------------------
  void createVerticesWQT(size_t _QTParentHeightMapIndex, size_t _QTParentVerticesIndex,
                                      int _refinementLevel, int _distance);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recursively fills m_vertices with vertices in an order determined by their positions in the black quadtree
  /// @ref the ordering is based on Lindstrom and Pascucci, 2001
  /// @param [in] QTParentHeightMapIndex, the m_heightmap index of the current vertices' parent in the quadtree
  /// @param [in] QTParentVerticesIndex, the m_vertices index of the current vertices' parent in the quadtree
  /// @param [in] refinementLevel, the refinement level (height in the DAG) of the current vertex
  /// @param [in] distance, the distance from the parent vertex to its children
  //--------------------------------------------------------------------------------------------------------------------
  void createVerticesBQT(size_t _QTParentHeightMapIndex, size_t _QTParentVerticesIndex,
                                      int _refinementLevel, int _distance);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the index of m_vertices corresponding to the first vertex that is a child of QTParent in the
  /// quadtree structure and a child of DAGParent in the directed acyclic graph structure
  /// @ref method is from Lindstrom and Pascucci, 2001
  /// @param [in] QTParent, the index of the output vertex's quadtree parent
  /// @param [in] DAGParent, the index of the output vertex's graph parent
  //--------------------------------------------------------------------------------------------------------------------
  size_t getChild1(size_t _QTParent, size_t _DAGParent) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief returns the index of m_vertices corresponding to the second vertex that is a child of QTParent in the
  /// quadtree structure and a child of DAGParent in the directed acyclic graph structure
  /// @ref method is from Lindstrom and Pascucci, 2001
  /// @param [in] QTParent, the index of the output vertex's quadtree parent
  /// @param [in] DAGParent, the index of the output vertex's graph parent
  //--------------------------------------------------------------------------------------------------------------------
  size_t getChild2(size_t _QTParent, size_t _DAGParent) const;
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recursively fills m_verticesArrangedByGraphLevel and the childList members of each vertex
  /// @param [in] DAGParent, the index of the current vertex's graph parent
  /// @param [in] currentVertex, the index of the current vertex whose childList is being filled
  /// @param [in] refinementLevel, the refinement level (height in the DAG) of the current vertex
  //--------------------------------------------------------------------------------------------------------------------
  void assignChildren(size_t _DAGParent, size_t _currentVertex, int _refinementLevel);

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief assigns a bounding sphere radius to each vertex using m_verticesArrangedByGraphLevel
  //--------------------------------------------------------------------------------------------------------------------
  void assignRadius();
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief recursively assigns a delta value to each vertex
  /// @param [in] DAGGrandParent, index of the current vertex's grand parentin the DAG
  /// @param [in] DAGParent, index of the current vertex's parent in the DAG
  /// @param [in] currentVertex, index of the current vertex being assigned
  //--------------------------------------------------------------------------------------------------------------------
  void assignDelta(size_t _DAGGrandParent, size_t _DAGParent, size_t _currentVertex, int _refinementLevel);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief assigns an augmented delta value to each vertex using m_verticesArrangedByGraphLevel
  //--------------------------------------------------------------------------------------------------------------------
  void assignAugmentedDelta();

  //--------------------------------------------------------------------------------------------------------------------
  /// @brief accessed by meshRefine method to recursively apply LOD algorithm
  /// @ref based on pseudocode from Lindstrom and Pascucci (2001)
  /// @param [in] currentVertex, index of the current vertex
  /// @param [in] DAGChildVertex, index of a child of the current vertex in the graph
  /// @param [in] refinementLevel, the refinement level (height in the DAG) of the current vertex
  /// @param [in] cameraPos, camera position
  /// @param [in] tolerance, the user-specified error tolerance
  /// @param [in] lambda, equal to the field of view divided by the number of pixels along the field of view
  //--------------------------------------------------------------------------------------------------------------------
  void submeshRefine(size_t _DAGParentVertex, size_t _currentVertex, int _refinementLevel,
                       ngl::Vec3 _cameraPos, float _tolerance, float _lambda);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief accessed by submeshRefine method to add active vertices to m_indices
  /// @ref based on pseudocode from Lindstrom and Pascucci (2001)
  /// @param [in] vertex, the vertex index to be added to m_indices
  /// @param [in] parity, determines whether or not to add additional indices to m_indices in order to turn corners in
  /// the triangle strip
  //--------------------------------------------------------------------------------------------------------------------
  void tstripAppend(size_t _vertex, int _parity);
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief determines whether the current vertex is active and should be included in m_indices
  /// @ref based on pseudocode from Lindstrom and Pascucci (2001)
  /// @param [in] vertex, the index of the vertex being considered
  /// @param [in] tolerance, the user-specified error tolerance
  /// @param [in] lambda, equal to the field of view divided by the number of pixels along the field of view
  //--------------------------------------------------------------------------------------------------------------------
  bool isActive(size_t _vertex, ngl::Vec3 _cameraPos, float _tolerance, float _lambda) const;


private:


  //PRIVATE MEMBER VARIABLES
  //--------------------------------------------------------------------------------------------------------------------
  /// @brief parity used for the mesh refine method, to decide when to "turn corners" in the triangle strip
  //--------------------------------------------------------------------------------------------------------------------
  int m_parity = 0;
  //--------------------------------------------------------------------------------------------------------------------

};

#endif //TERRAINDATA_H_
