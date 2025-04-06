#pragma once

#include "common.h"
#include "hittable.h"
#include <glm/glm.hpp>

class Sphere : public Hittable {
public:
  // stationary sphere
  Sphere(const vec3 &static_center, const double _radius,
         std::shared_ptr<Material> _mat)
      : center(static_center, vec3(0, 0, 0)), radius(std::fmax(0, _radius)),
        mat(_mat) {
    auto half_bbox = vec3(radius, radius, radius);
    bbox = AABB(static_center - half_bbox, static_center + half_bbox);
  }

  // moving sphere
  Sphere(const vec3 &center_begin, const vec3 &center_end, const double _radius,
         std::shared_ptr<Material> _mat)
      : center(center_begin, center_end - center_begin),
        radius(std::fmax(0, _radius)), mat(_mat) {
    auto half_bbox = vec3(radius, radius, radius);
    AABB box1(center.at(0) - half_bbox, center.at(0) + half_bbox);
    AABB box2(center.at(1) - half_bbox, center.at(1) + half_bbox);
    bbox = AABB(box1, box2);
  }

  Sphere(const Ray &_center, const double _radius,
         std::shared_ptr<Material> _mat)
      : center(_center), radius(std::fmax(0, _radius)), mat(_mat) {}

  virtual bool hit(const Ray &r, const Interval &ray_t,
                   HitRecord &rec) const override {
    vec3 oc = center.at(r.time()) - r.origin();
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
    rec.set_face_normal(r, glm::normalize(r.at(root) - center.at(r.time())));

    return true;
  }

  virtual AABB get_bbox() const override { return bbox; }

private:
  // the sphere can be moving
  Ray center;
  double radius;
  std::shared_ptr<Material> mat;
  AABB bbox;
};