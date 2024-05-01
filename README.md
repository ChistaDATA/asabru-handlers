# asabru-handlers

### This repo contains various handlers used in the ChistaDATA Database Proxy ( chista-asabru ) 

This project contains code for the ChistaDATA database proxy handlers.

### Prerequisite

Make
`https://www.gnu.org/software/make`

CMake
`https://cmake.org/install/`

### Build

```
mkdir build
cd build
cmake ..
make
```

#### Build with local dependencies

```
mkdir build
cd build
cmake -DASABRU_COMMONS_BUILD=LOCAL_DIR -DASABRU_ENGINE_BUILD=LOCAL_DIR -DASABRU_PARSERS_BUILD=LOCAL_DIR ..
make
```
