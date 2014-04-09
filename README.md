1. Coding style conventions
===========================
1.1. General rules
-----------------
* Avoid using platform-dependent components.
* C++11 and Boost at your service, sir.
* Only UTF-8.

1.2. C++ rules
--------------
* Headers should have .h extension.
* Source files should have .cpp extension.
* Source and headers both should be all lower case, shouldn't contain spaces and be written idealln without underscores.
Examples: `longlivethequeen.h` and `longlivethequeen.cpp` or `long_live_the_queen.h` and `long_live_the_queen.cpp`
* Header and source both live in the same directory.
* Explicit variable's type declaration is preferrable to implicit (e.g. auto or decltype).
* Fully qualified names are preferrable to usings.
* Prefer c++-style cast to c-style.
* Try to avoid casts.
* Split your code into namespaces. Unnamed ones instead static functions and named for extern.
* Prefer RAII and scoped memory management to new/delete.
* Prefer true/false result returning to throwing exceptions.
* Prefer throwing exceptions to returning error codes.
* You may place your inlines and templates into header as well as in source file.
* Keep in mind "The Rule of Three"

1.3. Formatting rules
---------------------
* K&R with 4 spaces instead of tabs.
* The Long Lines are eligible, but be afraid of complicated function and type composition. Better use auto instead or just wrap line around.
* Class name example: 
  `class ClassName;`
* Class member example: 
  `std::string mClassMember = "23";`
* Function name example: 
  `void FunctionName();`
* Variable name example: 
  `int mixedCaseName = 42;`
* Control statement example: 
  `for(int i = 0; i < n; ++i) {`
