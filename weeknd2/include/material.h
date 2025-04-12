#pragma once
#include "hittable.h"
#include "texture.h"

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       color &attenuation, Ray &scattered) const {
    return false;
  }

  virtual color emitted(double u, double v, const vec3 &p) const {
    return color(0, 0, 0);
  }
};

class Lambertian : public Material {
public:
  Lambertian(const color &albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
  Lambertian(std::shared_ptr<Texture> _tex) : tex(_tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    auto scatter_direction = rec.normal + random_unit_vec3();
    // to avoid the situation when the sample direcion is opposite to normal
    scatter_direction =
        near_zero(scatter_direction) ? rec.normal : scatter_direction;
    scattered = Ray(rec.p, scatter_direction, r_in.time());
    attenuation = tex->get_value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  std::shared_ptr<Texture> tex;
};

class Metal : public Material {
public:
  Metal(const color &_albedo, double _fuzz)
      : albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    auto reflected = reflect(r_in.direction(), rec.normal);
    reflected = glm::normalize(reflected) + (fuzz * random_unit_vec3());
    scattered = Ray(rec.p, reflected, r_in.time());
    attenuation = albedo;
    // to control the direciton range
    return glm::dot(scattered.direction(), rec.normal) > 0;
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
    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    auto direction =
        cannot_refract || reflectance(cos_theta, ri) > random_double()
            ? reflect(unit_direction, rec.normal)
            : refract(unit_direction, rec.normal, ri);

    scattered = Ray(rec.p, direction, r_in.time());
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

class DiffuseLight : public Material {
public:
  DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
  DiffuseLight(const color &emit) : tex(make_shared<SolidColor>(emit)) {}

  color emitted(double u, double v, const vec3 &p) const override {
    return tex->get_value(u, v, p);
  }

private:
  shared_ptr<Texture> tex;
};

// same in all directions
class Isotropic : public Material {
public:
  Isotropic(const color &albedo) : tex(make_shared<SolidColor>(albedo)) {}
  Isotropic(shared_ptr<Texture> tex) : tex(tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, color &attenuation,
               Ray &scattered) const override {
    // random scatter direction rather than calculation based on normal here
    scattered = Ray(rec.p, random_unit_vec3(), r_in.time());
    attenuation = tex->get_value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  shared_ptr<Texture> tex;
};