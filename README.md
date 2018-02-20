# invm-frameworks

invm-frameworks is a framework library supporting a subset of
Internationalization (I18N) functionality, storage Command Line Interface (CLI)
applications, storage Common Information Model (CIM) providers.

For more information please visit our project home.
https://01.org/intel-nvm-frameworks

## Building

### Linux
```
mkdir output
cd output
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make -j all
sudo make install
```

RPMs can also be built:
```
make rpm
```

The RPMs will be in ./output/rpmbuild/RPMS/

### Windows
Install Microsoft Visual Studio 2017

Open the CMakeLists.txt as a CMake project

Build all with CMake
