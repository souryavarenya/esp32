# Instructions for Contributing

These are few guidelines to be kept in mind while writing a Ferret Library and this is an attempt to standardize the code-base.

### Basics

- Clone the `Blank_Ferret` directory and rename the project to start with `F`. (eg. `FMyLib`)

- Modify the project name in `CMakeLists.txt` and also `project.make` (for compatibility with older methods)

  For example, in the `CMakeLists.txt` file,

  ```
  cmake_minimum_required(VERSION 3.5)
  
  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
  project(FMyLib)
  ```

- Name the `.cpp` and `.hpp` files to be the lower case version of your project name. (eg. `fmylib.cpp` and `fmylib.hpp`)

### Coding 

- All the classes declared should be as short and indicative as possible and must be prefixed with `F` like `FClass` or `FMyClass`. The first-letter-capital scheme must be followed.





## Some Concepts

### Process

