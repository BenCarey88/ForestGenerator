//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene_ShaderMehtods.cpp
/// @brief implementation file for NGLScene methods to do with loading uniforms to shaders
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::loadTextures(ngl::ShaderLib *_shader, const std::string &_shaderName,
                            const char *_textureMapFile, const char *_normalMapFile)
{
  ///@ref https://stackoverflow.com/questions/25252512/how-can-i-pass-multiple-textures-to-a-single-shader

  (*_shader)[_shaderName]->use();

  GLint textureMapLocation = glGetUniformLocation((*_shader)[_shaderName]->getID(), "textureMap");
  GLint normalMapLocation  = glGetUniformLocation((*_shader)[_shaderName]->getID(), "normalMap");
  glUniform1i(textureMapLocation, 0);
  glUniform1i(normalMapLocation,  1);

  // Then bind the uniform samplers to texture units:
  GLuint texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(_textureMapFile, &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load texture for "<<_shaderName<<"\n";
  }
  stbi_image_free(data);

  unsigned int normalMap;
  glGenTextures(1, &normalMap);
  glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
  glBindTexture(GL_TEXTURE_2D, normalMap);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  data = stbi_load(_normalMapFile, &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load normal map for "<<_shaderName<<"\n";
  }
  stbi_image_free(data);
}

//------------------------------------------------------------------------------------------------------------------------

void NGLScene::loadUniformsToShader(ngl::ShaderLib *_shader, const std::string &_shaderName)
{
  ngl::Mat4 currentTransform = (*m_currentMouseTransform)*m_initialRotation;
  ngl::Mat4 MVP= m_project*m_view*currentTransform;
  ngl::Mat4 MV = m_view*currentTransform;
  ngl::Mat4 M = currentTransform;
  ngl::Mat3 normalMatrix= MV.inverse().transpose();
  ngl::Mat3 newNormalMatrix= m_view.inverse().transpose();
  ngl::Vec3 lightPos = (currentTransform * ngl::Vec4(0,100,0,1)).toVec3();

  (*_shader)[_shaderName]->use();
  _shader->setUniform("MVP",MVP);
  _shader->setUniform("normalMatrix",normalMatrix);
  _shader->setUniform("newNormalMatrix",newNormalMatrix);
  _shader->setUniform("MV",MV);
  _shader->setUniform("M",M);
  _shader->setUniform("lightPosition",lightPos);
  _shader->setUniform("maxHeight",m_forest.m_terrainGen.m_amplitude);
}
