# Bikaya - Phase 0

This is the first phase of a a lab project from an [SO course](http://www.cs.unibo.it/~renzo/so/) from the [University of Bologna](https://www.unibo.it/en).

The main goal of this phase is the setup of a multi-architecture build system. The actual logic is very 
simple and closely related to this [example project](https://github.com/Maldus512/umps_uarm_hello_world/tree/example).
While the example just writes to `terminal 0` a string, this program reads `terminal 0`'s input up to the first *newline* character
and prints the string to `printer 0`.

## Building the project
In this example we're building for the ARM architecture.

    mkdir build-arm
    cd build-arm
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain/arm.cmake
    make
    
- Create a build dir and cd into it (CMake creates its build hierarchy in the working directory, this avoids cluttering of source folders).
- Run cmake passing it the path of the main *CMakeLists.txt* and a *Toolchain file*, which defines the architecture-specific stuff for the build.
- Calling make the project is built.
- The compiled program can be run using the corresponding emulator (uarm in this case). While the `terminal 0` is automatically
enabled on the emulated machines you might need to enable `printer 0` in not already done in a previous run.

### Dependencies
- `arm-none-eabi-gcc` (arm cross compiler toolchain)
- `mipsel-linux-gnu-gcc` (mps cross compiler toolchain)
- `uarm` (arm emulator, [on github](https://github.com/mellotanica/uARM))
- `umps` (mps emulator, [on github](https://github.com/tjonjic/umps))
- `make`
- `cmake`

### Troubleshooting
- **Missing object files during CMake configuration**: this project needs a bunch of headers and object files for includes and linking
of the core libraries (provided by the emulators and mainly looked for in the toolchain files).
If one of this files is not found an error is logged and the project is not built.
It's possible to specify different paths through the **cache** (`ccmake .` or `cmake-gui .` in the build dir) 
and re-run the configuration step. 
If you find those files automatically installed in a different path in your distribution
you should add that path to the HINT field of the `find_file()`/`find_path()` function that looks for it,
since it probably is a common install location. 
- **Wrong CMake version**: I have't tested this with older versions, so the CMake version requirements (first line of `./CMakeLists.txt`) are very strict.
If you're using an older version bump them down and test the system, it should be fine up to a point (probably at least down to 3.12).
If everything works we can update the requirements to a larger range.
- **Can't run ARM build, missing stub file / invalid kernel file**: you're probably using an older version of `uarm`, which requires an 
additional conversion of the kernel file, similarly to the command `umps2-elf2umps` for `umps2`.
Compile the latest version from [github](https://github.com/mellotanica/uARM), which should work without any conversion or additional file.

## Useful material
- **CMake**
    - [Official CMake documentation](https://cmake.org/cmake/help/v3.16/)
    - [Introduction to CMake](http://www2.informatik.uni-freiburg.de/~jakobro/files/cmake-an-introduction.pdf)
    - [Modern CMake guide](https://cliutils.gitlab.io/modern-cmake/)
    - [Official CMake docs about cross compiling](https://cmake.org/cmake/help/v3.16/manual/cmake-toolchains.7.html)
- **Emulators**
    - **uMPS**
        - [uMPS ui tour](http://www.cs.unibo.it/~renzo/so/umps2/umps2-ui-tour.pdf)
        - [uMPS principles of operation](http://www.cs.unibo.it/~renzo/so/princOfOperations.pdf)
    - **uARM**
        - [uARM introduction (italian)](http://mellotanica.github.io/uARM/uarm_intro.pdf)
        - [uARM manual](https://github.com/mellotanica/uARM/blob/doc/manual/manual.pdf)
