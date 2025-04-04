#pragma once

#include "common.h"
#include "hittable.h"
#include <glm/glm.hpp>


class Sphere : public Hittable {
public:
  Sphere(const vec3 &_center, const double _radius, std::shared_ptr<Material> _mat)
      : center(_center), radius(std::fmax(0, _radius)), mat(_mat) {}

  virtual bool hit(const Ray &r, const Interval &ray_t,
                   HitRecord &rec) const override {
    vec3 oc = center - r.origin();
    auto a = glm::dot(r.direction(), r.direction());
    auto h = glm::dot(r.direction(), oc);
    auto c = glm::dot(oc, oc) - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0) {
      // no intersection
      return false;
    }

    auto sqrtd = std::sqrt(discriminant);
    // neaeresr root
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        return false;
      }
    }

    // set record
    rec.set(r.at(root), root, mat);
    rec.set_face_normal(r, glm::normalize(r.at(root) - center));

    return true;
  }

private:
  vec3 center;
  double radius;
  std::shared_ptr<Material> mat;
};