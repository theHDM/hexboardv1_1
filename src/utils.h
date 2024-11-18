#pragma once
// Library of utilities
// This pulls in the standard C++ libraries
// commonly used in the Hexboard project
// and defines some often-used inline
// functions (inline means the compiler
// copies and pastes them into the code
// before creating the .uf2 file, which
// is generally faster)

#include <stdint.h>
// i.e. instead of using "byte", use uint8_t

// defining "uint" as a generic positive number
using uint = unsigned int;

// the NeoPixel takes an unsigned 32-bit integer as the pixel color code
// for most architectures, only the first 24 bits get used.
// use an alias to make it easier to identify.
using pixel_code = uint32_t;

// time codes are unsigned 64-bit integers.
// use an alias to make it easier to identify.
using time_uS = uint64_t;

// function that guarantees the mod value is always positive
template <typename T>
inline T positiveMod(T n, int d) {
  return (((n % d) + d) % d);
}
// extensions of linear interpolation used in this project
template <typename T>
inline T linterp(T xOne, T xTwo, float yOne, float yTwo, float y) {
  if (yOne == yTwo) {
    return xOne;
  } else if (y <= yOne) {
    return xOne;
  } else if (y >= yTwo) {
    return xTwo;
  } else {
    float w = (y - yOne) / (yTwo - yOne);
    return (xOne * (1 - w)) + (xTwo * w);
  }
}
// use standard C++ library string classes
// because for some reason Arduino::String corrupts memory.
// use an alias to prevent confusion.
#include <string>
using str = std::string;
// vectors are preferred to arrays
// disk space is not a concern so
// general "int" is used here except where
// a specific type such as "Arduino::byte" is required
#include <vector>
// specifically when we only need small positive integers
using byte_vec = std::vector<uint8_t>;
// generic integer vector
using int_vec = std::vector<int>;
// generic integer matrix
using int_matrix = std::vector<std::vector<int>>;
// template to resize any 2-d vector into a square
template <typename T>
inline void resize_matrix(std::vector<std::vector<T>>& m, 
                          size_t dimension_A, size_t dimension_B) {
  m.resize(dimension_A);
  for (auto& i : m) {
    i.resize(dimension_B);
  }
}

#include <functional>
// easy shorthand for passing functions as arguments
using func = std::function<void()>;

#include <deque>
// double-ended queues are used to manage channel
// assignments for MIDI, and button press lengths
// for the rotary knob
using time_queue = std::deque<time_uS>;
using byte_queue = std::deque<uint8_t>;
using int_queue = std::deque<int>;
// utility to combine the front() and pop() methods
template <typename T>
inline T pop_and_get(std::deque<T>& q) {
  T front_value = q.front();
  q.pop_front();
  return front_value;
};
// maps are useful for piecewise linear relationships
// especially when combined with interpolation
#include <map>
template <typename T1, typename T2>
inline std::map<T2,T1> invert(const std::map<T1,T2>& orig_map) {
  std::map<T2,T1> flip;
  for (auto i : orig_map) {
    flip[i.second] = i.first;
  }
  return flip;
};
// if you have a map of keys to values, use this utility
// to LERP the value that would be in between two keys.
// e.g. map=[<0.5,10>,<0.7,20>], lerp_over_map(map,0.6) = 15
template <typename T1, typename T2>
inline T2 lerp_over_map(const std::map<T1, T2>& lerp_map, T1 pos) {
  auto LB = lerp_map.lower_bound(pos);
  auto UB = std::next(LB);
  return linterp(LB->second, UB->second, LB->first, UB->first, pos);
};

// a hexboard obviously needs to have a class of hexagon coordinates.
// https://www.redblobgames.com/grids/hexagons/ for more details.
//
//                     | -y axis
//              [-1,-1] [ 1,-1]
//  -x axis [-2, 0] [ 0, 0] [ 2, 0]  +x axis
//              [-1, 1] [ 1, 1]
//                     | +y axis

struct hex_t { 
	int x;      
	int y;
	hex_t(int x=0, int y=0) : x(x), y(y) {}
  // overload the = operator
  hex_t& operator=(const hex_t& rhs) {
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
  // two hexes are == if their coordinates are ==
	bool operator==(const hex_t& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}
  // left-to-right, top-to-bottom order
  bool operator<(const hex_t& rhs) const {
    if (y == rhs.y) {
      return (x < rhs.x);
    } else {
      return (y < rhs.y);
    }
  }
  // you can + two hexes by adding the coordinates
	hex_t operator+(const hex_t& rhs) const {
		return hex_t(x + rhs.x, y + rhs.y);
	}
  // you can * a hex by a scalar to multi-step
	hex_t operator*(const int& rhs) const {
		return hex_t(rhs * x, rhs * y);
	}
};

// keep this as a non-class enum because
// we need to be able to cycle directions
enum {
	dir_e = 0,
	dir_ne = 1,
	dir_nw = 2,
	dir_w = 3,
	dir_sw = 4,
	dir_se = 5
};
hex_t unitHex[] = {
  // E       NE      NW      W       SW      SE
  { 2, 0},{ 1,-1},{-1,-1},{-2, 0},{-1, 1},{ 1, 1}
};
enum {N_A = 255};
hex_t not_a_hex = {N_A,N_A};

#include <cmath>
// trigonometric functions we need
struct polar {
	float r; 
	float d; // angle in degrees
};
struct cartesian {
	float x;
	float y;
};
inline cartesian polar_to_cartesian (const polar input) {
	float a = input.d *  0.01745329251;
	return {input.r * cos(a), input.r * sin(a)};
}
inline polar cartesian_to_polar (const cartesian input) {
	float c = sqrtf(input.x * input.x + input.y * input.y);
	float h = 180.0f + atan2f(-(input.y / c), -(input.x / c)) * 57.2957795131;
	return {c, h};
}
// other helpful utilities
template <typename T>
inline T min_of_3(T a, T b, T c) {
	return std::min(std::min(a,b),c);
};
template <typename T>
inline T max_of_3(T a, T b, T c) {
	return std::max(std::max(a,b),c);
};
template <typename T>
inline T clip(T value, T floor, T ceiling) {
	return std::max(floor,std::min(value,ceiling));
};
template <typename T>
inline int sgn(T val) {
  return (T(0) < val) - (val < T(0));
};