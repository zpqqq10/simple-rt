#pragma once
#include "common.h"

// orthonormal basis
// to transform the pdf-weighted directions relative to z-axis into ones
// relative to normal
class ONB {
public:
  // similar to x, y, z
  // similar to glm, basis vectors are made public
  vec3 u, v, w;
  ONB(const vec3 &normal) {
    w = glm::normalize(normal);
    // to simply pick a vector to help build onb
    vec3 a = (std::fabs(w.x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    v = glm::normalize(glm::cross(w, a));
    // u is naturally normalized
    u = glm::cross(w, v);
  }

  vec3 transform(const vec3 &vec) const {
    // Transform from basis coordinates to local space.
    return (vec.x * u) + (vec.y * v) + (vec.z * w);
  }
};
