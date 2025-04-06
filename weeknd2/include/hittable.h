#pragma once
#include "aabb.h"
#include "common.h"
#include "ray.h"
#include <vector>

using std::vector;
class Material;

class HitRecord {
  // hit point
  vec3 p;
  vec3 normal;
  double t;
  bool is_front_face;

public:
  std::shared_ptr<Material> mat;
  void set(const vec3 &_p, const vec3 &_normal, const double _t) {
    p = _p;
    normal = _normal;
    t = _t;
  }

  void set(const vec3 &_p, const double _t, shared_ptr<Material> _mat) {
    p = _p;
    t = _t;
    mat = _mat;
  }

  double get_t() const { return t; }
  vec3 get_p() const { return p; }

  vec3 get_normal() const { return normal; }

  bool front_face() const { return is_front_face; }

  void set_face_normal(const Ray &r, const vec3 &_outward_normal) {
    // check unit length
    auto normal_length = glm::dot(_outward_normal, _outward_normal);
    auto outward_normal = _outward_normal;
    if (!((normal_length - 1.) < 1e-3 || (1. - normal_length) < 1e-3)) {
      outward_normal = glm::normalize(_outward_normal);
    }

    // when there is an intersection, same direction means the ray is inside the
    // object
    is_front_face = glm::dot(r.direction(), outward_normal) < 0;
    normal = is_front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
public:
  virtual ~Hittable() = default;

  virtual bool hit(const Ray &r, const Interval &ray_t,
                   HitRecord &rec) const = 0;

  virtual AABB get_bbox() const = 0;
};

class HittableList : public Hittable {
  AABB bbox;

public:
  vector<shared_ptr<Hittable>> objects;

  HittableList() {}
  HittableList(shared_ptr<Hittable> object) { add(object); }

  void clear() { objects.clear(); }

  // add one object to the list
  void add(shared_ptr<Hittable> object) {
    objects.emplace_back(object);
    bbox = AABB(bbox, object->get_bbox());
  }

  virtual bool hit(const Ray &r, const Interval &ray_t,
                   HitRecord &rec) const override {
    HitRecord tmp_rec;
    bool hit = false;

    auto closet_so_far = ray_t.max;
    for (auto object : objects) {
      if (object->hit(r, Interval(ray_t.min, closet_so_far), tmp_rec)) {
        hit = true;
        closet_so_far = tmp_rec.get_t();
        rec = tmp_rec;
      }
    }

    return hit;
  }
  
  virtual AABB get_bbox() const override { return bbox; }
};