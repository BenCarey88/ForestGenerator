//----------------------------------------------------------------------------------------------------------------------
/// @file InstanceCacheMacros.h
/// @author Ben Carey
/// @version 1.0
/// @date 01/08/19
//----------------------------------------------------------------------------------------------------------------------

#ifndef INSTANCECACHEMACROS_H_
#define INSTANCECACHEMACROS_H_

#include <vector>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the instance cache structure consists of a nesting of std::vectors: vector<vector<vector<Class>>>>
/// (1) the outer layer index separates instances by id of the branch
/// (2) the middle layer separates instances of the same id by the age of tree generation they're introduced in
/// (3) the inner index separates different instances of the same id and age
/// Since several classes contain different implementations of this nested structure to store different types
/// of variable, I have created the following macros to reduce repetition and improve readability of the code
///
/// (I initially tried to do this with a templated class, but ran into issues when I tried to pass a unique
/// pointer as the template argument, since some of the methods would have required calling the copy constructor
/// of the unique pointer)
//----------------------------------------------------------------------------------------------------------------------

#define CACHE_STRUCTURE(_class) std::vector<std::vector<std::vector<_class>>>

#define RESIZE_CACHE_BY_VALUES(_cache, _idMax, _ageMax) \
  _cache.resize(_idMax);                                \
  for(auto &ROW : _cache)                               \
  {                                                     \
    ROW.resize(_ageMax);                                \
  }

#define RESIZE_CACHE_BY_OTHER_CACHE(_cache, _otherCache)    \
  _cache.resize(_otherCache.size());                        \
  for(size_t ID=0; ID<_cache.size(); ID++)                  \
  {                                                         \
    _cache[ID].resize(_otherCache[ID].size());              \
    for(size_t AGE=0; AGE<_cache[ID].size(); AGE++)         \
    {                                                       \
      _cache[ID][AGE].resize(_otherCache[ID][AGE].size());  \
    }                                                       \
  }


/// @note this macro gives _function access to the variables ID, AGE and INDEX
/// @note _function can be replaced by any string of commands, separated by ;s
#define FOR_EACH_ELEMENT(_cache, _function)                       \
  for(size_t ID=0; ID<_cache.size(); ID++)                        \
  {                                                               \
    for(size_t AGE=0; AGE<_cache[ID].size(); AGE++)               \
    {                                                             \
      for(size_t INDEX=0; INDEX<_cache[ID][AGE].size(); INDEX++)  \
      {                                                           \
        _function;                                                \
      }                                                           \
    }                                                             \
  }


#endif //INSTANCECACHEMACROS_H_
