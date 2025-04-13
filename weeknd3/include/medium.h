#pragma once
#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public Hittable {
public:
  ConstantMedium(shared_ptr<Hittable> boundary, double density,
                 shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<Isotropic>(tex)) {}

  ConstantMedium(shared_ptr<Hittable> boundary, double density,
                 const color &albedo)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<Isotropic>(albedo)) {}

  bool hit(const Ray &r, const Interval &ray_t, HitRecord &rec) const override {
    HitRecord rec1, rec2;

    // check if the ray goes through the medium
    if (!boundary->hit(r, Interval::get_universe(), rec1))
      return false;

    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    if (rec1.t < ray_t.min)
      rec1.t = ray_t.min;
    if (rec2.t > ray_t.max)
      rec2.t = ray_t.max;

    if (rec1.t >= rec2.t)
      return false;

    if (rec1.t < 0)
      rec1.t = 0;

    auto ray_length = glm::length(r.direction());
    // actual distance between the 2 intersections
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    // to randomly pick a value and determine if a hit happens
    // just a simple simulation and so i use division instead of log here hoping
    // it can be faster
    auto randomv = glm::clamp(random_double(), 1e-6, 1.);

    auto hit_distance = neg_inv_density * (1 - 1 / randomv);
    // auto hit_distance = neg_inv_density * std::log(randomv);

    if (hit_distance > distance_inside_boundary)
      return false;

    // normal is casual
    rec.set(r.at(rec.t), vec3(1, 0, 0), rec1.t + hit_distance / ray_length);
    rec.is_front_face = true; // also arbitrary
    rec.mat = phase_function;

    return true;
  }

  AABB get_bbox() const override { return boundary->get_bbox(); }

private:
  shared_ptr<Hittable> boundary;
  // negative inverse density
  double neg_inv_density;
  shared_ptr<Material> phase_function;
};
