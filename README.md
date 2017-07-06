# invm-frameworks 

invm-frameworks is a framework library supporting a subset of
Internationalization (I18N) functionality, storage Command Line Interface (CLI)
applications, storage Common Information Model (CIM) providers.

For more information please visit our project home.
https://01.org/intel-nvm-frameworks

## Building
CMake is used to generate Unix Makefiles, then GNU Make is used to build for Linux and Windows. As such we recommend mingw_w64 and msys be used to build in a windows environment. Both of which can be found at mingw.org

### Linux
The recommend way to build is:
```
mkdir output
cd output
cmake ..
make -j all
```

### Windows
The recommend way for building on Windows is to use the CMake Unix Makefiles generator.

```
mkdir output
cd output
cmake .. -G "Unix Makefiels"
make -j all
```