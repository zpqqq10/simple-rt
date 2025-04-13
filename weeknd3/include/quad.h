#pragma once
#include "hittable.h"

class Quad : public Hittable {
public:
  Quad(const vec3 &Q, const vec3 &u, const vec3 &v, shared_ptr<Material> mat)
      : Q(Q), u(u), v(v), mat(mat) {
    auto n = glm::cross(u, v);
    normal = glm::normalize(n);
    D = glm::dot(normal, Q);
    w = n / glm::dot(n, n);
    // cross product represents the area of the quad
    area = glm::length(n);
    set_bounding_box();
  }

  virtual void set_bounding_box() {
    // Compute the bounding box of all four vertices.
    auto bbox_diagonal1 = AABB(Q, Q + u + v);
    auto bbox_diagonal2 = AABB(Q + u, Q + v);
    bbox = AABB(bbox_diagonal1, bbox_diagonal2);
  }

  void debugp() const override { std::clog << "quad" << std::flush; }

  double pdf_value(const vec3 &origin, const vec3 &direction) const override {
    HitRecord rec;
    if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec))
      return 0;

    // similar to light sampling here, in fact it is only used for light
    // sampling in our case
    auto distance_squared = rec.t * rec.t * glm::dot(direction, direction);
    auto cosine = std::fabs(dot(direction, rec.normal) / glm::length(direction));

    return distance_squared / (cosine * area);
  }

  vec3 random(const vec3 &origin) const override {
    auto p = Q + (random_double() * u) + (random_double() * v);
    return p - origin;
  }

  AABB get_bbox() const override { return bbox; }

  // check parallelism
  // check aabb box
  // check intersection
  bool hit(const Ray &r, const Interval &ray_t, HitRecord &rec) const override {
    auto denom = glm::dot(normal, r.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < 1e-8)
      return false;

    // Return false if the hit point parameter t is outside the ray interval.
    auto t = (D - glm::dot(normal, r.origin())) / denom;
    if (!ray_t.contains(t))
      return false;

    // Determine if the hit point lies within the planar shape using its plane
    // coordinates.
    auto intersection = r.at(t);
    vec3 planar_hitpt_vector = intersection - Q;
    // check if alpha and beta are within [0, 1]
    auto alpha = glm::dot(w, glm::cross(planar_hitpt_vector, v));
    auto beta = glm::dot(w, glm::cross(u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
      return false;

    // Ray hits the 2D shape; set the rest of the hit record and return true.

    rec.set(intersection, t, mat);
    rec.set_face_normal(r, normal);
    return true;
  }

  virtual bool is_interior(double a, double b, HitRecord &rec) const {
    Interval unit_interval = Interval(0, 1);
    // Given the hit point in plane coordinates, return false if it is outside
    // the primitive, otherwise set the hit record UV coordinates and return
    // true.

    if (!unit_interval.contains(a) || !unit_interval.contains(b))
      return false;

    rec.u = a;
    rec.v = b;
    return true;
  }

private:
  vec3 Q;
  vec3 u, v;
  shared_ptr<Material> mat;
  AABB bbox;
  vec3 normal;
  // constant for plane definition Ax+By+Cz = D
  double D;
  // to help determine local coordinate
  vec3 w;
  double area;
};

// a & b are corners
inline shared_ptr<HittableList> box(const vec3 &a, const vec3 &b,
                                    shared_ptr<Material> mat) {
  // Returns the 3D box (six sides) that contains the two opposite vertices a &
  // b.

  auto sides = make_shared<HittableList>();

  // Construct the two opposite vertices with the minimum and maximum
  // coordinates.
  auto min =
      vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
  auto max =
      vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

  auto dx = vec3(max.x - min.x, 0, 0);
  auto dy = vec3(0, max.y - min.y, 0);
  auto dz = vec3(0, 0, max.z - min.z);

  sides->add(make_shared<Quad>(vec3(min.x, min.y, max.z), dx, dy,
                               mat)); // front
  sides->add(make_shared<Quad>(vec3(max.x, min.y, max.z), -dz, dy,
                               mat)); // right
  sides->add(make_shared<Quad>(vec3(max.x, min.y, min.z), -dx, dy,
                               mat)); // back
  sides->add(make_shared<Quad>(vec3(min.x, min.y, min.z), dz, dy,
                               mat)); // left
  sides->add(make_shared<Quad>(vec3(min.x, max.y, max.z), dx, -dz,
                               mat)); // top
  sides->add(make_shared<Quad>(vec3(min.x, min.y, min.z), dx, dz,
                               mat)); // bottom

  return sides;
}