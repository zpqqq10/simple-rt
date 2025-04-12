#pragma once
#include "aabb.h"
#include "common.h"
#include "ray.h"
#include <vector>

using std::vector;
class Material;

// in fact this class is better to be treated as a struct
// better to set all the members public
class HitRecord {

public:
  // hit point
  vec3 p;
  vec3 normal;
  double t;
  bool is_front_face;
  // u-v for texture
  // in graphics pipeline this should be stored within the model
  double u, v;
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
    for (const auto object : objects) {
      if (object->hit(r, Interval(ray_t.min, closet_so_far), tmp_rec)) {
        hit = true;
        closet_so_far = tmp_rec.t;
        rec = tmp_rec;
      }
    }

    return hit;
  }

  virtual AABB get_bbox() const override { return bbox; }
};

// reversely moving the ray instead of moveing the object
// but need to recalculate for every ray
class Translate : public Hittable {
public:
  Translate(shared_ptr<Hittable> _object, const vec3 &_offset)
      : object(_object), offset(_offset) {
    bbox = object->get_bbox() + offset;
  }

  bool hit(const Ray &r, const Interval &ray_t, HitRecord &rec) const override {
    // Move the ray backwards by the offset
    Ray offset_r(r.origin() - offset, r.direction(), r.time());

    // Determine whether an intersection exists along the offset ray (and if so,
    // where)
    if (!object->hit(offset_r, ray_t, rec))
      return false;

    // Move the intersection point forwards by the offset
    rec.p = rec.p + offset;

    return true;
  }
  AABB get_bbox() const override { return bbox; }

private:
  shared_ptr<Hittable> object;
  vec3 offset;
  AABB bbox;
};

// reversely moving the ray instead of moveing the object
// but need to recalculate for every ray
class RotateY : public Hittable {
public:
  RotateY(shared_ptr<Hittable> object, double angle) : object(object) {
    auto radians = degrees2radians(angle);
    sin_theta = std::sin(radians);
    cos_theta = std::cos(radians);
    bbox = object->get_bbox();

    vec3 min(infinity, infinity, infinity);
    vec3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
          auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
          auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

          auto newx = cos_theta * x + sin_theta * z;
          auto newz = -sin_theta * x + cos_theta * z;

          vec3 tester(newx, y, newz);

          for (int c = 0; c < 3; c++) {
            min[c] = std::fmin(min[c], tester[c]);
            max[c] = std::fmax(max[c], tester[c]);
          }
        }
      }
    }

    bbox = AABB(min, max);
  }

  bool hit(const Ray &r, const Interval &ray_t, HitRecord &rec) const override {

    // Transform the ray from world space to object space.

    auto origin = vec3((cos_theta * r.origin().x) - (sin_theta * r.origin().z),
                       r.origin().y,
                       (sin_theta * r.origin().x) + (cos_theta * r.origin().z));

    auto direction =
        vec3((cos_theta * r.direction().x) - (sin_theta * r.direction().z),
             r.direction().y,
             (sin_theta * r.direction().x) + (cos_theta * r.direction().z));

    Ray rotated_r(origin, direction, r.time());

    // Determine whether an intersection exists in object space (and if so,
    // where).

    if (!object->hit(rotated_r, ray_t, rec))
      return false;

    // Transform the intersection from object space back to world space.

    rec.p = vec3((cos_theta * rec.p.x) + (sin_theta * rec.p.z), rec.p.y,
                 (-sin_theta * rec.p.x) + (cos_theta * rec.p.z));

    rec.normal = vec3((cos_theta * rec.normal.x) + (sin_theta * rec.normal.z),
                      rec.normal.y,
                      (-sin_theta * rec.normal.x) + (cos_theta * rec.normal.z));

    return true;
  }

  AABB get_bbox() const override { return bbox; }

private:
  shared_ptr<Hittable> object;
  double sin_theta;
  double cos_theta;
  AABB bbox;
};