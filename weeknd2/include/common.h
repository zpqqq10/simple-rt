#pragma once

#include <cstdlib>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <memory>

// #define USE_DOUBLE

#ifdef USE_DOUBLE
using vec3 = glm::dvec3;
using vec2 = glm::dvec2;
using floating = double;
#else
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using floating = float;
#endif
using color = vec3;

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.141592653589;

inline double degrees2radians(double degrees) { return degrees * PI / 180.0; }

inline double random_double() {
  // Returns a random real in [0,1).
  return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
  // Returns a random integer in [min,max].
  return int(random_double(min, max + 1));
}

inline vec3 random_vec3() {
  return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3(double min, double max) {
  return vec3(random_double(min, max), random_double(min, max),
              random_double(min, max));
}

inline vec3 random_unit_vec3() {
  // i don undestand why a rejection method is necessary?
  auto p = random_vec3(-1, 1);
  return glm::normalize(p);
}

inline vec3 random_hemisphere(const vec3 &normal) {
  vec3 random_vector = random_unit_vec3();
  return glm::dot(normal, random_vector) > 0 ? random_vector : -random_vector;
}

inline bool near_zero(const vec3 &vec) {
  return (std::fabs(vec.x) < 1e-8) && (fabs(vec.y) < 1e-8) &&
         (fabs(vec.z) < 1e-8);
}

// to calculate the mirrored reflect direction
inline vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * glm::dot(v, n) * n;
}

inline vec3 refract(const vec3 &uv, const vec3 &n, floating etai_over_etat) {
  auto cos_theta = (floating)std::fmin(dot(-uv, n), 1.0);
  vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  vec3 r_out_parallel =
      -(floating)std::sqrt(std::fabs(1.0 - glm::dot(r_out_perp, r_out_perp))) *
      n;
  return r_out_perp + r_out_parallel;
}

inline vec3 random_in_unit_circle() {
  while (true) {
    auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (glm::dot(p, p) < 1)
      return p;
  }
}

#include "color.h"
#include "interval.h"
