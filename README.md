# SIMPLE-RT

A simple ray tracing program following [RayTracingInOneWeekend](https://raytracing.github.io/).

- implement with glm
- compile with cmake
- reorganize and rename according to my personal habits
- developed with vscode and clangd

## Running

Download glm 1.0.0-light from [glm-releases](https://github.com/g-truc/glm/releases) and place the extracted `glm` under `include`.

To run the first weekend:

```shell
cd weeknd1/weeknd2/weeknd3
mkdir build && cd build
make
./test >> image.ppm
```

A reminder: `glm::length()` returns the **length** of a vector, and `foo.length()` returns the **dimension** of a vector.

## Comments on book3

Code of book3 int this repo is half finished. Book3 is not as good as the book1 and book2 for the following reasons, in my opinions:

1. Code is not well organized like there is too much hardcoding.

2. The procedure is in a mess, for example `pdf_value()` and `random()` of `HittableList` should be implemented right after `Hittable` and `Quad`, or the `HittableList lights` would call `pdf_value()` of the base class and cause numerical problems.

3. Explanation is not clear. Take the explanation on Figure 11 as an example, and where does $\theta$ comes from?
