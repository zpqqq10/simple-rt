#include "bvh.h"
#include "camera.h"
#include "common.h"
#include "hittable.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"
#include <glm/glm.hpp>

void cornell_box() {
  HittableList world;
  HittableList lights;

  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  // bigger than 1 to ensure intensity
  auto light = make_shared<DiffuseLight>(color(15, 15, 15));
  shared_ptr<Material> aluminum =
      make_shared<Metal>(color(0.8, 0.85, 0.88), 0.0);
  auto empty_material = shared_ptr<Material>();

  world.add(make_shared<Quad>(
      QUAD_LIGHT_MIN, vec3(QUAD_LIGHT_MAX.x - QUAD_LIGHT_MIN.x, 0, 0),
      vec3(0, 0, QUAD_LIGHT_MAX.z - QUAD_LIGHT_MIN.z), light));
  lights.add(make_shared<Quad>(
      QUAD_LIGHT_MAX, vec3(QUAD_LIGHT_MIN.x - QUAD_LIGHT_MAX.x, 0, 0),
      vec3(0, 0, QUAD_LIGHT_MIN.z - QUAD_LIGHT_MAX.z), empty_material));

  world.add(make_shared<Quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555),
                              green));
  world.add(
      make_shared<Quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
  world.add(make_shared<Quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<Quad>(vec3(555, 555, 555), vec3(-555, 0, 0),
                              vec3(0, 0, -555), white));
  world.add(make_shared<Quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0),
                              white));

  //   shared_ptr<Hittable> box1 = box(vec3(0, 0, 0), vec3(165, 330, 165),
  //   aluminum);
  shared_ptr<Hittable> box1 = box(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<RotateY>(box1, 15);
  box1 = make_shared<Translate>(box1, vec3(265, 0, 295));
  world.add(box1);

  shared_ptr<Hittable> box2 = box(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<RotateY>(box2, -18);
  box2 = make_shared<Translate>(box2, vec3(130, 0, 65));
  world.add(box2);

  Camera cam(640, 640, lights, 1024, 50, 40, vec3(278, 278, -800),
             vec3(278, 278, 0), vec3(0, 1, 0), 0, 10, vec3(0, 0, 0));

  cam.render(world);
}

int main() { cornell_box(); }