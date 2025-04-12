#pragma once
#include "common.h"
#include "perlin.h"
// #include "image_loader.h"

class Texture {
public:
  virtual ~Texture() = default;

  virtual color get_value(double u, double v, const vec3 &p) const = 0;
};

// in place of a simple albedo
class SolidColor : public Texture {
public:
  SolidColor(const color &albedo) : albedo(albedo) {}

  SolidColor(double red, double green, double blue)
      : SolidColor(color(red, green, blue)) {}

  color get_value(double u, double v, const vec3 &p) const override {
    return albedo;
  }

private:
  color albedo;
};

class CheckerTexture : public Texture {
public:
  CheckerTexture(double scale, shared_ptr<Texture> even,
                 shared_ptr<Texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

  CheckerTexture(double scale, const color &c1, const color &c2)
      : CheckerTexture(scale, make_shared<SolidColor>(c1),
                       make_shared<SolidColor>(c2)) {}

  color get_value(double u, double v, const vec3 &p) const override {
    auto xInteger = int(std::floor(inv_scale * p.x));
    auto yInteger = int(std::floor(inv_scale * p.y));
    auto zInteger = int(std::floor(inv_scale * p.z));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->get_value(u, v, p) : odd->get_value(u, v, p);
  }

private:
  double inv_scale;
  shared_ptr<Texture> even;
  shared_ptr<Texture> odd;
};

// report error due to clangd but we can compile as usual
// so comment it out for convenience temporarily
// class ImageTexture : public Texture {
// public:
//   ImageTexture(const char *filename) : image(filename) {}

//   color get_value(double u, double v, const vec3 &p) const override {
//     // If we have no texture data, then return solid cyan as a debugging aid.
//     if (image.height() <= 0)
//       return color(0, 1, 1);

//     // Clamp input texture coordinates to [0,1] x [1,0]
//     u = Interval(0, 1).clamp(u);
//     v = 1.0 - Interval(0, 1).clamp(v); // Flip V to image coordinates

//     auto i = int(u * image.width());
//     auto j = int(v * image.height());
//     auto pixel = image.pixel_data(i, j);

//     auto color_scale = 1.0 / 255.0;
//     return color(color_scale * pixel[0], color_scale * pixel[1],
//                  color_scale * pixel[2]);
//   }

// private:
//   ImageLoader image;
// };

class NoiseTexture : public Texture {
public:
  NoiseTexture() {}
  NoiseTexture(const floating _scale, const int _depth,
               const bool _use_turb = true, const bool _marbled = true)
      : scale(_scale), turb_depth(_depth), use_turb(_use_turb),
        marbled(_marbled) {}

  color get_value(double u, double v, const vec3 &p) const override {
    // return color(1, 1, 1) * noise.noise(scale * p);
    // perlin interpolation can return negative values
    return use_turb
               ? (marbled ? color(.5, .5, .5) *
                                (1 + std::sin(scale * p.z +
                                              10 * noise.turb(p, turb_depth)))
                          : color(1, 1, 1) * noise.turb(p, turb_depth))
               : color(.5, .5, .5) * (noise.noise(scale * p) + 1);
  }

private:
  Perlin noise;
  floating scale;
  int turb_depth;
  bool use_turb;
  bool marbled;
};