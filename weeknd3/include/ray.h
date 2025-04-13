#pragma once

#include "common.h"
#include <glm/glm.hpp>

class Ray {
public:
  Ray() {}
  Ray(const vec3 &origin, const vec3 &direction, double _tm)
      : orig(origin), dir(direction), tm(_tm) {}

  Ray(const vec3 &origin, const vec3 &direction) : Ray(origin, direction, 0) {}
  // : orig(origin), dir(direction), tm(0) {}
  const vec3 &origin() const { return orig; }
  const vec3 &direction() const { return dir; }

  const double time() const { return tm; }
  vec3 normalizedDirection() const { return glm::normalize(dir); }
  vec3 at(floating t) const { return orig + t * dir; }

private:
  // origin
  vec3 orig;
  // direction
  vec3 dir;
  // time info of the ray
  double tm;
};
