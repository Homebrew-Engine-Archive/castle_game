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
* Source and headers both should be all lower case, shouldn't contain spaces and be written ideally without underscores.
There are some true \m/ names:
`longlivethequeen.h`
`longlivethequeen.cpp`
`long_live_the_queen.h`
`long_live_the_queen.cpp`
* Header and source both live in the same directory.
* Explicit variable's type specification is preferrable (e.g. auto or decltype).
* Explicit namespace specification is preferrable.
* Use C++-style casts, or even better don't use it at all.
* Use annonymous namespaces for your static methods, types and constants.
* Use scoped resource management (smart pointers, lockers).
* Feel free to throw exceptions until you got an arrow in your knee.
* Your exceptions should be base of std::exception.
* Keep in mind "The Rule of Three"

1.3. Formatting rules
---------------------
* Use K&R with 4 spaces instead of tabs.
* CamelCase for class and function names.
* mixedCase for variables.
* Optional: c_style_case_t for type aliases.
* The Long Lines are eligible, but be afraid of complicated function and type composition. Better use `auto` instead or just wrap line around.
* You may place `&`, `&&` and `*` either near type or identifier both.
* You may place `const` any side, but prefer following notations:
```
ReturnType const* FunctionName(const Argument &argumentWithName)
{
    // <body>
}
```
or
```
ReturnType const& FunctionName(Argument const&);
```

Here is simple header example:
```
#ifndef HEADER_H_
#define HEADER_H_

namespace Header
{
    class HeaderClass
    {
    public:
        explicit HeaderClass(int num);
    };
}

#endif
```
Here is simple function's body:
```
std::istream& ReadBook(std::istream &in, Game::Objects::Book &book)
{
    int numPages = 0;
    in >> numPages;

    book.SetPagesCount(numPages);
    
    for(Game::Objects::Page &page : book.Pages()) {
        ReadPage(in, page);
        if(!in) {
            break;
        }
    }
    
    if(!in) {
        throw std::runtime_error("Stop read your book, let's code.");
    }
    
    return in;
}
```

2. Install
==========

3. Run
======

4. License
==========

5. Troubleshooting
==================

6. Thanks
=========