# Chess UI
This is a work-in-progress chess GUI written in C++ using the Qt 6 framework. The eventual goal is to support the UCI protocol to allow for the use of any engine, but it currently only supports [this](https://github.com/boriskrisanov/chess_engine) engine by compiling it as a static library. This is still in the early stages of development, so there may be bugs or layout issues and many things need to be refactored.

<img width="960" alt="screenshot" src="https://github.com/user-attachments/assets/d1d3b84e-a595-445c-8244-eb01081422e0"/>

## Compiling
This project uses CMake, so after installing Qt 6, simply run
```shell
$ cmake -S . -B build
$ cmake --build build
```
Make sure to clone with `--recursive` to also clone the engine submodule.

