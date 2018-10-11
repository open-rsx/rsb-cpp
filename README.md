# Introduction #

This repository contains the C++ implementation of the [Robotics Service Bus](https://github.com/open-rsx) middleware.

# Dependencies #

- [CMake](https://cmake.org/) build systen
- [Boost C++ libraries](https://www.boost.org/)
- [The RSC C++ library](https://github.com/open-rsx/rsc)
- The RSB protocol definitions, but this dependency will be removed

# Building and Installing #

RSB uses the default CMake workflow (assuming the RSC library has
already been installed into `<prefix>`):

```sh
cd <path to code>/build/
cmake -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=<prefix> ..
make
make test       # opt. run unit tests
make doc        # opt. create doxygen documentation
make cppcheck   # opt. static code analysis
make install    # install
```

To generate a coverage report chose `coverage` as CMake build type and do

```sh
make
make test
make coverage
```

# Contributing #

If you want to contribute to this project, please

- Submit your intended changes as coherent pull requests
- Rebase onto the master branch and squash any fixups and corrections
- Make sure the unit tests pass (See [Building and Installing](#building-and-installing))

# Acknowledgments #

The development of this software has been supported as follows:

- This research was funded by the EC 7th Framework Programme (FP7/2007-2013), in the TA2 (grant agreement ICT-2007-214 793) and HUMAVIPS (grant aggrement ICT-2009-247525) projects.
- The development of this software was supported by CoR-Lab, Research Institute for Cognition and Robotics Bielefeld University.
- This work was supported by the Cluster of Excellence Cognitive Interaction Technology ‘CITEC’ (EXC 277) at Bielefeld University, which is funded by the German Research Foundation (DFG).
