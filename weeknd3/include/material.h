#pragma once
#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "texture.h"

// importance sampling record
class ISRecord {
public:
  color attenuation;
  shared_ptr<PDF> pdf_ptr;
  // when the scatter direction is determined, no need to perform pdf sample
  bool is_direction_determined;
  Ray skip_pdf_ray;
};

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       ISRecord &srec) const {
    return false;
  }

  // for importance sampling
  // As long as the weights are positive and add up to one, any such mixture of
  // PDFs is a PDF
  virtual double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                                const Ray &scattered) const {
    return 0;
  }

  virtual color emitted(const Ray &r_in, const HitRecord &rec, double u,
                        double v, const vec3 &p) const {
    return color(0, 0, 0);
  }
};

class Lambertian : public Material {
public:
  Lambertian(const color &albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
  Lambertian(std::shared_ptr<Texture> _tex) : tex(_tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec,
               ISRecord &srec) const override {
    srec.attenuation = tex->get_value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = make_shared<CosinePDF>(rec.normal);
    srec.is_direction_determined = false;
    return true;

  }

  double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                        const Ray &scattered) const override {
    auto cos_theta = glm::dot(rec.normal, scattered.normalizedDirection());
    // this pdf is derived from the assumption that scattering of a Lambertian
    // surface is proportional to cos\theta
    // the assumption is from the Lambertian property that the scattered light
    // obeys a distribution of cos\theta (remember how we calculate Lambertian
    // intensity)
    return cos_theta < 0 ? 0 : cos_theta / PI;
  }

private:
  std::shared_ptr<Texture> tex;
};

class Metal : public Material {
public:
  Metal(const color &_albedo, double _fuzz)
      : albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec,
               ISRecord &srec) const override {
    auto reflected = reflect(r_in.direction(), rec.normal);
    reflected = glm::normalize(reflected) + (fuzz * random_unit_vec3());

    srec.attenuation = albedo;
    srec.pdf_ptr = nullptr;
    // the direction is determined
    srec.is_direction_determined = true;
    srec.skip_pdf_ray = Ray(rec.p, reflected, r_in.time());
    return true;
  }

private:
  color albedo;
  floating fuzz;
};

class Dielectric : public Material {
public:
  Dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const Ray &r_in, const HitRecord &rec,
               ISRecord &srec) const override {
    srec.attenuation = color(1.0, 1.0, 1.0);
    srec.pdf_ptr = nullptr;
    srec.is_direction_determined = true;
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

    srec.skip_pdf_ray = Ray(rec.p, direction, r_in.time());
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

  // do not consider light falloff here
  color emitted(const Ray &r_in, const HitRecord &rec, double u, double v,
                const vec3 &p) const override {
    // only emitted on one side
    return rec.is_front_face ? tex->get_value(u, v, p) : color(0, 0, 0);
  }

private:
  shared_ptr<Texture> tex;
};

// same in all directions
class Isotropic : public Material {
public:
  Isotropic(const color &albedo) : tex(make_shared<SolidColor>(albedo)) {}
  Isotropic(shared_ptr<Texture> tex) : tex(tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec,
               ISRecord &srec) const override {
    // random scatter direction rather than calculation based on normal here
    srec.attenuation = tex->get_value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = make_shared<SpherePDF>();
    srec.is_direction_determined = false;
    return true;
  }

  double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                        const Ray &scattered) const override {
    return 1 / (4 * PI);
  }

private:
  shared_ptr<Texture> tex;
};