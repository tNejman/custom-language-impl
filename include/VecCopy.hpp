#pragma once

#include <algorithm>
#include <iterator>
#include <vector>


template <typename T>
std::vector<T> copyVec( const std::vector<T>& src ) {
  std::vector<T> dest;
  dest.reserve( src.size() );
  std::transform( src.begin(), src.end(), std::back_inserter( dest ), []( const T& item ) { return item.copy(); } );

  return dest;
}