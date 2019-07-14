//----------------------------------------------------------------------------------------------------------------------
/// @file Grid.cpp
/// @brief implementation file for Grid struct
//----------------------------------------------------------------------------------------------------------------------

#include "Grid.h"

Grid::Grid(GLshort _numRows, float _spacing)
{
  float startVal = float(_numRows)*_spacing*0.5f;
  m_vertices = {};
  m_indices = {};
  GLshort index = 0;

  //ROWS
  for(float a=-startVal; a<=startVal; a+=_spacing)
  {
    m_vertices.push_back({a,0,-startVal});
    for(float b=-startVal; b<startVal; b+=_spacing)
    {
      m_indices.push_back(index);
      index++;
      m_vertices.push_back({a,0,b+_spacing});
      m_indices.push_back(index);
    }
    index++;
  }

  //COLUMNS - note that we already filled m_vertices when doing rows
  //so here we just need to think about indices
  for(GLshort i=0; i<_numRows+1; i++)
  {
    for(GLshort j=0; j<_numRows; j++)
    {
      m_indices.push_back(i+j*(_numRows+1));
      m_indices.push_back(i+(j+1)*(_numRows+1));
    }
  }
}
