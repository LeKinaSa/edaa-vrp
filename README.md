# Capacitated Vehicle Routing Problem

### Group 2

- Clara Martins
- Daniel Monteiro
- Gonçalo Pascoal

### Dependencies

**External Dependencies** (must be installed in the user's computer in order to compile and run the program)

- CMake
- C++ Compiler (for example, GCC)
- SFML (`libsfml-dev` package on Ubuntu / Debian based operating systems): used by the GraphViewerCpp internal dependency

**Internal Dependencies** (are already included in the project; their installation is not required)

- `nlohmann/json`: C++ library for parsing JSON files ([GitHub](https://github.com/nlohmann/json))
- `tinyxml2`: C++ library for parsing XML files([GitHub](https://github.com/leethomason/tinyxml2))
- `GraphViewerCpp`: C++ reimplementation of GraphViewer by [@dmfrodrigues](https://github.com/dmfrodrigues/GraphViewerCpp)
- `cxxopts`: Lightweight command line argument library ([GitHub](https://github.com/jarro2783/cxxopts))

### Building the Project

Create an empty directory, for example a `build` directory, and `cd` into it.
```
mkdir build
cd build
```

Initialize CMake and build the project (use `..` to reference the parent directory of `build`, which should contain the `CMakeLists.txt` file)
```
cmake ..
cmake --build .
```

TODO: run instructions