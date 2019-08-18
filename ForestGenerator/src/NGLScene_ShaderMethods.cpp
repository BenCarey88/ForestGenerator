//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_ShaderMehtods.cpp
/// @brief implementation file for NGLScene methods to do with loading uniforms to shaders
//----------------------------------------------------------------------------------------------------------------------

#include <ngl/NGLInit.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleIndexVAO.h>
#include <ngl/SimpleVAO.h>
#include <ngl/Texture.h>
#include <ngl/ShaderLib.h>
#include <ngl/Image.h>

#include "InstanceCacheVAO.h"
#include "NGLScene.h"
#include "Camera.h"
#include "PrintFunctions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TreeTexLoc 0
#define TreeNormalLoc 1
#define LeafTexLoc 2
#define LeafNormalLoc 3
#define TerrainTexLoc 3
#define TerrainNormalLoc 4

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::compileShaders()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->loadShader("SkeletalTreeShader", "shaders/SkeletalTreeVertex.glsl",
                     "shaders/SkeletalTreeFragment.glsl");
  shader->loadShader("TreeShader", "shaders/TreeVertex.glsl",
                     "shaders/TreeFragment.glsl", "shaders/TreeGeometry.glsl");
  shader->loadShader("LeafShader", "shaders/LeafVertex.glsl",
                     "shaders/LeafFragment.glsl", "shaders/LeafGeometry.glsl");
  shader->loadShader("PolygonShader", "shaders/PolygonVertex.glsl",
                     "shaders/PolygonFragment.glsl");

  shader->loadShader("SkeletalForestShader", "shaders/SkeletalForestVertex.glsl",
                     "shaders/SkeletalForestFragment.glsl");
  shader->loadShader("ForestShader", "shaders/ForestVertex.glsl",
                     "shaders/ForestFragment.glsl", "shaders/ForestGeometry.glsl");
  shader->loadShader("ForestLeafShader", "shaders/ForestLeafVertex.glsl",
                     "shaders/ForestLeafFragment.glsl", "shaders/ForestLeafGeometry.glsl");
  shader->loadShader("ForestPolygonShader", "shaders/ForestPolygonVertex.glsl",
                     "shaders/ForestPolygonFragment.glsl");

  shader->loadShader("GridShader", "shaders/GridVertex.glsl",
                     "shaders/GridFragment.glsl");
  shader->loadShader("TerrainShader", "shaders/TerrainVertex.glsl",
                     "shaders/TerrainFragment.glsl");
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::loadShaderTextures()
{
  loadTextureToShader("TreeShader", "textureMap", "textures/American_oak_pxr128.jpg", TreeTexLoc);
  loadTextureToShader("TreeShader", "normalMap", "textures/American_oak_pxr128_normal.jpg", TreeNormalLoc);
  loadTextureToShader("ForestShader", "textureMap", "textures/American_oak_pxr128.jpg", TreeTexLoc);
  loadTextureToShader("ForestShader", "normalMap", "textures/American_oak_pxr128_normal.jpg", TreeNormalLoc);

  loadTextureToShader("LeafShader", "textureMap", "textures/leaf.png", LeafTexLoc);
  loadTextureToShader("ForestLeafShader", "textureMap", "textures/leaf.png", LeafTexLoc);

  loadTextureToShader("TerrainShader", "textureMap", "textures/dirtTexture.jpg", TerrainTexLoc);
  loadTextureToShader("TerrainShader", "normalMap", "textures/Lawn_grass_pxr128_normal.jpg", TerrainNormalLoc);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::loadTextureToShader(const std::string &_shaderName, const char *_textureMapName,
                                   const char *_textureMapFile, GLuint _storageLocation)
{
  ///@ref https://stackoverflow.com/questions/25252512/how-can-i-pass-multiple-textures-to-a-single-shader

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)[_shaderName]->use();

  newLine();
  print(_shaderName);
  print(" ", (*shader)[_shaderName]->getID(), "\n");

  GLint textureMapLocation = glGetUniformLocation((*shader)[_shaderName]->getID(), _textureMapName);
  glUniform1i(textureMapLocation, _storageLocation);

  print("texture map location ", textureMapLocation, "\n");

  // Then bind the uniform samplers to texture units:
  GLuint texture;
  glGenTextures(1, &texture);

  print("texture id ", texture,", storage location ", _storageLocation, "\n");

  glActiveTexture(GL_TEXTURE0 + _storageLocation); // Texture unit 0
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  QImage image;
  bool loaded=image.load(_textureMapFile);
  if(loaded == true)
  {
    int width=image.width();
    int height=image.height();

    std::unique_ptr<unsigned char []> data ( new unsigned char[size_t(width*height*4)]);
    size_t index=0;
    QRgb colour;
    for( int y=0; y<height; ++y)
    {
      for( int x=0; x<width; ++x)
      {
        colour=image.pixel(x,y);

        data[index++]=uchar(qRed(colour));
        data[index++]=uchar(qGreen(colour));
        data[index++]=uchar(qBlue(colour));
        data[index++]=uchar(qAlpha(colour));
      }
    }
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data.get());

    glGenerateMipmap(GL_TEXTURE_2D); //  Allocate the mipmaps
  }
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::loadUniformsToShader(ngl::ShaderLib *_shader, const std::string &_shaderName)
{
  ngl::Mat4 currentTransform = (*m_currentMouseTransform);//*m_initialRotation;
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MV = m_view*currentTransform;
  ngl::Mat4 M = currentTransform;
  ngl::Mat3 normalMatrix= MV.inverse().transpose();
  ngl::Mat3 newNormalMatrix= m_view.inverse().transpose();
  ngl::Vec3 lightPos = (currentTransform * ngl::Vec4(0,100,100,1)).toVec3();

  (*_shader)[_shaderName]->use();
  _shader->setUniform("MVP",MVP);
  _shader->setUniform("normalMatrix",normalMatrix);
  _shader->setUniform("newNormalMatrix",newNormalMatrix);
  _shader->setUniform("MV",MV);
  _shader->setUniform("M",M);
  _shader->setUniform("lightPosition",lightPos);
  _shader->setUniform("maxHeight",m_forest.m_terrainGen.m_amplitude);

}
