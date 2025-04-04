#pragma once

#include <glm/glm.hpp>
#include "common.h"

class Ray {
public:
  Ray() {}
  Ray(const vec3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}
  const vec3 &origin() const { return orig; }
  const vec3 &direction() const { return dir; }
  vec3 normalizedDirection() const { return glm::normalize(dir); }
  vec3 at(floating t) const { return orig + t * dir; }

private:
  // origin
  vec3 orig;
  // direction
  vec3 dir;
};
