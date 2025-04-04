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
cd weeknd1
mkdir build && cd build
make
./test >> image.ppm
```
