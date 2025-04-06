#include "bvh.h"
#include "camera.h"
#include "common.h"
#include "hittable.h"
#include "sphere.h"
#include <glm/glm.hpp>

int main() {
  Camera cam;

  // objects in world
  HittableList world;
  auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

  const int NUM = 11;
  for (int a = -NUM; a < NUM; a++) {
    for (int b = -NUM; b < NUM; b++) {
      auto choose_mat = random_double();
      vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = random_vec3() * random_vec3();
          sphere_material = make_shared<Lambertian>(albedo);
          //   world.add(make_shared<Sphere>(center, 0.2, sphere_material));
          auto center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = random_vec3(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<Dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<Dielectric>(1.5);
  world.add(make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

  // now world becomes a list of size 1, containing the root node
  // w/ bvh: ~20s
  // w/o bvh: ~140s
  world = HittableList(make_shared<BVHNode>(world));

  cam.render(world);

  return 0;
}