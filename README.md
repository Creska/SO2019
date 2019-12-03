# CMake cross compilation sketch

This is a sample project for cross-compilation.
It builds basically the same program as [this example repo](https://github.com/Maldus512/umps_uarm_hello_world/tree/example),
 but using CMake instead of make or scons.

There is only one big difference under the hood. While the example repo
distributes their architecture-specific headers and sources (`./uarm` and `./umps`), this
build system looks for headers and compiled libraries as included in the emulators installs
similarly to the last slides of [this](http://www.cs.unibo.it/~renzo/so/bikaya/phase0_2020.pdf).
I'm not actually sure if we should distribute that stuff ourselves or trust the building machine to have the 
emulators installed, we'll see.

## Usage
In this example we're building for the ARM architecture.

    mkdir build-arm
    cd build-arm
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain/arm.cmake
    make
    
- Create a build dir and cd into it (CMake creates its build hierarchy in the working directory, this avoids cluttering of source folders)
- Run cmake passing it the main **CMakeLists.txt** and a **Toolchain file**, which defines the architecture-specific stuff for the build
- Calling make the project is (hopefully) built

### Possible errors
- **Missing files**: CMake needs a bunch of headers and object files for includes and linking
of the core libraries (provided by the emulators and mainly looked for in the toolchain files).
If one of this files is not found an error is logged and the project is not built.
It's possible to specify different paths through the **cache** (`ccmake .` or `cmake-gui .` in the build dir) 
and re-run the configuration step.
If you find those files automatically installed in a different path in your distribution
you should add that path to the HINT field of the `find_file()`/`find_path()` function that looks for it,
since it probably is a common install location. 
- **Wrong CMake version**: I have't tested this with older versions, so the requirements (first line of `./CMakeLists.txt`) are very strict.
If you're using an older version bump them down and test the system, it should be fine up to a point.
If everything works we can update the requirements to a larger range.

## Known problems
- **Can't run ARM build**: I wasn't able to run an ARM build, even from the example repo It seems to be missing a file.
Probably I'm missing something about the `uarm` tool and it has nothing to do with the build system.
    
    
## Info about CMake
- [Modern CMake guide](https://cliutils.gitlab.io/modern-cmake/)
- [Official CMake documentation](https://cmake.org/cmake/help/v3.16/)
- [Official CMake docs about cross compiling](https://cmake.org/cmake/help/v3.16/manual/cmake-toolchains.7.html)
    
