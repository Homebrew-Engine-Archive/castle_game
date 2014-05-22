1. Coding style conventions
===========================
1.1. General rules
-----------------
* C++ Header extension is `.h`.
* C++ Source code files extension is `.cpp`.
* Source and headers both should be all lower case, shouldn't contain spaces and be written ideally without underscores.
Here are some truly \m/ formatted names:
`yourjamsir.h`
`interfacecastle.cpp`
`archers_ready.h`
* Place your headers and sources in the same directory.
* Only UTF-8 in source files. Seriously.

1.2. C++ Exceptions
-------------------
* Feel free to throw errors, misunderstandings, just throw.
* Derive your exceptions from std::exception.
* Scoped resource management is better than manual.

1.3. Boost Features
--------------
* Boost at your service, Sir.

1.4. C++11 Features
-------------------
* Freely use any C++11 features. It's your future!
* `auto` is unwelcome. `auto&` and `auto*` either.
* Use anonymous namespaces for your static methods, types and constants.
* Look at `constexpr`. It's better than simply inline your functions.

1.5. Namespaces
--------------
* Explicit namespace is better than usings.
* `using po = boost::program_options` is better than `boost::program_options::options_descriptions = boo...`

1.6. Types
----------
* C++-style casts is better than C-style.

1.7. Declarations and Definitions.
------------------------------------------------------
* Forward declare your classes, enums and functions.
* Avoid multiple variable declaration.
* `const` is encouraged.
* `constexpr`, `template` and `inline` functions might be placed where declared. Either classes.
* Encapsulating your classes into namespaces is encouraged.

1.8. Formatting rules
---------------------
* General formatting rule - K&R with 4 spaces.
* CamelCase for classes and functions.
* mixedCase for variables.
* Prefix class mMembers with just single `m...` or none.
* Namespaces should consist of single word either capitalized or not.
* underscores_style_t for type aliases.
* The long lines are eligible, but be aware of complicated function and type composition. Better use `auto` instead or just wrap line around.
* You may place `&`, `&&` and `*` whereever you want.
* You may place variable specifiers anywhere, but prefer the following notations:
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
        explicit constexpr HeaderClass(int num);
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
Your tired, my Lord?