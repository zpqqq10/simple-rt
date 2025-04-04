#pragma once
#include "common.h"

class Interval {
public:
  double min, max;

  Interval() : min(+infinity), max(-infinity) {}

  Interval(double _min, double _max) : min(_min), max(_max) {}

  double size() const { return max - min; }

  bool contains(const double x) const { return min <= x && x <= max; }

  bool surrounds(const double x) const { return min < x && x < max; }

  double clamp(const double x) const {
    return (x < min) ? min : ((x > max) ? max : x);
  }

  // singleton
  static Interval get_empty() {
    static Interval empty = Interval(+infinity, -infinity);
    return empty;
  }

  // singleton
  static Interval get_universe() {
    static Interval universe = Interval(-infinity, +infinity);
    return universe;
  }

  // singleton
  static Interval get_positive() {
    // to avoid shadow acne
    // due to floating precision, if the intersection point is inside the
    // sphere, 0 will make the next intersection on the sphere rather than other
    // objects
    // the image will get brighter
    static Interval positive = Interval(1e-3, +infinity);
    return positive;
  }

  //   static const Interval empty, universe;
};

// const Interval Interval::empty = Interval(+infinity, -infinity);
// const Interval Interval::universe = Interval(-infinity, +infinity);