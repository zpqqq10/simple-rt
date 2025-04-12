#include "bvh.h"
#include "camera.h"
#include "common.h"
#include "hittable.h"
#include "medium.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"
#include <glm/glm.hpp>

void bouncing_spheres() {
  Camera cam;

  // objects in world
  HittableList world;
  // auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  // world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

  auto checker =
      make_shared<CheckerTexture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
  world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(checker)));

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
}

void checkered_spheres() {
  HittableList world;

  auto checker =
      make_shared<CheckerTexture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

  world.add(make_shared<Sphere>(vec3(0, -10, 0), 10,
                                make_shared<Lambertian>(checker)));
  world.add(make_shared<Sphere>(vec3(0, 10, 0), 10,
                                make_shared<Lambertian>(checker)));

  Camera cam(1280, 720, 50, 50, 20, vec3(13, 2, 3), vec3(0, 0, 0),
             vec3(0, 1, 0), 0);

  cam.render(world);
}

void davis() {
  HittableList world;
  // auto earth_texture = make_shared<ImageTexture>("ad.jpg");
  // auto earth_surface = make_shared<Lambertian>(earth_texture);
  // auto globe = make_shared<Sphere>(vec3(0, 0, 0), 2, earth_surface);
  // world.add(globe);

  // head of ad!
  Camera cam(1280, 720, 50, 50, 20, vec3(10, 0, 6), vec3(0, 0, 0),
             vec3(0, 1, 0), 0);

  cam.render(world);
}

void perlin_spheres() {
  HittableList world;

  auto pertext = make_shared<NoiseTexture>(4, 5);
  world.add(make_shared<Sphere>(vec3(0, -1000, 0), 1000,
                                make_shared<Lambertian>(pertext)));
  world.add(
      make_shared<Sphere>(vec3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

  Camera cam(1280, 720, 50, 50, 20, vec3(13, 2, 3), vec3(0, 0, 0),
             vec3(0, 1, 0), 0);

  cam.render(world);
}

void quads() {
  HittableList world;

  // Materials
  auto left_red = make_shared<Lambertian>(color(1.0, 0.2, 0.2));
  auto back_green = make_shared<Lambertian>(color(0.2, 1.0, 0.2));
  auto right_blue = make_shared<Lambertian>(color(0.2, 0.2, 1.0));
  auto upper_orange = make_shared<Lambertian>(color(1.0, 0.5, 0.0));
  auto lower_teal = make_shared<Lambertian>(color(0.2, 0.8, 0.8));

  // Quads
  world.add(make_shared<Quad>(vec3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0),
                              left_red));
  world.add(make_shared<Quad>(vec3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0),
                              back_green));
  world.add(make_shared<Quad>(vec3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0),
                              right_blue));
  world.add(make_shared<Quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4),
                              upper_orange));
  world.add(make_shared<Quad>(vec3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4),
                              lower_teal));

  Camera cam(480, 480, 64, 50, 80, vec3(0, 0, 9), vec3(0, 0, 0), vec3(0, 1, 0),
             0);

  cam.render(world);
}

void cornell_box() {
  HittableList world;

  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  // bigger than 1 to ensure intensity
  auto light = make_shared<DiffuseLight>(color(15, 15, 15));

  world.add(make_shared<Quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555),
                              green));
  world.add(
      make_shared<Quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
  world.add(make_shared<Quad>(vec3(343, 554, 332), vec3(-130, 0, 0),
                              vec3(0, 0, -105), light));
  world.add(make_shared<Quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<Quad>(vec3(555, 555, 555), vec3(-555, 0, 0),
                              vec3(0, 0, -555), white));
  world.add(make_shared<Quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0),
                              white));

  shared_ptr<Hittable> box1 = box(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<RotateY>(box1, 15);
  box1 = make_shared<Translate>(box1, vec3(265, 0, 295));
  world.add(box1);

  shared_ptr<Hittable> box2 = box(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<RotateY>(box2, -18);
  box2 = make_shared<Translate>(box2, vec3(130, 0, 65));
  world.add(box2);

  Camera cam(640, 640, 256 * 2, 50, 40, vec3(278, 278, -800), vec3(278, 278, 0),
             vec3(0, 1, 0), 0, 10, vec3(0, 0, 0));

  cam.render(world);
}

void cornell_smoke() {
  HittableList world;

  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  auto light = make_shared<DiffuseLight>(color(7, 7, 7));

  world.add(make_shared<Quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555),
                              green));
  world.add(
      make_shared<Quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
  world.add(make_shared<Quad>(vec3(113, 554, 127), vec3(330, 0, 0),
                              vec3(0, 0, 305), light));
  world.add(make_shared<Quad>(vec3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<Quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<Quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0),
                              white));

  shared_ptr<Hittable> box1 = box(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<RotateY>(box1, 15);
  box1 = make_shared<Translate>(box1, vec3(265, 0, 295));

  shared_ptr<Hittable> box2 = box(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<RotateY>(box2, -18);
  box2 = make_shared<Translate>(box2, vec3(130, 0, 65));

  world.add(make_shared<ConstantMedium>(box1, 0.05, color(0, 0, 0)));
  world.add(make_shared<ConstantMedium>(box2, 0.05, color(1, 1, 1)));

  Camera cam(640, 640, 256, 50, 40, vec3(278, 278, -800), vec3(278, 278, 0),
             vec3(0, 1, 0), 0, 10, vec3(0, 0, 0));

  cam.render(world);
}

int main() {
  switch (6) {
  case 1:
    bouncing_spheres();
    break;
  case 2:
    checkered_spheres();
    break;
  case 3:
    davis();
    break;
  case 4:
    perlin_spheres();
    break;
  case 5:
    quads();
    break;
  case 6:
    cornell_box();
    break;
  case 7:
    cornell_smoke();
    break;
  }
}