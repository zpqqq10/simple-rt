#pragma once
#include "hittable.h"

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       color &attenuation, Ray &scattered) const {
    return false;
  }
};

class Lambertian : public Material {
public:
  Lambertian(const color &albedo) : albedo(albedo) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    auto scatter_direction = rec.get_normal() + random_unit_vec3();
    // to avoid the situation when the sample direcion is opposite to normal
    scatter_direction =
        near_zero(scatter_direction) ? rec.get_normal() : scatter_direction;
    scattered = Ray(rec.get_p(), scatter_direction);
    attenuation = albedo;
    return true;
  }

private:
  color albedo;
};

class Metal : public Material {
public:
  Metal(const color &_albedo, double _fuzz)
      : albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    auto reflected = reflect(r_in.direction(), rec.get_normal());
    reflected = glm::normalize(reflected) + (fuzz * random_unit_vec3());
    scattered = Ray(rec.get_p(), reflected);
    attenuation = albedo;
    // to control the direciton range
    return glm::dot(scattered.direction(), rec.get_normal()) > 0;
  }

private:
  color albedo;
  floating fuzz;
};

class Dielectric : public Material {
public:
  Dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    attenuation = color(1.0, 1.0, 1.0);
    double ri = rec.front_face() ? (1.0 / refraction_index) : refraction_index;

    auto unit_direction = glm::normalize(r_in.direction());
    // to determine *total internal reflection* 全反射
    double cos_theta = std::fmin(dot(-unit_direction, rec.get_normal()), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    auto direction =
        cannot_refract || reflectance(cos_theta, ri) > random_double()
            ? reflect(unit_direction, rec.get_normal())
            : refract(unit_direction, rec.get_normal(), ri);

    scattered = Ray(rec.get_p(), direction);
    return true;
  }

private:
  // Refractive index in vacuum or air, or the ratio of the material's
  // refractive index over the refractive index of the enclosing media
  double refraction_index;

  // Schlick Approximation on glass
  static double reflectance(double cosine, double refraction_index) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};