#ifndef PRINTFUNCTIONS_H_
#define PRINTFUNCTIONS_H_

#include <iostream>
#include <string>
#include <ngl/Mat4.h>
#include <ngl/Mat3.h>
#include <ngl/Vec3.h>
#include <ngl/AbstractVAO.h>
#include "Instance.h"


void print(std::string _str);
void print(std::string _str1, std::string _str2);
void print(std::string _str1, std::string _str2, std::string _str3);
void print(std::string _str1, std::string _str2, std::string _str3, std::string _str4);

void print(float _x);
void print(float _x1, std::string _str2);
void print(float _x1, std::string _str2, float _x2);
void print(float _x1, std::string _str2, float _x2, std::string _str3);
void print(float _x1, std::string _str2, float _x2, std::string _str3, float _x3);
void print(float _x1, std::string _str2, float _x2, std::string _str3, float _x3, std::string _str4);

void print(std::string _str, float _x);
void print(std::string _str1, float _x1, std::string _str2);
void print(std::string _str1, float _x1, std::string _str2, float _x2);
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3);
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3, float _x3);
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3, float _x3, std::string _str4);

void newLine();

void print(ngl::Mat4 _m);
void print(ngl::Mat3 _m);
void print(ngl::Vec3 _v);

void print(std::vector<float> _vec);
void print(std::vector<GLshort> _vec);
void print(std::vector<std::string> _vec);
void print(std::vector<ngl::Mat4> _vec);
void print(std::vector<ngl::Mat3> _vec);
void print(std::vector<ngl::Vec3> _vec);

void printIndices(std::vector<std::vector<std::vector<Instance> > > &_instanceCache);
void printTransform(std::vector<std::vector<std::vector<Instance> > > &_instanceCache);

#endif //PRINTFUNCTIONS_H_
