#pragma once
#include "aabb.h"
#include "hittable.h"
#include <algorithm>

class BVHNode : public Hittable {
public:
  BVHNode(HittableList list) : BVHNode(list.objects, 0, list.objects.size()) {
    // There's a C++ subtlety here. This constructor (without span indices)
    // creates an implicit copy of the hittable list, which we will modify. The
    // lifetime of the copied list only extends until this constructor exits.
    // That's OK, because we only need to persist the resulting bounding volume
    // hierarchy.
  }

  // build from top to bottom
  // another way is to build from bottom and can be parallelized
  BVHNode(std::vector<shared_ptr<Hittable>> &objects, size_t start,
          size_t end) {
    // object median split here, which is quite good(at least better than space
    // median split)
    bbox = AABB::get_empty();
    // only iterate the objects that needs to sort
    for (size_t index = start; index < end; ++index) {
      bbox = AABB(bbox, objects[index]->get_bbox());
    }
    int axis = bbox.longest_axis();
    auto comparator = (axis == 0)
                          ? box_x_compare
                          : ((axis == 1) ? box_y_compare : box_z_compare);

    size_t span = end - start;
    if (span == 1) {
      // only 1 object, no need to build a tree
      left = right = objects[start];
    } else if (span == 2) {
      // only 2 objects, build a simple tree
      left = objects[start];
      right = objects[start + 1];
    } else {
      auto mid = start + span / 2;
      // std::sort(std::begin(objects) + start, std::begin(objects) + end,
      //           comparator);
      std::nth_element(std::begin(objects) + start, std::begin(objects) + mid,
                       std::begin(objects) + end, comparator);

      left = make_shared<BVHNode>(objects, start, mid);
      right = make_shared<BVHNode>(objects, mid, end);
    }

    // bbox = AABB(left->get_bbox(), right->get_bbox());
  }

  virtual bool hit(const Ray &r, const Interval &ray_t,
                   HitRecord &rec) const override {
    if (!bbox.hit(r, ray_t))
      return false;

    // if the leaves are reached(exactly spheres), enter ray-tracing calculation
    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(
        r, Interval(ray_t.min, hit_left ? rec.get_t() : ray_t.max), rec);

    return hit_left || hit_right;
  }

  virtual AABB get_bbox() const override { return bbox; }

private:
  shared_ptr<Hittable> left;
  shared_ptr<Hittable> right;
  AABB bbox;

  static inline bool box_compare(const shared_ptr<Hittable> a,
                                 const shared_ptr<Hittable> b, int axis_index) {
    auto a_axis_interval = a->get_bbox().axis_interval(axis_index);
    auto b_axis_interval = b->get_bbox().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
  }
};
