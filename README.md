# `boost::archive::json`

![Workflow](https://github.com/Ceber/boost_json_archive/actions/workflows/cmake-single-platform.yml/badge.svg)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/ceber68)

## Background

This library provides JSON input/output archive objects usable with [boost serialization](https://www.boost.org/doc/libs/1_75_0/libs/serialization/doc/serialization.html) module.

It is inspired by [json_archive](https://github.com/Greedysky/json_archive) and [boost_archive_json](https://github.com/briancairl/boost_archive_json), and essentially serves as "boost only" version for the isolated human-readable JSON serialization/de-serialization use case with no other external dependencies. As such, no external header-only library is in use.

Instead, this library makes use of the [boost_json](https://www.boost.org/doc/libs/1_75_0/libs/json/doc/html/index.html) library as a backbone for serialization. This is a outstandingly easy to used and performant C++ JSON library.

## Availibilities
- Standard Types serialization
- Objects/Structs serialization
- Vector/Array serialization 
- Map serialization
- Polymorphic serialization
- CPack/STGZ Packaging
- Conan Package management
- Code coverage computation
- Code formatting computation

## Build


### CMake Build
From project root directory:
```
cmake -S . -B build/<any_subdirectory>
cmake --build build/<any_subdirectory>
```

### Conan Build
From project root directory:
```
conan create . -pr:b=default -pr:h=Linux_Release --build boost_json_archive --build missing
```


## Usage
See unit-tests.

## Running unit-tests

From build folder:
```
ctest --verbose

```


## Requirements
- C++17
- [boost >=1.75](https://www.boost.org/) [boost::serialization and boost::json]
- [gtest](https://github.com/google/googletest) [tests only]


## Notes

### Legacy C++ support

This library is written in C++17, but usage of syntax/STL features is fairly limited. Namely,
there is liberal usage of `if constexpr` over the older `enable_if` tricks for clarity.

If there is a demonstrated need for C++11/14 support, getting things in shape would be very doable.


### Build System Requirements

At least CMake v3.13.

### Known issues / Limitations

### Only tested on Linux
Probably only working on Linux ?

### Memory Leak in tests ?
I think I forgot to free some pointers...oups...
