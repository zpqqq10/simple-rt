#pragma once

#include "common.h"
#include "hittable.h"
#include <glm/glm.hpp>

// directly modify u & v instead of returning a vector
inline void get_sphere_uv(const vec3 &dir, double &u, double &v) {
  // p: a given point on the sphere of radius one, centered at the origin.
  // u: returned value [0,1] of angle around the Y axis from X=-1.
  // v: returned value [0,1] of angle from Y=-1 to Y=+1.
  //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
  //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
  //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

  auto theta = std::acos(-dir.y);
  auto phi = std::atan2(-dir.z, dir.x) + PI;

  u = phi / (2 * PI);
  v = theta / PI;
}

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
    auto outnormal = glm::normalize(r.at(root) - center.at(r.time()));
    rec.set(r.at(root), root, mat);
    rec.set_face_normal(r, outnormal);
    get_sphere_uv(outnormal, rec.u, rec.v);

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