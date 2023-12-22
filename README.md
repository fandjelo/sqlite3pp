# SQLite3pp

SQLite3pp is a simple to use and low overhead C++ wrapper around SQLite3
library. It manages resources automatically using RAII principle and makes it
very easy to extract data from the databases directly into plain datatypes and
STL containers.

## How to build?

SQLite3pp is written in C++17 and utilizes CMake (version >=3.18) incl. required
package generation. The project also contains conan file (for version 2) and can
be built using that. SQLite3pp depends only on SQLite3, but optionally needs
gtest for testing.

Building with CMake
```bash
cd sqlite3pp
cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
cmkae --build build
```

Building with Conan
```bash
cd sqlite3pp
conan build .
```

## Build options

Beside the usual build flags for shared/static/dynamic libraries, as well as 
Debug/Release builds, which are used with CMake and/or Conan, the project brings
custom options, which can be used to control the build.

 Flag                    | Values     | Default | Description
-------------------------|------------|---------|-----------------------------
 SQLITE3PP_WITH_TESTS    | True/False | True    | Build with GTest Unit-Tests    
 SQLITE3OO_WITH_EXAMPLES | True/False | True    | Build with examples 

When building without tests the build scripts will not search for GTest, so if
you build on a system where this is not available, may be this is something for
you. To build SQLite3pp using CMake without tests, run:

```bash
cd sqlite3pp
cmake -B build -S . -D CMAKE_BUILD_TYPE=Release -D SQLITE3PP_WITH_TESTS=False
cmkae --build build
```

## How to use in your project?

If using CMake, just prebuild SQLite3pp for your environment and use the usual
way of finding packages in CMake:
```cmake
find_package(sqlite3pp REQUIRED)
target_link_libraries(mytarget sqlite3pp::sqlite3pp)
```
Then just use it somewhere in your C++ code:
```c++
#include <sqlite3pp/Database.h>

int main() {
   sqlite3pp::Database db {"file.db"};
   db.execute("CREATE TABLE foo (id,name)");
   db.execute("INSERT INTO foo VALUES (1,'one'),(2,'two'),(3,'three')");

   // Read one integer
   const auto id = db.execute<int>("SELECT id FROM foo LIMIT 1");

   // ...
}
```
For more examples see [examples file](src/examples/main.cpp)

