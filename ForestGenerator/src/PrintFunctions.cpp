#include "PrintFunctions.h"

void print(std::string _str)
{
  std::cout<<_str;
}
void print(std::string _str1, std::string _str2)
{
  std::cout<<_str1<<_str2;
}
void print(std::string _str1, std::string _str2, std::string _str3)
{
  std::cout<<_str1<<_str2<<_str3;
}
void print(std::string _str1, std::string _str2, std::string _str3, std::string _str4)
{
  std::cout<<_str1<<_str2<<_str3<<_str4;
}

void print(float _x)
{
  std::cout<<_x;
}

void print(std::string _str, float _x)
{
  std::cout<<_str<<_x;
}
void print(std::string _str1, float _x1, std::string _str2)
{
  std::cout<<_str1<<_x1<<_str2;
}
void print(std::string _str1, float _x1, std::string _str2, float _x2)
{
  std::cout<<_str1<<_x1<<_str2<<_x2;
}
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3)
{
  std::cout<<_str1<<_x1<<_str2<<_x2<<_str3;
}
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3, float _x3)
{
  std::cout<<_str1<<_x1<<_str2<<_x2<<_str3<<_x3;
}
void print(std::string _str1, float _x1, std::string _str2, float _x2, std::string _str3, float _x3, std::string _str4)
{
  std::cout<<_str1<<_x1<<_str2<<_x2<<_str3<<_x3<<_str4;
}

void print(float _x1, std::string _str2)
{
  std::cout<<_x1<<_str2;
}
void print(float _x1, std::string _str2, float _x2)
{
  std::cout<<_x1<<_str2<<_x2;
}
void print(float _x1, std::string _str2, float _x2, std::string _str3)
{
  std::cout<<_x1<<_str2<<_x2<<_str3;
}
void print(float _x1, std::string _str2, float _x2, std::string _str3, float _x3)
{
  std::cout<<_x1<<_str2<<_x2<<_str3<<_x3;
}
void print(float _x1, std::string _str2, float _x2, std::string _str3, float _x3, std::string _str4)
{
  std::cout<<_x1<<_str2<<_x2<<_str3<<_x3<<_str4;
}

void newLine()
{
  std::cout<<"\n";
}

void print(ngl::Mat4 _m)
{
  std::cout<<_m.m_00<<" "<<_m.m_01<<" "<<_m.m_02<<" "<<_m.m_03<<"\n"
           <<_m.m_10<<" "<<_m.m_11<<" "<<_m.m_12<<" "<<_m.m_13<<"\n"
           <<_m.m_20<<" "<<_m.m_21<<" "<<_m.m_22<<" "<<_m.m_23<<"\n"
           <<_m.m_30<<" "<<_m.m_31<<" "<<_m.m_32<<" "<<_m.m_33<<"\n\n";
}
void print(ngl::Mat3 _m)
{
  std::cout<<_m.m_00<<" "<<_m.m_01<<" "<<_m.m_02<<"\n"
           <<_m.m_10<<" "<<_m.m_11<<" "<<_m.m_12<<"\n"
           <<_m.m_20<<" "<<_m.m_21<<" "<<_m.m_22<<"\n\n";
}
void print(ngl::Vec3 _v)
{
  std::cout<<_v.m_x<<", "<<_v.m_y<<", "<<_v.m_z<<"\n";
}

void print(std::vector<float> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}
void print(std::vector<GLshort> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}
void print(std::vector<std::string> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}
void print(std::vector<ngl::Mat4> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}
void print(std::vector<ngl::Mat3> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}
void print(std::vector<ngl::Vec3> _vec)
{
  for(auto x : _vec)
  {
    print(x);
  }
}

void printIndices(std::vector<std::vector<std::vector<Instance>>> &_instanceCache)
{
  for(auto &row : _instanceCache)
  {
    for(auto &col : row)
    {
      for(auto &instance : col)
      {
        //print(instance.m_indices);
      }
    }
  }
}
void printTransform(std::vector<std::vector<std::vector<Instance>>> &_instanceCache)
{
  for(auto &row : _instanceCache)
  {
    for(auto &col : row)
    {
      for(auto &instance : col)
      {
        print(instance.m_transform);
      }
    }
  }
}
