//----------------------------------------------------------------------------------------------------------------------
/// @class InstanceCacheVAO.h
/// @brief My own VAO class to allow efficient rendering of the output data from the forest class
/// @author Jonathan Macey, Ben Carey
/// @version 1.0
/// @date 26/09/19
/// Revision History :
/// 6/4/16: This was originally copied from the SimpleIndexVAO class in the ngl library
/// 26/09/19: rewritten by Ben Carey for use in ForestGenerator project
//----------------------------------------------------------------------------------------------------------------------


#ifndef INSTANCECACHEVAO_H_
#define INSTANCECACHEVAO_H_

#include "ngl/AbstractVAO.h"
#include "Forest.h"

namespace ngl
{

class NGL_DLLEXPORT InstanceCacheVAO : public AbstractVAO
{
  public :

    class VertexData : public AbstractVAO::VertexData
    {
    public :
          /*VertexData(size_t _size,const GLfloat &_data, unsigned int _indexSize,const GLvoid *_indexData,GLenum _indexType, GLenum _mode=GL_STATIC_DRAW ) :
            AbstractVAO::VertexData(_size,_data,_mode),
            m_indexSize(_indexSize), m_indexData(_indexData), m_indexType(_indexType){}

        unsigned int m_indexSize;
        const GLvoid *m_indexData;
        GLenum m_indexType;*/

      VertexData(size_t _size, const GLfloat &_data,
                 unsigned int _indexSize,const GLvoid *_indexData,GLenum _indexType,
                 GLsizei _instanceCount, GLenum _mode=GL_STATIC_DRAW ) :
          AbstractVAO::VertexData(_size,_data,_mode),
          m_indexSize(_indexSize), m_indexData(_indexData), m_indexType(_indexType),
          m_instanceCount(_instanceCount)
      {}

      unsigned int m_indexSize;
      const GLvoid *m_indexData;
      GLenum m_indexType;

      GLsizei m_instanceCount;
    };

    GLsizei m_instanceCount;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief creator method for the factory
    /// @param _mode the mode to draw with.
    /// @returns a new AbstractVAO * object
    //----------------------------------------------------------------------------------------------------------------------
    static std::unique_ptr<AbstractVAO> create(GLenum _mode=GL_TRIANGLES) { return std::unique_ptr<AbstractVAO>(new InstanceCacheVAO(_mode)); }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draw the VAO using glDrawArrays
    //----------------------------------------------------------------------------------------------------------------------
    virtual void draw()  const override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor don't do anything as the remove clears things
    //----------------------------------------------------------------------------------------------------------------------
    virtual ~InstanceCacheVAO();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief remove the VAO and buffers created
    //----------------------------------------------------------------------------------------------------------------------
    virtual void removeVAO() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief, this method sets the data for the VAO if data has already been set it will remove the existing data
    /// and then re-set with the new data.
    /// @param _size the size of the raw data passed
    /// @param _data the actual data to set for the VOA
    /// @param _indexSize the size of the index array passed
    /// @param _indexData the actual data to set for the VOA indexes (only GLubyte data at present need to write more methods
    /// but usually only use this
    /// @param _indexType the type of the values in the indices buffer. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
    /// @param _mode the draw mode hint used by GL

    //----------------------------------------------------------------------------------------------------------------------
    //void setData(size_t _size,const GLfloat &_data,unsigned int _indexSize,const GLvoid *_indexData,GLenum _indexType,GLenum _mode=GL_STATIC_DRAW);
    virtual void setData(const AbstractVAO::VertexData &_data) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief return the id of the buffer, if there is only 1 buffer just return this
    /// if we have the more than one buffer the sub class manages the id's
    /// @param _buffer index (default to 0 for single buffer VAO's)
    //----------------------------------------------------------------------------------------------------------------------
     GLuint getBufferID(unsigned int ) override{return m_buffer;}
     //----------------------------------------------------------------------------------------------------------------------
     /// @brief grab a raw pointer to the data
     /// @param _index the index to the buffer (0) default for simple
     /// @param _mode the access more
     //----------------------------------------------------------------------------------------------------------------------
     Real * mapBuffer(unsigned int _index=0, GLenum _accessMode=GL_READ_WRITE) override;


  protected :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor calles parent ctor to allocate vao;
    //----------------------------------------------------------------------------------------------------------------------
    InstanceCacheVAO(GLenum _mode)  : AbstractVAO(_mode)
    {

    }

  private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the id of the buffer for the VAO
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_buffer=0;
    GLuint m_idxBuffer=0;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief data type of the index data (e.g. GL_UNSIGNED_INT)
    //----------------------------------------------------------------------------------------------------------------------
    GLenum m_indexType;

};


} // end namespace

#endif //INSTANCECACHEVAO_H_
