#pragma once

#include <limits>

namespace tkom_limits {
constexpr static inline int MAX_INT = std::numeric_limits<int>::max();
constexpr static inline int MIN_INT = std::numeric_limits<int>::min();

constexpr static inline float MAX_FLOAT = static_cast<float>( std::numeric_limits<int>::max() );
constexpr static inline float MIN_FLOAT = static_cast<float>( std::numeric_limits<int>::min() );
}  // namespace tkom_limits