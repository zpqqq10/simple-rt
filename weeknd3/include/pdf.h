#pragma once
#include "common.h"
#include "hittable.h"
#include "onb.h"

class PDF {
public:
  virtual ~PDF() {}

  virtual double value(const vec3 &direction) const = 0;
  virtual vec3 generate() const = 0;
};

class SpherePDF : public PDF {
public:
  SpherePDF() {}

  double value(const vec3 &direction) const override { return 1 / (4 * PI); }

  vec3 generate() const override { return random_unit_vec3(); }
};

class CosinePDF : public PDF {
public:
  CosinePDF(const vec3 &w) : uvw(w) {}

  double value(const vec3 &direction) const override {
    // such calculation conforms better to the property that **more rays should
    // be scattering toward the normal** on Lambertian surface
    auto cosine_theta = dot(glm::normalize(direction), uvw.w);
    return std::fmax(0, cosine_theta / PI);
  }

  vec3 generate() const override {
    return uvw.transform(random_cosine_direction());
  }

private:
  ONB uvw;
};

class HittablePDF : public PDF {
public:
  HittablePDF(const Hittable &_objects, const vec3 &_origin)
      : objects(_objects), origin(_origin) {}

  double value(const vec3 &direction) const override {
    return objects.pdf_value(origin, direction);
  }

  vec3 generate() const override { return objects.random(origin); }

private:
  const Hittable &objects;
  vec3 origin;
};

class MixturePDF : public PDF {
public:
  MixturePDF(shared_ptr<PDF> _p0, shared_ptr<PDF> _p1, double _mix_rate = 0.5)
      : p0(_p0), p1(_p1), mix_rate(_mix_rate) {}

  double value(const vec3 &direction) const override {
    return mix_rate * p0->value(direction) +
           (1 - mix_rate) * p1->value(direction);
  }

  vec3 generate() const override {
    return random_double() < mix_rate ? p0->generate() : p1->generate();
  }

private:
  shared_ptr<PDF> p0, p1;
  double mix_rate;
};