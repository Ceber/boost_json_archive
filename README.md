# `boost::archive::json`
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

![Workflow](https://github.com/Ceber/boost_json_archive/actions/workflows/cmake-single-platform.yml/badge.svg)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/ceber68)

## Background

This library provides JSON input/output archive objects usable with [boost serialization](https://www.boost.org/doc/libs/1_75_0/libs/serialization/doc/serialization.html) module.

It is inspired by [json_archive](https://github.com/Greedysky/json_archive) and [boost_archive_json](https://github.com/briancairl/boost_archive_json), and essentially serves as "boost only" version for the isolated human-readable JSON serialization/de-serialization use-case with no other external dependencies. As such, no external header-only library is in use.

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
See "demos/" and "unit-tests/".

## Running demos
From build folder:
```
./bin/<demo>
```

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

### Limitations
- Multidimensional arrays are only supported with std::array (serializing `int[X][Y]` does not work).

### Contributors ✨
Special thanks to you !
<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tbody>
    <tr>
      <td align="center" valign="top" width="14.28%"><a href="http://www.tarnyko.net"><img src="https://avatars.githubusercontent.com/u/3727259?v=4?s=100" width="100px;" alt="Tarnyko"/><br /><sub><b>Tarnyko</b></sub></a><br /><a href="#infra-Tarnyko" title="Infrastructure (Hosting, Build-Tools, etc)">🚇</a> <a href="https://github.com/Ceber/boost_json_archive/commits?author=Tarnyko" title="Code">💻</a></td>
    </tr>
  </tbody>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
