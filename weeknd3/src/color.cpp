#include "color.h"
#include "interval.h"
#include <cstring>
#include <string>

double linear2gamma(double component){
  static const double factor = 1. / 2.2;
  // use power instead of sqrt here
  return component > 0 ? std::pow(component, factor) : 0;
}

void write_color(std::ostream &output, const color &pixel_color) {
  static const Interval intensity(0., 1);

  auto r = linear2gamma(pixel_color.r);
  auto g = linear2gamma(pixel_color.g);
  auto b = linear2gamma(pixel_color.b);
  int rbyte = int(255 * intensity.clamp(r));
  int gbyte = int(255 * intensity.clamp(g));
  int bbyte = int(255 * intensity.clamp(b));

  // int rbyte = (int)(255.999 * pixel_color.r);
  // int gbyte = (int)(255.999 * pixel_color.g);
  // int bbyte = (int)(255.999 * pixel_color.b);

  std::cout << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
  return;
}