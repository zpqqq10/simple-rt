#pragma once

#include "hittable.h"
#include "material.h"

class Camera {

  int image_width = 1280;
  int image_height = 720;
  vec3 center;
  vec3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
  // for recursion security
  int max_depth = 48;
  // for anti-aliasing
  int samples_per_pixel = 32;
  floating pixel_sample_scale;
  floating vfov = 20; // Vertical view angle (field of view)

  // vec3 lookfrom = vec3(0, 0, 0); // Point camera is looking from
  // vec3 lookat = vec3(0, 0, -1);  // Point camera is looking at
  // vec3 vup = vec3(0, 1, 0);      // Camera-relative "up"
  // direction
  vec3 lookfrom = vec3(13, 2, 3); // Point camera is looking from
  vec3 lookat = vec3(0, 0, 0);    // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);       // Camera-relative "up" direction
  vec3 u, v, w;                   // Camera frame basis vectors

  // for defocus blur
  // the size of the aperture
  floating defocus_angle = 0.6; // Variation angle of rays through each pixel
  floating focus_dist =
      10; // Distance from camera lookfrom point to plane of perfect focus

  vec3 defocus_disk_u; // Defocus disk horizontal radius
  vec3 defocus_disk_v; // Defocus disk vertical radius

  void initialize() {
    // Camera
    auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees2radians(vfov);
    auto h = (floating)std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    // auto viewport_height = 2 * h * focal_length;
    // auto viewport_height = 2.0;
    auto viewport_width =
        (floating)viewport_height * (floating(image_width) / image_height);
    center = lookfrom;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = glm::normalize(lookfrom - lookat);
    u = glm::normalize(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    auto viewport_u = viewport_width * u;
    auto viewport_v = -viewport_height * v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    // in fact u, v should be ranged from [-1, 1]/[0, 1]?
    pixel_delta_u = viewport_u / (floating)image_width;
    pixel_delta_v = viewport_v / (floating)image_height;

    // Calculate the location of the upper left pixel.
    // simple geometry
    auto viewport_upper_left = center - focus_dist * w -
                               viewport_u / (floating)2. -
                               viewport_v / (floating)2.;
    //  center of the pixel
    pixel00_loc =
        viewport_upper_left + (floating)0.5 * (pixel_delta_u + pixel_delta_v);

    // for anti-aliasing
    pixel_sample_scale = 1. / samples_per_pixel;

    // camera defocus disk
    // a possibly incorrect interpretation: the mathematical relationship
    // guarantees that the **point on the focus plane are less likely to be
    // sampled by other pixels**
    // adjacent pixels may sample the same points that not on the focus plane
    // and blur appears
    auto defocus_radius =
        focus_dist * (floating)std::tan(degrees2radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  color ray_color(const Ray &r, const int depth,
                  const Hittable &objects) const {
    if (depth <= 0) {
      // this ray has experienced so much intersection, it should be so dark
      return color(0., 0., 0.);
    }
    HitRecord rec;
    if (objects.hit(r, Interval::get_positive(), rec)) {
      Ray scattered;
      color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, objects);
      }
      // ? hit nothing, in fact it would be better to set background color here?
      return color(0., 0., 0.);
      // sample diffuse direction w/o materials, change sample distribution
      // instead of vec3 diffuse_direction =
      // random_hemisphere(rec.get_normal()); vec3 diffuse_direction =
      // rec.get_normal() + random_unit_dvec3(); return 0.5 *
      //        ray_color(Ray(rec.get_p(), diffuse_direction), depth - 1,
      //        objects);
    }

    // background color
    auto weight = 0.5 * (r.normalizedDirection().y + 1.);
    return floating(1. - weight) * color(1., 1., 1.) +
           floating(weight) * color(.5, .7, 1.);
  }

  Ray get_ray(int i, int j) const {
    // Construct a camera ray originating from the origin and directed at
    // randomly sampled point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x) * pixel_delta_u) +
                        ((j + offset.y) * pixel_delta_v);

    // from camera to sample position
    // Construct a camera ray originating from the defocus disk and directed at
    // a randomly sampled point around the pixel location i, j.
    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
  }

  vec2 sample_square() const {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
    // square.
    return vec2(random_double() - 0.5, random_double() - 0.5);
  }

  vec3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_circle();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

public:
  Camera() { initialize(); }

  void render(const Hittable &objects) {
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
      if (j % 200 == 0) {
        std::clog << "finish " << j << " lines\r" << std::flush;
      }
      for (int i = 0; i < image_width; ++i) {
        color final_color(0., 0., 0.);

        for (int sample = 0; sample < samples_per_pixel; ++sample) {
          Ray r = get_ray(i, j);
          final_color += ray_color(r, max_depth, objects);
        }

        // remember the weight
        write_color(std::cout, final_color * pixel_sample_scale);
      }
    }
  }
};