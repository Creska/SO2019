# Bikaya - Phase 1

This phase implements the PCB and ASL functionality. It uses Linux style lists to arrange PCB in process queues and in trees. 
Every process queue is handled by a SEMD contained in the ASL (also managed through the same kind of lists), and both the process queue and the tree are relevant to ASL manipulation.

The content of a couple of files (types_bikaya and the test file) has been rearranged in order to avoid redundancy. 

See asl.c and pcb.c files for more notes on the implementation.

### Dependencies
- `arm-none-eabi-gcc` (arm cross compiler toolchain)
- `mipsel-linux-gnu-gcc` (mips cross compiler toolchain)
- `uarm` (arm emulator, [on github](https://github.com/mellotanica/uARM))
- `umps` (mps emulator, [on github](https://github.com/tjonjic/umps))
- `make`
- `cmake`

## Building the project
In this example we're building for the ARM architecture.

    mkdir build-arm
    cd build-arm
    cmake .. -DARCH=uarm     
    make
    
- Create a build dir and cd into it (CMake creates its build hierarchy in the working directory, this avoids cluttering of source folders).
- Run cmake passing it the path of the main *CMakeLists.txt* and a value for the `ARCH` variable (*uarm/arm* or *umps/umps2/mips*).
- Calling make the project is built.
- The compiled program can be run using the corresponding emulator (uarm in this case).

You can also set directly the path to the toolchain file (which defines the architecture-specific stuff for the build), in the previous example you would have to call cmake with:
    
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain/uarm.cmake

### Debug build type

The project can be built in debug mode to enable **DEBUG_LOG** printing and in general the inclusion of debug features 
that would be otherwise left out of compilation (for example the DEBUG_LOG macros are empty outside of debug mode).

The cmake cache variable `CMAKE_BUILD_TYPE` should be set to *Debug* (or *DEBUG*), you can do this with `ccmake` or `cmake-gui`.
This internally cause a -DDEBUG flag to be passed to the compiler.
Any other value for the cmake build type doesn't trigger the debug flag, therefore disables any debug features.

## Troubleshooting
- **Missing object files during CMake configuration**: this project needs a bunch of headers and object files for includes and linking
of the core libraries (provided by the emulators and mainly looked for in the toolchain files).
If one of this files is not found an error is logged and the project is not built.
It's possible to specify different paths through the **cache** (`ccmake .` or `cmake-gui .` in the build dir) 
and re-run the configuration step. 
If you find those files automatically installed in a different path in your distribution
you should add that path to the HINT field of the `find_file()`/`find_path()` function that looks for it,
since it probably is a common install location. 
- **Wrong CMake version**: I haven't tested this with older versions, so the CMake version requirements (first line of `./CMakeLists.txt`) are very strict.
If you're using an older version bump them down and test the system, it should be fine up to a point (probably at least down to 3.12).
If everything works we can update the requirements to a larger range.
- **Can't run ARM build, missing stub file / invalid kernel file**: you're probably using an older version of `uarm`, which requires an 
additional conversion of the kernel file, similarly to the command `umps2-elf2umps` for `umps2`.
Compile the latest version from [github](https://github.com/mellotanica/uARM), which should work without any conversion or additional file.

## Useful material
- **CMake**
    - [Official CMake documentation](https://cmake.org/cmake/help/v3.16/)
    - [Modern CMake guide](https://cliutils.gitlab.io/modern-cmake/)
    - [Cmake cheat sheet](http://www.brianlheim.com/2018/04/09/cmake-cheat-sheet.html)
    - [Official CMake docs about cross compiling](https://cmake.org/cmake/help/v3.16/manual/cmake-toolchains.7.html)
- **Emulators**
    - **uMPS**
        - [uMPS ui tour](http://www.cs.unibo.it/~renzo/so/umps2/umps2-ui-tour.pdf)
        - [uMPS principles of operation](http://www.cs.unibo.it/~renzo/so/princOfOperations.pdf)
    - **uARM**
        - [uARM introduction (italian)](http://mellotanica.github.io/uARM/uarm_intro.pdf)
        - [uARM manual](https://github.com/mellotanica/uARM/blob/doc/manual/manual.pdf)
