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

### Running the Project

The project does not currently rely on user input. All options are passed through command
line arguments. To obtain a description of the available arguments, run the executable without
any arguments or with the `-h` or `--help` argument.

```
> ./cvrp

Solver for large CVRP instances from the LoggiBUD dataset
Usage:
  LoggiCVRP [OPTION...]

      --cvrp arg     [REQ] Path to CVRP JSON file
      --osm arg      [REQ] Path to OSM XML file
      --vmm          [OPT] Visualize map matching
      --vsp          [OPT] Visualize shortest paths (for depot point)
  -t, --threads arg  [OPT] Number of threads to use in shortest path calculation (default: 1)
  -h, --help         [OPT] Print usage
  -l, --logs         [OPT] Enable additional execution logs
      --quadtree     [OPT] Use quadtrees instead of k-d trees for map matching
      --bin-heap     [OPT] Use binary heaps instead of Fibonacci heaps for Dijkstra's algorithm
```

Arguments marked `[REQ]` are required, whilst arguments marked `[OPT]` are optional. The following snippet shows an example execution:

```
./cvrp --osm belem.xml --cvrp cvrp-0-pa-34.json --vmm --vsp -l -t 12
```

The program will run using the `belem.xml` OSM file, the `cvrp-0-pa-34.json` LoggiBUD
CVRP file. Visualization for both map matching (`--vmm`) and shortest paths (`--vsp`) is enabled. Shortest path calculation will use twelve threads (`-t 12`) and additional logs
will be printed to the screen (`-l`).
